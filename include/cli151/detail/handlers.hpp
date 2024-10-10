#pragma once

#include <cli151/common.hpp>
#include <cli151/detail/compat.hpp>
#include <cli151/detail/concepts.hpp>

#include <cassert>
#include <string_view>
#include <type_traits>

namespace cli151::detail
{

inline auto get_next_value(const int argc, const char* const* argv,
                           std::optional<std::string_view> current_value, int& current_index)
	-> expected<std::string_view>
{
	if (current_value.has_value())
	{
		return current_value.value();
	}
	else if (current_index >= argc)
	{
		return compat::unexpected(error{
			.type = error_type::missing_args,
			.arg_index = current_index,
		});
	}
	else
	{
		return argv[current_index++];
	}
}

/*
parse_value(out, argc, argv, current_value, current_index) -> expected
out: If successful, the result is placed here. On error, no change.
argc/argv: Passed from the command line
current_value: Based on the argument type and how it was passed on the CLI:
    If it is a keyword argument with the key and value in the same arg (--key=value), then value.
    Otherwise nothing,
current_index: The next index to read from argv, if needed.
    On success, should be incremented to the next argument to parse.
*/

inline auto parse_value(std::string_view& out, const int argc, const char* const* argv,
                        std::optional<std::string_view> current_value, int& current_index)
	-> expected<void>
{
	return get_next_value(argc, argv, current_value, current_index)
	    .transform([&out](std::string_view result) { out = result; });
}

inline auto parse_value(const char*& out, const int argc, const char* const* argv,
                        std::optional<std::string_view> current_value, int& current_index)
	-> expected<void>
{
	return get_next_value(argc, argv, current_value, current_index)
	    .transform([&out](std::string_view result) { out = result.data(); });
}

template <class T>
	requires(std::is_integral_v<T> || std::is_floating_point_v<T>)
inline auto parse_value(T& out, const int argc, const char* const* argv,
                        std::optional<std::string_view> current_value, int& current_index)
	-> expected<void>
{
	return get_next_value(argc, argv, current_value, current_index)
	    .and_then(
			[&out, &current_index](std::string_view result) -> expected<void>
			{
				const auto [ptr, ec] =
					compat::from_chars(result.data(), result.data() + result.size(), out);

				if (ec != std::errc{})
				{
					return compat::unexpected(error{
						.type = error_type::invalid_number,
						.arg_index = current_index,
					});
				}

				return {};
			});
}

template <class T>
inline auto parse_value(std::optional<T>& out, const int argc, const char* const* argv,
                        std::optional<std::string_view> current_value, int& current_index)
	-> expected<void>
{
	T result{};
	const auto parse_result = parse_value(result, argc, argv, current_value, current_index);
	if (parse_result)
	{
		out.emplace(std::move(result));
	}

	return parse_result;
}

inline auto parse_value(bool& out, [[maybe_unused]] const int argc,
                        [[maybe_unused]] const char* const* argv,
                        [[maybe_unused]] std::optional<std::string_view> current_value,
                        [[maybe_unused]] int& current_index) -> expected<void>
{
	out = !out;
	return {};
}

template <class T, auto Memptr>
auto parse_value_into_struct(T& out, const int argc, const char* const* argv,
                             std::optional<std::string_view> current_value, int& current_index)
	-> expected<void>
{
	return parse_value(out.*Memptr, argc, argv, current_value, current_index);
}

} // namespace cli151::detail
