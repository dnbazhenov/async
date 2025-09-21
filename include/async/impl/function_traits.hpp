/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <tuple>
#include <type_traits>

namespace async::impl
{

template <typename F, typename = void>
struct function_traits
{
    static constexpr auto is_function_ptr = false;
    static constexpr auto is_callable = false;
    static constexpr auto is_const_callable = false;
};

// Specialization for function types
template <typename R, typename... A>
struct function_traits<R(A...)>
{
    using return_type = R;
    using args_tuple = std::tuple<A...>;
    using type = R(A...);
    static constexpr auto is_function_ptr = true;
    static constexpr auto is_callable = false;
    static constexpr auto is_const_callable = false;
};

// Specialization for function pointer types
template <typename R, typename... A>
struct function_traits<R (*)(A...)> : function_traits<R(A...)>
{};

// Specialization for member function pointers
template <typename C, typename R, typename... A>
struct function_traits<R (C::*)(A...)> : function_traits<R(A...)>
{
    static constexpr auto is_function_ptr = false;
    static constexpr auto is_callable = true;
    static constexpr auto is_const_callable = false;
};

template <typename C, typename R, typename... A>
struct function_traits<R (C::*)(A...) const> : function_traits<R(A...)>
{
    static constexpr auto is_function_ptr = false;
    static constexpr auto is_callable = true;
    static constexpr auto is_const_callable = false;
};

// Specialization for callable objects (lambdas, functors)
template <typename F>
struct function_traits<F, std::void_t<decltype(&F::operator())>> :
    function_traits<decltype(&F::operator())>
{};

template <typename Function>
using function_traits_t = typename function_traits<Function>::type;

template <typename T>
concept is_function_ptr = function_traits<std::remove_reference_t<T>>::is_function_ptr;

template <typename T>
concept is_callable = function_traits<std::remove_reference_t<T>>::is_callable;

template <typename T>
concept is_const_callable = function_traits<std::remove_reference_t<T>>::is_callable;

} // namespace async::impl
