#pragma once

#include <cli151/common.hpp>
#include <cli151/detail/concepts.hpp>
#include <cli151/detail/handlers.hpp>
#include <cli151/detail/reflect.hpp>

#include <frozen/string.h>
#include <frozen/unordered_map.h>

#include <algorithm>
#include <array>
#include <string_view>
#include <tuple> // IWYU pragma: keep (std::get<std::tuple>)
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

// Provides the kebabbed name of the nth field of T, as well as its abbreviation.
template <class T, std::size_t N>
struct kebabbed_name
{
  private:
	constexpr static auto info = std::get<N>(meta<T>::value.args_);
	constexpr static auto arg_name =
		info.options.arg_name == default_ ? get_member_name<info.memptr>() : info.options.arg_name;
	constexpr static auto arg_name_data = arg_name.data();

  public:
	// Reflect the name, unless explicitly given
	constexpr static auto name = info.options.arg_name == default_
	                                 ? detail::kebab<arg_name.size(), &arg_name_data>()
	                                 : info.options.arg_name;
	// Use the first letter of the full name, unless explicitly given
	constexpr static auto abbr =
		info.options.abbr == default_ ? name.substr(0, 1) : info.options.abbr;
};

// Computes the arg_type of the Nth field of T - evaluating guesses when necessary.
template <class T, std::size_t N>
consteval auto type_of_arg() -> arg_type
{
	// For the first draft, ignore user input. Just guess. (TODO)

	using type =
		typename pointer_to_member<decltype(std::get<N>(meta<T>::value.args_).memptr)>::member;

	// If this is an optional, bool, or a set, it's a keyword.
	if constexpr (is_optional<type>::value || std::is_same_v<bool, type> || set_like<type>)
	{
		return arg_type::keyword;
	}
	// Otherwise, if this is the first argument, it's positional.
	else if constexpr (N == 0)
	{
		return arg_type::positional_required;
	}
	// Otherwise, it's whatever the previous argument is.
	else
	{
		return type_of_arg<T, N - 1>();
	}
}

struct help_data
{
	std::string_view name;
	std::string_view abbr;
	std::string_view help;
	arg_type type;
};

template <class T, std::size_t I>
consteval auto make_help_data() -> help_data
{
	constexpr opt data = std::get<I>(meta<T>::value.args_).options;

	return {
		.name = kebabbed_name<T, I>::name,
		.abbr = kebabbed_name<T, I>::abbr,
		.help = data.help,
		.type = type_of_arg<T, I>(),
	};
}

template <class T, class Seq>
struct help_data_of_impl
{};

template <class T, std::size_t... Is>
struct help_data_of_impl<T, std::index_sequence<Is...>>
{
	constexpr static std::array data{
		make_help_data<T, Is>()...,
	};
};

template <class T>
using help_data_of = help_data_of_impl<T, std::make_index_sequence<meta<T>::value.n_args>>;

// Workaround since frozen::string isn't default constructible.
// The data in the array is arbitrary and will be overwritten.
template <std::size_t... Is>
consteval auto default_name_to_index_map_data(std::index_sequence<Is...>)
{
	return std::array<std::pair<frozen::string, std::size_t>, sizeof...(Is)>{
		std::pair{frozen::string{""}, Is}...,
	};
}

template <class T>
consteval auto make_long_name_to_index_map_data()
{
	constexpr auto should_include = [](const help_data& info)
	{ return !info.name.empty() && info.type == arg_type::keyword; };

	constexpr auto help_data = help_data_of<T>::data;
	constexpr auto size = std::count_if(help_data.begin(), help_data.end(), should_include);

	// Temp hack: Seems to be some issues with 0-length data.
	if constexpr (size == 0)
	{
		return std::array<std::pair<frozen::string, std::size_t>, 1>{
			std::pair{frozen::string{""}, 0},
		};
	}
	else
	{
		auto data = default_name_to_index_map_data(std::make_index_sequence<size>());

		std::size_t index = 0;

		for (std::size_t i = 0; i < help_data.size(); ++i)
		{
			if (should_include(help_data[i]))
			{
				data[index].first = help_data[i].name;
				data[index].second = i;
				++index;
			}
		}

#ifndef NDEBUG
		assert(index == size);
		for ([[maybe_unused]] const auto& [name, i] : data)
		{
			assert(name.size() > 0);
			assert(name != default_);
			assert(i < meta<T>::value.n_args);
		}
#endif

		return data;
	}
}

