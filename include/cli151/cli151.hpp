#pragma once

#include <cli151/detail/concepts.hpp>
#include <cli151/error.hpp>

#include <expected>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace cli151
{

// Arbitrary string to indicate that a parameter should not change.
// Mainly useful for if you want to change the abbr or arg_name fields
// while keeping the default generated ones for previous fields.
constexpr static std::string_view default_ = "<default>";

// Indicates that the field should be omitted, i.e. no help text,
// no abbreviated name, or no long name.
constexpr static std::string_view none = "";

enum class arg_type
{
	// The argument is determined by its position in the arguments, and is required
	positional_required,
	// The argument is determined by its position in the arguments, and is optional
	positional_optional,
	// The argument is determined by keyword (short or long), and is optional
	keyword,
	// Have the library guess:
	// All booleans are keyword arguments
	// std::optional is guessed to be keyword
	unspecified,

	// Unbounded containers
	// std::optionals cannot be positional_required
};

struct opt
{
	// Ideally, this is the order people tend to want to use these
	std::string_view help = default_;     // Almost always
	std::string_view abbr = default_;     // Occasionally, to deal with conflicts
	std::string_view arg_name = default_; // Rarely, usually reflected
};

template <class T>
struct arg
{
	T memptr;
	opt options;

	consteval arg(T m) : memptr{m} {}
	consteval arg(T m, opt options_) : memptr{m}, options{options_} {}
};

// all args should be a pointer-to-member
template <class... Ts>
struct args
{
	std::tuple<arg<Ts>...> args_;

	// Each arg should either be a pointer-to-member or a cli::arg
	// All pointer-to-members will be used to construct a cli::arg
	template <class... Us>
	consteval args(Us... params) : args_{arg<Ts>{params}...}
	{}
};

template <class... Ts>
args(Ts...) -> args<std::conditional_t<std::is_member_pointer_v<Ts>, arg<Ts>, Ts>...>;

template <class T>
struct meta
{};

// Probably want some sort of error code
template <class T>
auto parse(int argc, char* argv[]) -> expected<T>
{
	T result;

	// Map 1: Convert short names to long names
	// Map 2: Convert long names to {memptr, used}

	// First construct a map from names to... something. Function pointers, I think.
	// Basically, when we find a name, figure out what to do next.
	// These functions should take the list of args (in), the current index (inout), and the
	// struct(out).
	return result;
}

} // namespace cli151
