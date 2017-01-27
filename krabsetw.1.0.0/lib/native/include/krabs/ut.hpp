// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "compiler_check.hpp"
#include "trace.hpp"
#include "provider.hpp"

namespace krabs { namespace details {

    /**
     * <summary>
     *   Used as a template argument to a trace instance. This class implements
     *   code paths for user traces. Should never be used or seen by client
     *   code.
     * </summary>
     */
    struct ut {

        typedef krabs::provider<> provider_type;

        /**
         * <summary>
         *   Used to assign a name to the trace instance that is being
         *   instantiated.
         * </summary>
         * <remarks>
         *   There really isn't a name policy to enforce with user traces, but
         *   kernel traces do have specific naming requirements.
         * </remarks>
         */
        static const std::wstring enforce_name_policy(
            const std::wstring &name);

        /**
         * <summary>
         *   Generates a value that fills the EnableFlags field in an
         *   EVENT_TRACE_PROPERTIES structure. This controls the providers that
         *   get enabled for a kernel trace. For a user trace, it doesn't do
         *   much of anything.
         * </summary>
         */
        static const unsigned long construct_enable_flags(
            const krabs::trace<krabs::details::ut> &trace);

        /**
         * <summary>
         *   Enables the providers that are attached to the given trace.
         * </summary>
         */
        static void enable_providers(
            const krabs::trace<krabs::details::ut> &trace);

        /**
         * <summary>
         *   Decides to forward an event to any of the providers in the trace.
         * </summary>
         */
        static void forward_events(
            const EVENT_RECORD &record,
            const krabs::trace<krabs::details::ut> &trace);

        /**
         * <summary>
         *   Sets the ETW trace log file mode.
         * </summary>
         */
        static unsigned long augment_file_mode();

        /**
         * <summary>
         *   Returns the GUID of the trace session.
         * </summary>
         */
        static krabs::guid get_trace_guid();
    };


    // Implementation
    // ------------------------------------------------------------------------

    inline const std::wstring ut::enforce_name_policy(
        const std::wstring &name_hint)
    {
        if (name_hint.empty()) {
            return std::to_wstring(krabs::guid::random_guid());
        }

        return name_hint;
    }

    inline const unsigned long ut::construct_enable_flags(
        const krabs::trace<krabs::details::ut> &)
    {
        return 0;
    }

    inline void ut::enable_providers(
        const krabs::trace<krabs::details::ut> &trace)
    {
        std::map<krabs::guid, std::tuple<UCHAR, ULONGLONG, ULONGLONG>> providerFlags;

        // This function essentially takes the union of all the provider flags
        // for a given provider GUID. This comes about when multiple providers
        // for the same GUID are provided and request different provider flags.
        // TODO: Only forward the calls that are requested to each provider.
        for (auto &provider : trace.providers_) {
            if (providerFlags.find(provider.get().guid_) != providerFlags.end()) {
                providerFlags[provider.get().guid_] = std::make_tuple (0, 0, 0);
            }

            std::get<0>(providerFlags[provider.get().guid_]) |= provider.get().level_;
            std::get<1>(providerFlags[provider.get().guid_]) |= provider.get().any_;
            std::get<2>(providerFlags[provider.get().guid_]) |= provider.get().all_;
        }

        for (auto &provider : providerFlags) {
            GUID guid = provider.first;
            ULONG status = EnableTraceEx2(trace.registrationHandle_,
                                          &guid,
                                          EVENT_CONTROL_CODE_ENABLE_PROVIDER,
                                          std::get<0>(provider.second),
                                          std::get<1>(provider.second),
                                          std::get<2>(provider.second),
                                          0,
                                          nullptr);
            UNREFERENCED_PARAMETER(status);
        }
    }

    inline void ut::forward_events(
        const EVENT_RECORD &record,
        const krabs::trace<krabs::details::ut> &trace)
    {
        for (auto &provider : trace.providers_) {
            if (record.EventHeader.ProviderId == provider.get().guid_) {
                provider.get().on_event(record);
            }
        }
    }

    inline unsigned long ut::augment_file_mode()
    {
        return 0;
    }

    inline krabs::guid ut::get_trace_guid()
    {
        return krabs::guid::random_guid();
    }

} /* namespace details */ } /* namespace krabs */
