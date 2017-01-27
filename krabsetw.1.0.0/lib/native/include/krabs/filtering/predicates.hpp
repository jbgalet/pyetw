// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include <evntcons.h>
#include <functional>
#include <algorithm>
#include <string>
#include <boost/algorithm/string.hpp>

#include "../compiler_check.hpp"
#include "comparers.hpp"
#include "view_adapters.hpp"

using namespace krabs::predicates::adapters;
using namespace krabs::predicates::comparers;

namespace krabs { namespace predicates {

    namespace details {

        /**
         * <summary>
         *   Returns true for any event.
         * </summary>
         */
        struct any_event {
            bool operator()(const EVENT_RECORD &) const
            {
                return true;
            }
        };

        /**
         * <summary>
         *   Returns false for any event.
         * </summary>
         */
        struct no_event {
            bool operator()(const EVENT_RECORD &) const
            {
                return false;
            }
        };

        /**
         * <summary>
         *   Performs a logical AND on two filters.
         * </summary>
         */
        template <typename T1, typename T2>
        struct and_filter {
            and_filter(const T1 &t1, const T2 &t2)
                : t1_(t1)
                , t2_(t2)
            {}

            bool operator()(const EVENT_RECORD &record) const
            {
                return (t1_(record) && t2_(record));
            }

        private:
            const T1 t1_;
            const T2 t2_;
        };

        /**
         * <summary>
         *   Performs a logical OR on two filters.
         * </summary>
         */
        template <typename T1, typename T2>
        struct or_filter {
            or_filter(const T1 &t1, const T2 &t2)
                : t1_(t1)
                , t2_(t2)
            {}

            bool operator()(const EVENT_RECORD &record) const
            {
                return (t1_(record) || t2_(record));
            }

        private:
            const T1 t1_;
            const T2 t2_;
        };

        /**
         * <summary>
         *   Performs a logical NOT on a filter.
         * </summary>
         */
        template <typename T1>
        struct not_filter {
            not_filter(const T1 &t1)
                : t1_(t1)
            {}

            bool operator()(const EVENT_RECORD &record) const
            {
                return !t1_(record);
            }

        private:
            const T1 t1_;
        };

        /**
        * <summary>
        *   Returns true if the event property matches the expected value.
        * </summary>
        */
        template <typename T>
        struct property_is {
            property_is(const std::wstring &property, const T &expected)
                : property_(property)
                , expected_(expected)
            {}

            bool operator()(const EVENT_RECORD &record) const
            {
                krabs::schema schema(record);
                krabs::parser parser(schema);

                try {
                    return (expected_ == parser.parse<T>(property_));
                }
                catch (...) {
                    return false;
                }
            }

        private:
            const std::wstring property_;
            const T expected_;
        };

        /**
         * <summary>
         *   Gets a collection_view of a property using the specified adapter
         *   and executes the specified predicate against the view.
         *   This is used to provide type-specialization for properties
         *   that can be represented by the collection_view.
         * </summary>
         */
        template <typename T, typename Adapter, typename Predicate>
        struct property_view_predicate
        {
            property_view_predicate(
                const std::wstring &property,
                const T &expected,
                Adapter adapter,
                Predicate predicate)
                : property_(property)
                , expected_(expected)
                , adapter_(adapter)
                , predicate_(predicate)
            { }

            bool operator()(const EVENT_RECORD &record)
            {
                krabs::schema schema(record);
                krabs::parser parser(schema);

                try {
                    auto view = parser.view_of(property_, adapter_);
                    return predicate_(view.begin(), view.end(), expected_.begin(), expected_.end());
                }
                catch (...) {
                    return false;
                }
            }

        private:
            const std::wstring property_;
            const T expected_;
            Adapter adapter_;
            Predicate predicate_;
        };
    } /* namespace details */

    // Filter factory functions
    // ------------------------------------------------------------------------

    /**
     * <summary>
     *   A simple filter that accepts any event.
     * </summary>
     */
    static details::any_event any_event;

    /**
     * <summary>
     *   A simple filter that accepts no events.
     * </summary>
     */
    static details::no_event no_event;

    /**
     * <summary>
     *   Accepts an event if its ID matches the expected value.
     * </summary>
     */
    struct id_is {
        id_is(size_t expected)
        : expected_(USHORT(expected))
        {}

        bool operator()(const EVENT_RECORD &record)
        {
            return (record.EventHeader.EventDescriptor.Id == expected_);
        }

    private:
        USHORT expected_;
    };

    /**
     * <summary>
     *   Accepts an event if its opcode matches the expected value.
     * </summary>
     */
    struct opcode_is {
        opcode_is(size_t expected)
        : expected_(USHORT(expected))
        {}

        bool operator()(const EVENT_RECORD &record)
        {
            return (record.EventHeader.EventDescriptor.Opcode == expected_);
        }

    private:
        USHORT expected_;
    };