template <class T>
consteval auto make_short_name_to_index_map_data()
{
	constexpr auto should_include = [](const help_data& info)
	{ return !info.abbr.empty() && info.type == arg_type::keyword; };

	constexpr auto help_data = help_data_of<T>::data;
	constexpr auto size = std::count_if(help_data.begin(), help_data.end(), should_include);

	// Temp hack: Seems to be some issues with 0-length data.
	if constexpr (size == 0)
	{
		return std::array<std::pair<frozen::string, std::size_t>, 1>{
			std::pair{frozen::string{""}, 0},
		};
	}
	else
	{
		auto data = default_name_to_index_map_data(std::make_index_sequence<size>());

		std::size_t index = 0;

		for (std::size_t i = 0; i < help_data.size(); ++i)
		{
			if (should_include(help_data[i]))
			{
				data[index].first = help_data[i].abbr;
				data[index].second = i;
				++index;
			}
		}

#ifndef NDEBUG
		assert(index == size);
		for ([[maybe_unused]] const auto& [name, i] : data)
		{
			assert(name.size() > 0);
			assert(name != default_);
			assert(i < meta<T>::value.n_args);
		}
#endif

		return data;
	}
}

template <class T>
consteval auto make_positional_args_indexes_data()
{
	constexpr auto should_include = [](const help_data& info)
	{ return info.type == arg_type::positional_required; };

	constexpr auto help_data = help_data_of<T>::data;
	constexpr auto size = std::count_if(help_data.begin(), help_data.end(), should_include);

	std::array<std::size_t, size> data{};

	std::size_t index = 0;

	for (std::size_t i = 0; i < help_data.size(); ++i)
	{
		if (should_include(help_data[i]))
		{
			data[index++] = i;
		}
	}

#ifndef NDEBUG
	assert(index == size);
	for ([[maybe_unused]] const auto i : data)
	{
		assert(i < meta<T>::value.n_args);
	}
#endif

	return data;
}

template <class T>
using handler_t = auto(*)(T&, int, const char* const*, std::optional<std::string_view>, int&, bool&)
                      -> expected<void>;

template <class T, class Seq>
struct handler_dispatcher_impl
{};

template <class T, std::size_t... Is>
struct handler_dispatcher_impl<T, std::index_sequence<Is...>>
{
	// This is only a map from the long name. We need the short names too, and we need to omit
	// ones which are not specified. Maybe a helper that computes the data? Counting the number
	// of names is cheap.

	// Maps long and short names of keyword arguments to the index in index_to_handler_map.
	constexpr static auto long_name_to_index_map =
		frozen::make_unordered_map(make_long_name_to_index_map_data<T>());
	constexpr static auto short_name_to_index_map =
		frozen::make_unordered_map(make_short_name_to_index_map_data<T>());

	constexpr static std::array<handler_t<T>, sizeof...(Is)> index_to_handler_map{
		parse_value_into_struct<T, Is>...,
	};

	// Indexes of all the positional arguments in the order they appear.
	// (TODO: In the future when we handle positional_optional, those will all be at the end)
	// Potential future TODO: Ideally, and likely in most circumstances, this array will just be
	// 0, 1, 2, 3, ... In that case, this array serves as an unnecessary indirection. Either we
	// can skip the array conditionally (likely easy), or we can rearrange the args (likely
	// difficult).
	constexpr static auto positional_args_indexes = make_positional_args_indexes_data<T>();
};

template <class T>
using handler_dispatcher =
	handler_dispatcher_impl<T, std::make_index_sequence<meta<T>::value.n_args>>;

template <class T>
auto parse_long_keyword(const std::string_view view, int& arg_index)
	-> expected<std::pair<std::size_t, std::optional<std::string_view>>>
{
	using dispatcher = detail::handler_dispatcher<T>;

	const auto nodashes = view.substr(2);

	// In case this is a key + value, parse out delimiters
	const auto delimiter_pos = nodashes.find_first_of(":= ");
	const auto key = nodashes.substr(0, delimiter_pos);

	const auto handler_index = dispatcher::long_name_to_index_map.find(key);

	if (handler_index == dispatcher::long_name_to_index_map.end())
	{
		return compat::unexpected(error{
			.type = error_type::invalid_key,
			.arg_index = arg_index,
		});
	}

	const auto value = delimiter_pos == std::string_view::npos ? std::optional<std::string_view>{}
	                                                           : nodashes.substr(delimiter_pos + 1);

	++arg_index;

	return std::pair{handler_index->second, value};
}

template <class T>
auto parse_short_keyword(const std::string_view view, int& arg_index)
	-> expected<std::pair<std::size_t, std::optional<std::string_view>>>
{
	using dispatcher = detail::handler_dispatcher<T>;

	const auto nodashes = view.substr(1);

	// In case this is a key + value, parse out delimiters
	const auto delimiter_pos = nodashes.find_first_of(":= ");
	const auto key = nodashes.substr(0, delimiter_pos);

	const auto handler_index = dispatcher::short_name_to_index_map.find(key);

	if (handler_index == dispatcher::short_name_to_index_map.end())
	{
		return compat::unexpected(error{
			.type = error_type::invalid_key,
			.arg_index = arg_index,
		});
	}

	const auto value = delimiter_pos == std::string_view::npos ? std::optional<std::string_view>{}
	                                                           : nodashes.substr(delimiter_pos + 1);

	++arg_index;

	return std::pair{handler_index->second, value};
}

} // namespace cli151::detail
