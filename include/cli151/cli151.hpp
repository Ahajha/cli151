#pragma once

#include <expected>
#include <string_view>
#include <type_traits>

namespace cli151
{

template <class T>
struct arg
{
	T ptr_to_member;
	// If empty, no value. Mainly applicable for short names.
	std::string_view field_name;
	std::string_view short_name;

	// Should have a few constructors, all constexpr/consteval
	// This constructor will only be available when reflection is available
	// Will need to check which compilers support this
	consteval arg(T t) : ptr_to_member{t}
	{
		// reflect the other fields
	}

	// This should give a compile failure if fn is length 0. Todo: verify
	// If given a string literal, short_name ends up being a nul character
	// Should find a way to fix that I guess.
	consteval arg(T t, std::string_view fn)
		: ptr_to_member{t}, field_name{fn}, short_name{fn.data(), 1}
	{}
	consteval arg(T t, std::string_view fn, std::string_view sn)
		: ptr_to_member{t}, field_name{fn}, short_name{sn}
	{}
};

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
