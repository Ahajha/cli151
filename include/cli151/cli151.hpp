#pragma once

#include <expected>
#include <string_view>
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

template <class T>
struct arg
{
	T ptr;

	// Ideally, this is the order people tend to want to use these
	std::string_view help = default_;     // Almost always
	std::string_view abbr = default_;     // Occasionally, to deal with conflicts
	std::string_view arg_name = default_; // Rarely, usually reflected
};

template <class T>
arg(T, ...) -> arg<T>;

// all args should be a cli::arg
template <class... Ts>
struct args : Ts...
{
	// Each arg should either be a pointer-to-member or a cli::arg
	// All pointer-to-members will be used to construct a cli::arg
	consteval args(auto... args_) : Ts{args_}... {}
};

namespace detail
{
template <class T>
struct is_member_pointer : std::false_type
{};

template <class Class, class Member>
struct is_member_pointer<Member Class::*> : std::true_type
{};

} // namespace detail

template <class... Ts>
args(Ts...) -> args<std::conditional_t<detail::is_member_pointer<Ts>::value, arg<Ts>, Ts>...>;

template <class T>
struct meta
{};

// Probably want some sort of error code
template <class T>
auto parse(int argc, char* argv[]) -> std::expected<T, int>
{
	// First construct a map from names to... something. Function pointers, I think.
	// Basically, when we find a name, figure out what to do next.
	// These functions should take the list of args (in), the current index (inout), and the
	// struct(out).
	return T{};
}

} // namespace cli151
