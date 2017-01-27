// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include <evntcons.h>
#include <functional>
#include <deque>

#include "../compiler_check.hpp"

namespace krabs { namespace testing {
    class event_filter_proxy;
} /* namespace testing */} /* namespace krabs */

namespace krabs { namespace details {
    template <typename T> class base_provider;
} /* namespace details */} /* namespace krabs */


namespace krabs {

    typedef void(*c_provider_callback)(const EVENT_RECORD &);
    typedef std::function<void(const EVENT_RECORD &)> provider_callback;
    typedef std::function<bool(const EVENT_RECORD &)> filter_predicate;

    template <typename T> class provider;

    /**
     * <summary>
     *   Use this to provide event filtering before an event bubbles to
     *   specific callbacks.
     * </summary>
     * <remarks>
     *   Each event_filter has a single predicate (which can do complicated
     *   checks and logic on the event). All callbacks registered under the
     *   filter are invoked only if the predicate returns true for a given
     *   event.
     * </remarks>
     */
    class event_filter {
    public:

        /**
         * <summary>
         *   Constructs an event_filter that applies the given predicate to all
         *   events.
         * </summary>
         */
        event_filter(filter_predicate predicate);

        /**
         * <summary>
         * Adds a function to call when an event for this filter is fired.
         * </summary>
         */
        void add_on_event_callback(c_provider_callback callback);

        template <typename U>
        void add_on_event_callback(U &callback);

        template <typename U>
        void add_on_event_callback(const U &callback);

    private:

        /**
         * <summary>
         *   Called when an event occurs, forwards to callbacks if the event
         *   satisfies the predicate.
         * </summary>
         */
        void on_event(const EVENT_RECORD &record) const;

    private:
        std::deque<provider_callback> callbacks_;
        filter_predicate predicate_;

    private:
        template <typename T>
        friend class details::base_provider;

        friend class krabs::testing::event_filter_proxy;
    };

    // Implementation
    // ------------------------------------------------------------------------

    inline event_filter::event_filter(filter_predicate predicate)
    : predicate_(predicate)
    {}

    inline void event_filter::add_on_event_callback(c_provider_callback callback)
    {
        // C function pointers don't interact well with std::ref, so we
        // overload to take care of this scenario.
        callbacks_.push_back(callback);
    }

    template <typename U>
    void event_filter::add_on_event_callback(U &callback)
    {
        // std::function copies its argument -- because our callbacks list
        // is a list of std::function, this causes problems when a user
        // intended for their particular instance to be called.
        // std::ref lets us get around this and point to a specific instance
        // that they handed us.
        callbacks_.push_back(std::ref(callback));
    }

    template <typename U>
    void event_filter::add_on_event_callback(const U &callback)
    {
        // This is where temporaries bind to. Temporaries can't be wrapped in
        // a std::ref because they'll go away very quickly. We are forced to
        // actually copy these.
        callbacks_.push_back(callback);
    }

    inline void event_filter::on_event(const EVENT_RECORD &record) const
    {
        if (callbacks_.empty()) {
            return;
        }

        if (!predicate_(record)) {
            return;
        }

        for (auto &callback : callbacks_) {
            callback(record);
        }
    }
} /* namespace krabs */
