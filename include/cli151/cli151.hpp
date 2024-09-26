#pragma once

#include <cli151/common.hpp>
#include <cli151/detail/compat.hpp>
#include <cli151/detail/helpers.hpp>

namespace cli151
{

// Probably want some sort of error code
template <class T>
auto parse(int argc, char* argv[]) -> expected<T>
{
	T result;

	using dispatcher = detail::handler_dispatcher<T>;

	const auto& index_map = dispatcher::index_map;
	const auto& callback_map = dispatcher::callback_map;
	std::array<bool, index_map.size()> used;            // filled with false?
	std::fill_n(used.begin(), index_map.size(), false); // is there a constructor for this?

	// High level overview:
	// - Figure out if this is positional or keyword
	// - If keyword, parse out the key and look up the callback
	// - If positional, go by the next positional arg to use (can use the indexes... I think a scan
	//   for the next positional is okay for now. Maybe can optimize later.)
	// - Call the callback (which should advance the position to the next arg to look at)

	return result;
}

} // namespace cli151
