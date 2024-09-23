#pragma once

#include <cli151/common.hpp>
#include <cli151/detail/helpers.hpp>

#include <expected>

namespace cli151
{

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
