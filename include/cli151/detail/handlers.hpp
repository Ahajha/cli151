#pragma once

#include <cli151/common.hpp>

#include <cassert>
#include <charconv>
#include <concepts>
#include <string_view>

namespace cli151::detail
{

inline auto parse_value(std::string_view& out, const int argc, const char* argv[],
                        int& current_index) -> expected<void>
{
	assert(current_index < argc);
	out = argv[current_index++];
	return {};
}

inline auto parse_value(const char*& out, const int argc, const char* argv[], int& current_index)
	-> expected<void>
{
	assert(current_index < argc);
	out = argv[current_index++];
	return {};
}

template <class T>
	requires(std::integral<T> || std::floating_point<T>)
inline auto parse_value(T& out, const int argc, const char* argv[], int& current_index)
	-> expected<void>
{
	assert(current_index < argc);

	const std::string_view view{argv[current_index]};
	auto [ptr, ec] = std::from_chars(view.data(), view.data() + view.size(), out);

	// TODO: This check is probably wrong
	if (ec == std::errc{})
	{
		return compat::unexpected<error>(4); // arbitrary for now
	}

	return {};
}

template <class T, auto Memptr>
auto parse_value_into_struct(T& out, const int argc, const char* argv[], int& current_index)
	-> expected<void>
{
	if (current_index >= argc)
	{
		return compat::unexpected<error>(3); // arbitrary for now
	}

	return parse_value(out.*Memptr, argc, argv, current_index);
}

} // namespace cli151::detail
