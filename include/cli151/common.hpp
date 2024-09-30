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

enum class error_type
{
	invalid_key,
	invalid_number, // TODO split this one
	no_more_positional_args,
	missing_args,  // TODO this might need more data depending on how many missing args there are
	duplicate_arg, // TODO Maybe a pair of indexes to show the duplication
};

struct error
{
	error_type type;
	int arg_index;
};

template <class T>
using expected = compat::expected<T, error>;

enum class arg_type
{
	// The argument is determined by its position in the arguments, and is required
	positional_required,
	// The argument is determined by its position in the arguments, and is optional.
	// positional_optional, // Unsupported for now.
	// The argument is determined by keyword (short or long), and is optional
	keyword,
	// Have the library guess. With no other information, the first argument passed in that is
	// either a std::optional or a bool is a keyword argument. Any arguments before that are
	// positional, and any after are also keywords.
	// TODO: For the first draft, just try to get this guessing working. Guesses mixed with
	// explicit arg types might be tricky.
	guess,

	// Unbounded containers are sort of special, we'll have to consider them separately.
	// std::optionals cannot be positional_required
	// bools are always keyword
};

struct opt
{
	std::string_view help = default_;
	std::string_view abbr = default_;
	std::string_view arg_name = default_;
	arg_type type = arg_type::guess;
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
	constexpr static std::size_t n_args = sizeof...(Ts);

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
