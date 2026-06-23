#pragma once

#if __cplusplus < 202302L
#error out of date c++ version, compile with -stdc++=2c
#elif defined(__clang__) && __clang_major__ < 22
#error out of date clang, compile with latest version
#elif !defined(__clang__) && defined(__GNUC__) && __GNUC__ < 14
#error out of date g++, compile with latest version
#elif defined(_MSC_VER) && _MSC_VER < 19
#error out of date msvc, compile with latest version
#elif !defined(__clang__) && !defined(__GNUC__) && !defined(_MSC_VER)
#error compiler unknown, could not detect gcc, clang, or msvc
#else

#include <concepts>
#include <memory>
#include <ranges>
#include <utility>

namespace irv {
    namespace detail {
        template<typename tp_type_t>
        struct implicitly_convert_to_fn {
            template<std::convertible_to<tp_type_t> tp_object_t>
            auto constexpr operator()(tp_object_t&& p_object)
            const noexcept(
                std::is_nothrow_convertible_v<
                    tp_object_t,
                    tp_type_t
                >
            )
            -> tp_type_t {
                return std::forward<tp_object_t>(p_object);
            }
        };
    }
    template<typename tp_type_t>
    auto constexpr implicitly_convert_to = detail::implicitly_convert_to_fn<tp_type_t>{};

    namespace detail {
        template<
            typename tp_to_t,
            class... tp_from_ts
        >
        concept parenthesized_functional_castable = requires { tp_to_t(std::declval<tp_from_ts>()...); };

        template<
            typename tp_to_t,
            class... tp_from_ts
        >
        concept nothrow_parenthesized_functional_castable = noexcept(tp_to_t(std::declval<tp_from_ts>()...));

        template<typename tp_type_t>
        struct parenthesized_functional_cast_fn {
            template<class... tp_arguments_ts>
            requires(
                parenthesized_functional_castable<
                    tp_type_t,
                    tp_arguments_ts...
                >
            )
            auto constexpr operator()(tp_arguments_ts&&... p_arguments)
            const noexcept(
                nothrow_parenthesized_functional_castable<
                    tp_type_t,
                    tp_arguments_ts...
                >
            )
            -> tp_type_t {
                return tp_type_t(std::forward<tp_arguments_ts>(p_arguments)...);
            }
        };
    }
    template<typename tp_type_t>
    auto constexpr parenthesized_functional_cast = detail::parenthesized_functional_cast_fn<tp_type_t>{};

    namespace detail {
        template<
            typename tp_to_t,
            class... tp_from_ts
        >
        concept braced_functional_castable = requires { tp_to_t(std::declval<tp_from_ts>()...); };

        template<
            typename tp_to_t,
            class... tp_from_ts
        >
        concept nothrow_braced_functional_castable = noexcept(tp_to_t(std::declval<tp_from_ts>()...));

        template<typename tp_type_t>
        struct braced_functional_cast_fn {
            template<class... tp_arguments_ts>
            requires(
                braced_functional_castable<
                    tp_type_t,
                    tp_arguments_ts...
                >
            )
            auto constexpr operator()(tp_arguments_ts&&... p_arguments)
            const noexcept(
                nothrow_braced_functional_castable<
                    tp_type_t,
                    tp_arguments_ts...
                >
            )
            -> tp_type_t {
                return tp_type_t{std::forward<tp_arguments_ts>(p_arguments)...};
            }
        };
    }
    template<typename tp_type_t>
    auto constexpr braced_functional_cast = detail::braced_functional_cast_fn<tp_type_t>{};

    namespace detail {
        template<
            typename tp_from_t,
            typename tp_to_t
        >
        concept static_castable_to = requires { static_cast<tp_to_t>(std::declval<tp_from_t>()); };

        template<
            typename tp_from_t,
            typename tp_to_t
        >
        concept nothrow_static_castable_to = noexcept(static_cast<tp_to_t>(std::declval<tp_from_t>()));

        template<typename tp_type_t>
        struct static_cast_to_fn {
            template<static_castable_to<tp_type_t> tp_object_t>
            auto constexpr operator()(tp_object_t&& p_object)
            const noexcept(
                nothrow_static_castable_to<
                    tp_object_t,
                    tp_type_t
                >
            )
            -> tp_type_t {
                return static_cast<tp_type_t>(std::forward<tp_object_t>(p_object));
            }
        };
    }
    template<typename tp_type_t>
    auto constexpr static_cast_to = detail::static_cast_to_fn<tp_type_t>{};

    namespace detail {
        template<
            typename tp_from_t,
            typename tp_to_t
        >
        concept lifetime_startable_as =
            std::is_implicit_lifetime_v<tp_to_t> &&
            requires { std::start_lifetime_as<tp_to_t>(static_cast<void*>(std::addressof(std::declval<tp_from_t>()))); };
        
        template<
            typename tp_from_t,
            typename tp_to_t
        >
        concept nothrow_lifetime_startable_as =
            std::is_implicit_lifetime_v<tp_to_t> &&
            noexcept(std::start_lifetime_as<tp_to_t>(std::declval<tp_from_t>()));
        
        template<typename tp_type_t>
        struct start_lifetime_as_fn {
            template<lifetime_startable_as<tp_type_t> tp_object_t>
            requires(!std::is_rvalue_reference_v<tp_object_t&&>)
            auto constexpr operator()(tp_object_t&& p_object)
            const noexcept(
                nothrow_lifetime_startable_as<
                    tp_object_t,
                    tp_type_t
                >
            )
            -> auto&& {
                return *std::start_lifetime_as<tp_type_t>(static_cast<void*>(std::addressof(p_object)));
            }
        };
    }
    template<typename tp_type_t>
    auto constexpr start_lifetime_as = detail::start_lifetime_as_fn<tp_type_t>{};

    namespace detail {
        template<
            typename tp_from_t,
            typename tp_to_t
        >
        concept convertible_to_by_member_conversion_operator = requires { std::declval<tp_from_t>().operator tp_to_t(); };
        
        template<
            typename tp_from_t,
            typename tp_to_t
        >
        concept nothrow_convertible_to_by_member_conversion_operator = noexcept(std::declval<tp_from_t>().operator tp_to_t());
        
        template<typename tp_type_t>
        struct convert_by_operator_fn {
            template<convertible_to_by_member_conversion_operator<tp_type_t> tp_object_t>
            auto constexpr operator()(tp_object_t&& p_object)
            const noexcept(
                nothrow_convertible_to_by_member_conversion_operator<
                    tp_object_t,
                    tp_type_t
                >
            )
            -> tp_type_t {
                return std::forward<tp_object_t>(p_object).operator tp_type_t();
            }
        };
    }
    template<typename tp_type_t>
    auto constexpr convert_by_conversion_operator = detail::convert_by_operator_fn<tp_type_t>{};
}

#endif
