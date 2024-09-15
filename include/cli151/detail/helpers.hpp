#pragma once

#include <frozen/string.h>

#include <algorithm>
#include <array>

namespace cli151::detail
{

template <std::size_t N, std::size_t... Is>
constexpr auto to_array(const char (&arr)[N], std::index_sequence<Is...>) -> std::array<char, N - 1>
{
	return {{arr[Is]...}};
}

template <std::size_t N>
constexpr auto to_array(const char (&arr)[N]) -> std::array<char, N - 1>
{
	return to_array(arr, std::make_index_sequence<N - 1>());
}

template <std::size_t N>
consteval auto kebab(const char (&arr)[N]) -> frozen::string
{
	auto arr2 = to_array(arr);

	std::ranges::replace(arr2, '_', '-');

	return arr2;
}

} // namespace cli151::detail
