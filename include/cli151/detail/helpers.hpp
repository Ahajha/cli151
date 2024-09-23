#pragma once

#include <array>
#include <string_view>
#include <utility>
#include <tuple>

namespace cli151
{
template <class T>
struct meta;
}

namespace cli151::detail
{

template <std::size_t N, const char* const* str, std::size_t... Is>
struct kebabber
{
	constexpr static std::array<char, N> data{{((*str)[Is] == '_' ? '-' : (*str)[Is])...}};
};

template <std::size_t N, const char* const* str, std::size_t... Is>
consteval auto to_array_and_kebab(std::index_sequence<Is...>) -> std::string_view
{
	return {kebabber<N, str, Is...>::data.data(), N};
}

// Given a string and a size, produces a new string at compile time
// with all underscores replaced with dashes.
template <std::size_t N, const char* const* str>
consteval auto kebab() -> std::string_view
{
	return to_array_and_kebab<N, str>(std::make_index_sequence<N>());
}

template <class T, std::size_t N>
struct kebabbed_name
{
private:
	constexpr static auto arg_name = std::get<N>(cli151::meta<T>::value.args_).options.arg_name;
	constexpr static auto data = arg_name.data();
public:
	// The kebabbed name of the nth field of T
	constexpr static auto name = detail::kebab<arg_name.size(), &data>();
};

} // namespace cli151::detail
