#pragma once

#include <cli151/common.hpp>
#include <cli151/detail/handlers.hpp>
#include <cli151/detail/reflect.hpp>

#include <frozen/string.h>
#include <frozen/unordered_map.h>

#include <array>
#include <string_view>
#include <tuple>
#include <utility>

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
	constexpr static auto info = std::get<N>(cli151::meta<T>::value.args_);
	constexpr static auto maybe_arg_name = info.options.arg_name;
	constexpr static auto arg_name =
		maybe_arg_name == default_ ? get_member_name<info.memptr>() : maybe_arg_name;
	constexpr static auto data = arg_name.data();

  public:
	// The kebabbed name of the nth field of T
	// If the field name was manually specified, do not autokebab
	constexpr static auto name =
		maybe_arg_name == default_ ? detail::kebab<arg_name.size(), &data>() : arg_name;
};

template <class T>
using handler_t = auto(*)(T&, int, const char*[], int&) -> expected<void>;

template <class T, class Seq>
struct handler_dispatcher_impl
{};

template <class T, std::size_t... Is>
struct handler_dispatcher_impl<T, std::index_sequence<Is...>>
{
	constexpr static frozen::unordered_map<frozen::string, std::size_t, sizeof...(Is)> index_map{
		std::pair{frozen::string{kebabbed_name<T, Is>::name}, Is}...,
	};

	constexpr static std::array<handler_t<T>, sizeof...(Is)> callback_map{
		parse_value_into_struct<T, std::get<Is>(cli151::meta<T>::value.args_).memptr>...,
	};
};

template <class T>
using handler_dispatcher =
	handler_dispatcher_impl<T, std::make_index_sequence<cli151::meta<T>::value.n_args>>;

} // namespace cli151::detail
