#pragma once

#include <cli151/common.hpp>

#include <cassert>
#include <charconv>
#include <concepts>
#include <string_view>

namespace cli151::detail
{

/*
parse_value(out, argc, argv, current_value, current_index) -> expected
out: If successful, the result is placed here. On error, no change.
argc/argv: Passed from the command line
current_value: Based on the argument type and how it was passed on the CLI:
    Positional argument: The argument itself
    Keyword argument (roughly) in the form `--<key><delimiter><value>`: <value>
    Keyword argument (roughly) in the form `--<key> <value>`: <value>
current_index: The index into argv where current_value comes from.
    On success, should be incremented to the next argument to parse
    On failure, should be the index where the error ocurred
    Assumed to be < argc on entering, may be == argc at exit (to be checked by the caller)
*/

inline auto parse_value(std::string_view& out, const int argc, const char* const* argv,
                        std::string_view current_value, int& current_index) -> expected<void>
{
	assert(current_index < argc);
	out = current_value;
	++current_index;
	return {};
}

inline auto parse_value(const char*& out, const int argc, const char* const* argv,
                        std::string_view current_value, int& current_index) -> expected<void>
{
	assert(current_index < argc);
	out = current_value.data();
	++current_index;
	return {};
}

// FIXME I think this one is broken
template <class T>
	requires(std::integral<T> || std::floating_point<T>)
inline auto parse_value(T& out, const int argc, const char* const* argv,
                        std::string_view current_value, int& current_index) -> expected<void>
{
	assert(current_index < argc);

	auto [ptr, ec] =
		std::from_chars(current_value.data(), current_value.data() + current_value.size(), out);

	// TODO: This check is probably wrong
	if (ec == std::errc{})
	{
		return compat::unexpected<error>(4); // arbitrary for now
	}

	return {};
}

// Handlers probably need to be able to handle the first argument being part of the key.
// We don't want to re-split the arg.
// Could just add the current value as a param - it should at least be part of the current arg.
// After that the parsers can use additional args.

template <class T, auto Memptr>
auto parse_value_into_struct(T& out, const int argc, const char* const* argv,
                             std::string_view current_value, int& current_index) -> expected<void>
{
	if (current_index >= argc)
	{
		return compat::unexpected<error>(3); // arbitrary for now
	}

	return parse_value(out.*Memptr, argc, argv, current_value, current_index);
}

} // namespace cli151::detail
