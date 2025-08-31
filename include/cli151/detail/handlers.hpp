#pragma once

#include <cli151/common.hpp>
#include <cli151/detail/compat.hpp>
#include <cli151/detail/concepts.hpp>
#include <cli151/detail/output.hpp>

#include <array>
#include <cassert>
#include <optional>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace cli151::detail
{

template <class Stream>
auto get_next_value(const int argc, const char* const* argv,
                    std::optional<std::string_view> current_value, int& current_index,
                    Stream errstream) -> std::optional<std::string_view>
{
	if (current_value.has_value())
	{
		return current_value.value();
	}
	else if (current_index >= argc)
	{
		output(errstream, "Missing expected value");
		return {};
	}
	else
	{
		return argv[current_index++];
	}
}

/*
parse_value(out, argc, argv, current_value, current_index) -> bool
out: If successful, the result is placed here. On error, no change.
argc/argv: Passed from the command line
current_value: Based on the argument type and how it was passed on the CLI:
    If it is a keyword argument with the key and value in the same arg (--key=value), then value.
    Otherwise nothing,
current_index: The next index to read from argv, if needed.
    On success, should be incremented to the next argument to parse.
*/

template <class Stream>
auto parse_value(std::string_view& out, const int argc, const char* const* argv,
                 std::optional<std::string_view> current_value, int& current_index,
                 Stream errstream) -> bool
{
	auto result = get_next_value(argc, argv, current_value, current_index, errstream);
	if (result)
	{
		out = *result;
	}
	return result.has_value();
}

template <class Stream>
auto parse_value(const char*& out, const int argc, const char* const* argv,
                 std::optional<std::string_view> current_value, int& current_index,
                 Stream errstream) -> bool
{
	auto result = get_next_value(argc, argv, current_value, current_index, errstream);
	if (result)
	{
		out = result->data();
	}
	return result.has_value();
}

template <class T, class Stream>
	requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
auto parse_value(T& out, const int argc, const char* const* argv,
                 std::optional<std::string_view> current_value, int& current_index,
                 Stream errstream) -> bool
{
	auto result = get_next_value(argc, argv, current_value, current_index, errstream);
	if (!result)
	{
		return false;
	}

	const auto [ptr, ec] = compat::from_chars(result->data(), result->data() + result->size(), out);

	if (ec == std::errc())
	{
		return true;
	}

	// By the contract of std::from_chars
	assert(ec == std::errc::result_out_of_range || ec == std::errc::invalid_argument);

	if (ec == std::errc::result_out_of_range)
	{
		output(errstream, "Value ({}) out of range", *result);
	}
	else
	{
		output(errstream, "Not a number ({})", *result);
	}
	return false;
}

// pair / tuple / array
template <class T, class Stream, std::size_t... Is>
auto parse_tuple_like_impl(T& out, const int argc, const char* const* argv,
                           std::optional<std::string_view> current_value, int& current_index,
                           std::index_sequence<Is...>, Stream errstream) -> bool
{
	constexpr auto n_elements = sizeof...(Is);
	static_assert(n_elements > 0, "Requires non-empty pair/tuple");

	const auto parser = [&]<std::size_t I>() -> bool
	{
		if constexpr (I == 0)
		{
			return parse_value(std::get<I>(out), argc, argv, current_value, current_index,
			                   errstream);
		}
		else
		{
			return parse_value(std::get<I>(out), argc, argv, {}, current_index, errstream);
		}
	};

	return (parser.template operator()<Is>() && ...);
}

template <class Stream, class... Ts>
auto parse_value(std::tuple<Ts...>& out, const int argc, const char* const* argv,
                 std::optional<std::string_view> current_value, int& current_index,
                 Stream errstream) -> bool
{
	return parse_tuple_like_impl(out, argc, argv, current_value, current_index,
	                             std::make_index_sequence<sizeof...(Ts)>(), errstream);
}

template <class T, class U, class Stream>
auto parse_value(std::pair<T, U>& out, const int argc, const char* const* argv,
                 std::optional<std::string_view> current_value, int& current_index,
                 Stream errstream) -> bool
{
	return parse_tuple_like_impl(out, argc, argv, current_value, current_index,
	                             std::make_index_sequence<2>(), errstream);
}

template <class T, std::size_t N, class Stream>
auto parse_value(std::array<T, N>& out, const int argc, const char* const* argv,
                 std::optional<std::string_view> current_value, int& current_index,
                 Stream errstream) -> bool
{
	static_assert(N > 0, "Requires non-empty array");

	if (!parse_value(out[0], argc, argv, current_value, current_index, errstream))
	{
		return false;
	}

	for (std::size_t i = 1; i < N; ++i)
	{
		if (!parse_value(out[i], argc, argv, {}, current_index, errstream))
		{
			return false;
		}
	}

	return true;
}

template <class T, class Stream>
auto parse_value(std::optional<T>& out, const int argc, const char* const* argv,
                 std::optional<std::string_view> current_value, int& current_index,
                 Stream errstream) -> bool
{
	T result{};
	const auto parse_result =
		parse_value(result, argc, argv, current_value, current_index, errstream);
	if (parse_result)
	{
		out.emplace(std::move(result));
	}

	return parse_result;
}

template <set_like T, class Stream>
auto parse_value(T& out, const int argc, const char* const* argv,
                 std::optional<std::string_view> current_value, int& current_index,
                 Stream errstream) -> bool
{
	typename T::value_type to_insert;
	const auto parse_result =
		parse_value(to_insert, argc, argv, current_value, current_index, errstream);
	if (parse_result)
	{
		out.insert(std::move(to_insert));
	}

	return parse_result;
}

template <class Stream>
auto parse_value(bool& out, [[maybe_unused]] const int argc,
                 [[maybe_unused]] const char* const* argv,
                 [[maybe_unused]] std::optional<std::string_view> current_value,
                 [[maybe_unused]] int& current_index, [[maybe_unused]] Stream errstream) -> bool
{
	out = !out;
	return true;
}

template <class T, std::size_t N>
consteval auto is_single_use_arg() -> bool
{
	using M =
		typename pointer_to_member<decltype(std::get<N>(meta<T>::value.args_).memptr)>::member;
	return !set_like<M>;
}

template <class T, std::size_t I, class Stream>
auto parse_value_into_struct(T& out, const int argc, const char* const* argv,
                             std::optional<std::string_view> current_value, int& current_index,
                             bool& used, Stream errstream) -> bool
{
	constexpr auto memptr = std::get<I>(meta<T>::value.args_).memptr;

	if constexpr (is_single_use_arg<T, I>())
	{
		if (used)
		{
			output(errstream, "Duplicate keyword");
			return false;
		}
		used = true;
	}

	return parse_value(out.*memptr, argc, argv, current_value, current_index, errstream);
}

} // namespace cli151::detail
