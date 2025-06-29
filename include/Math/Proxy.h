#pragma once

#include <concepts>

template<class T>
struct UnaliasedRef
{
	T& r;

	constexpr UnaliasedRef(T& r) [[gnu::always_inline]] : r(r) {}

	[[gnu::always_inline]]
	T& operator=(auto&& proxy)
	{
		proxy.template Eval<false>(r);

		return r;
	}
};

template<class T>
concept HasProxy = []() consteval
{
	using F = decltype([]<bool>(auto&){});

	if constexpr (requires { typename T::Proxy<F>; })
		return std::constructible_from<T, typename T::Proxy<F>>;
	else
		return false;
}();

template<HasProxy T> [[gnu::always_inline, nodiscard]]
constexpr UnaliasedRef<T> AssureUnaliased(T& t) { return t; }

template<class T> [[gnu::always_inline, nodiscard]]
constexpr T& AssureUnaliased(T& t) { return t; }

template<bool mayAlias, HasProxy T>
[[gnu::always_inline, nodiscard]]
std::conditional_t<mayAlias, T&, UnaliasedRef<T>>
constexpr ToggleAliased(T& t) { return t; }