    /**
     * <summary>
     *   Accepts an event if its version matches the expected value.
     * </summary>
     */
    struct version_is {
        version_is(size_t expected)
        : expected_(USHORT(expected))
        {}

        bool operator()(const EVENT_RECORD &record)
        {
            return (record.EventHeader.EventDescriptor.Version == expected_);
        }

    private:
        USHORT expected_;
    };

    /**
     * <summary>
     *   Accepts an event if the named property matches the expected value.
     * </summary>
     */
    template <typename T>
    details::property_is<T> property_is(
        const std::wstring &prop,
        const T &expected)
    {
        return details::property_is<T>(prop, expected);
    }

     /**
      * <summary>
      *   Explicit specialization for string arrays, because C++.
      * </summary>
      */
    inline details::property_is<std::wstring> property_is(
        const std::wstring &prop,
        const wchar_t *expected)
    {
        return details::property_is<std::wstring>(prop, std::wstring(expected));
    }

    inline details::property_is<std::string> property_is(
        const std::wstring &prop,
        const char *expected)
    {
        return details::property_is<std::string>(prop, std::string(expected));
    }

    // View-based filters
    // ------------------------------------------------------------------------

    /**
     * View based filters work on ranges of data in-place in the etw record.
     * By default, these expect a null terminated string property in the
     * record, but if an adapter is specified, any range of data can be
     * compared. The comparison functor must support taking two iterator
     * pairs that iterate the value_type specified by the adapter. Because
     * comparisons use iterators, it's possible to compare various flavors
     * of strings as well as arrays and binary data.
     */

    /**
     * Accepts events if property exactly matches the expected value
     */
    template <
        typename Adapter = adapters::null_string<wchar_t>,
        typename T,
        typename Comparer = equals<std::equal_to<typename Adapter::value_type>>>
    details::property_view_predicate<T, Adapter, Comparer> property_equals(
        const std::wstring &prop,
        const T& expected)
    {
        return{ prop, expected, Adapter(), Comparer() };
    }

    /**
     * Accepts events if property case insensitive matches the expected value
     */
    template <
        typename Adapter = adapters::null_string<wchar_t>,
        typename T,
        typename Comparer = equals<iequal_to<typename Adapter::value_type>>>
    details::property_view_predicate<T, Adapter, Comparer> property_iequals(
        const std::wstring &prop,
        const T& expected)
    {
        return{ prop, expected, Adapter(), Comparer() };
    }

    /**
     * Accepts events if property contains expected value
     */
    template <
        typename Adapter = adapters::null_string<wchar_t>,
        typename T,
        typename Comparer = contains<std::equal_to<typename Adapter::value_type>>>
    details::property_view_predicate<T, Adapter, Comparer> property_contains(
        const std::wstring &prop,
        const T& expected)
    {
        return {prop, expected, Adapter(), Comparer()};
    }

    /**
     * Accepts events if property case insensitive contains expected value
     */
    template <
        typename Adapter = adapters::null_string<wchar_t>,
        typename T,
        typename Comparer = contains<iequal_to<typename Adapter::value_type>>>
    details::property_view_predicate<T, Adapter, Comparer> property_icontains(
        const std::wstring &prop,
        const T& expected)
    {
        return{ prop, expected, Adapter(), Comparer() };
    }

    /**
     * Accepts events if property starts with expected value
     */
    template <
        typename Adapter = adapters::null_string<wchar_t>,
        typename T,
        typename Comparer = starts_with<std::equal_to<typename Adapter::value_type>>>
    details::property_view_predicate<T, Adapter, Comparer> property_starts_with(
        const std::wstring &prop,
        const T& expected)
    {
        return{ prop, expected, Adapter(), Comparer() };
    }

    /**
     * Accepts events if property case insensitive starts with expected value
     */
    template <
        typename Adapter = adapters::null_string<wchar_t>,
        typename T,
        typename Comparer = starts_with<iequal_to<typename Adapter::value_type>>>
    details::property_view_predicate<T, Adapter, Comparer> property_istarts_with(
        const std::wstring &prop,
        const T& expected)
    {
        return{ prop, expected, Adapter(), Comparer() };
    }

    /**
     * <summary>
     *   Accepts an event if its two component filters both accept the event.
     * </summary>
     */
    template <typename T1, typename T2>
    details::and_filter<T1, T2> and_filter(const T1 &t1, const T2 &t2)
    {
        return details::and_filter<T1, T2>(t1, t2);
    }

    /**
     * <summary>
     *   Accepts an event if either of its two component filters accept the event.
     * </summary>
     */
    template <typename T1, typename T2>
    details::or_filter<T1, T2> or_filter(const T1 &t1, const T2 &t2)
    {
        return details::or_filter<T1, T2>(t1, t2);
    }

    /**
     * <summary>
     *   Negates the filter that is given to it.
     * </summary>
     */
    template <typename T1>
    details::not_filter<T1> not_filter(const T1 &t1)
    {
        return details::not_filter<T1>(t1);
    }

} /* namespace predicates */ } /* namespace krabs */
