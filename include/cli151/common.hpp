#pragma once

#include <cli151/detail/compat.hpp>

#include <string_view>
#include <type_traits>

namespace cli151
{

template <class T>
struct meta
{};

// Arbitrary string to indicate that a parameter should not change.
// Mainly useful for if you want to change the abbr or arg_name fields
// while keeping the default generated ones for previous fields.
constexpr static std::string_view default_ = "<default>";

// Indicates that the field should be omitted, i.e. no help text,
// no abbreviated name, or no long name.
constexpr static std::string_view none = "";

// temporary
using error = int;

template <class T>
using expected = compat::expected<T, error>;

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
	requires std::is_member_pointer_v<T>
struct arg
{
	T memptr;
	opt options;

	consteval arg(T m) : memptr{m} {}
	consteval arg(T m, opt options_) : memptr{m}, options{options_} {}
};

template <class... Ts>
	requires((sizeof...(Ts) > 0) && (std::is_member_pointer_v<Ts> && ...))
struct args
{
	std::tuple<arg<Ts>...> args_;
	std::size_t n_args = sizeof...(Ts);

	// Each arg should either be a pointer-to-member or a cli::arg
	// All pointer-to-members will be used to construct a cli::arg
	consteval args(auto... params) : args_{arg<Ts>(params)...} {}
};

namespace detail
{

template <class T>
struct to_memptr
{
	using type = T;
};

template <class T>
struct to_memptr<arg<T>>
{
	using type = T;
};

} // namespace detail

template <class... Ts>
args(Ts...) -> args<typename detail::to_memptr<Ts>::type...>;

} // namespace cli151
