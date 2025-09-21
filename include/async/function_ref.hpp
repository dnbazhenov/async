/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <async/impl/function_traits.hpp>

#include <utility>

namespace async
{

struct function_ref_base
{};

template <typename Function>
struct function_ref;

#define CONSTEXPR constexpr

template <typename Return, typename... Args>
struct function_ref<Return(Args...)> : public function_ref_base
{
    using return_type = Return;
    using signature = Return(Args...);

    CONSTEXPR function_ref()
    {}
    CONSTEXPR function_ref(const function_ref&) = default;
    CONSTEXPR function_ref(function_ref&& old) : _ref(old._ref)
    {
        old._ref = {};
    }

    template <impl::is_callable Function>
        requires(!std::derived_from<Function, function_ref_base>)
    CONSTEXPR explicit function_ref(const Function* func) :
        _ref(store(std::forward<Function>(*func)))
    {}
    template <impl::is_callable Function>
        requires(!std::derived_from<Function, function_ref_base>)
    CONSTEXPR explicit function_ref(Function* func) : _ref(store(std::forward<Function>(*func)))
    {}
    CONSTEXPR operator bool() const
    {
        return _ref.valid();
    }
    CONSTEXPR function_ref& operator=(const function_ref&) = default;
    CONSTEXPR function_ref& operator=(function_ref&& old)
    {
        _ref = old._ref;
        old._ref = {};
        return *this;
    }
    CONSTEXPR function_ref& operator=(std::nullptr_t)
    {
        _ref = {};
        return *this;
    }
    template <impl::is_callable Function>
        requires(!std::derived_from<Function, function_ref_base>)
    CONSTEXPR function_ref& operator=(const Function* func)
    {
        _ref = store(std::forward<const Function>(*func));
        return *this;
    }
    template <impl::is_callable Function>
        requires(!std::derived_from<Function, function_ref_base>)
    CONSTEXPR function_ref& operator=(Function* func)
    {
        _ref = store(std::forward<Function>(*func));
        return *this;
    }
    template <typename... Args2>
    Return operator()(Args2&&... args) const
    {
        return _ref.invoke(std::forward<Args2>(args)...);
    }

  private:
    union storage
    {
        const void* const_callable = nullptr;
        void* callable;
        void (*fn_ptr)();
    };

    struct reference
    {
        storage ptr;
        Return (*invoker)(const storage&, Args...) = nullptr;

        CONSTEXPR bool valid() const
        {
            return ptr.callable != nullptr;
        }

        template <typename... Args2>
        Return invoke(Args2&&... args) const
        {
            return invoker(ptr, std::forward<Args2>(args)...);
        }
    };

  private:
    static CONSTEXPR reference store(Return (*fn)(Args...))
    {
        return {static_cast<void*>(fn), [](const storage& ref, Args... args) -> Return {
                    auto fn = reinterpret_cast<Return (*)(Args...)>(ref.function_ptr);
                    return fn(std::forward<Args>...);
                }};
    }

    template <typename Callable>
    static CONSTEXPR reference store(Callable&& callable)
    {
        using const_type = std::remove_reference_t<Callable>;
        using type = std::decay_t<Callable>;

        if CONSTEXPR (std::is_same_v<const_type, type>)
        {
            return {{.callable = std::addressof(callable)},
                    [](const storage& ref, Args... args) -> Return {
                        auto* object = reinterpret_cast<type*>(ref.callable);
                        return object->operator()(std::forward<Args>(args)...);
                    }};
        }
        else
        {
            return {{.const_callable = std::addressof(callable)},
                    [](const storage& ref, Args... args) -> Return {
                        const_type* object = reinterpret_cast<const_type*>(ref.const_callable);
                        return object->operator()(std::forward<Args>(args)...);
                    }};
        }
    }

  private:
    reference _ref;
};

template <typename F>
function_ref(F*) -> function_ref<impl::function_traits_t<F>>;

template <typename F>
function_ref(const F*) -> function_ref<impl::function_traits_t<const F>>;

} // namespace async
