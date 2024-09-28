#pragma once

#include <cli151/common.hpp>
#include <cli151/detail/compat.hpp>
#include <cli151/detail/helpers.hpp>

namespace cli151
{

// Probably want some sort of error code
template <class T>
auto parse(int argc, const char* const* argv) -> expected<T>
{
	// T is probably an aggregate. We 0-initialize (or whichever type of initialization C++ calls
	// this) the result to prevent random values from appearing. Not sure if this should be part of
	// the public API, but could prevent some cryptic bugs.
	T result{};

	using dispatcher = detail::handler_dispatcher<T>;

	std::array<bool, dispatcher::index_to_handler_map.size()> used{};

	// High level overview:
	// - Figure out if this is positional or keyword
	// - If keyword, parse out the key and look up the callback
	// - If positional, go by the next positional arg to use (can use the indexes... I think a scan
	//   for the next positional is okay for now. Maybe can optimize later. Not sure if we can do
	//   the scan at runtime...)
	//   Nope - we'll need to make an array of indexes of positional args.
	// - Call the callback (which should advance the position to the next arg to look at)

	// Skip over argv[0]
	int arg_index = 1;
	while (arg_index < argc)
	{
		const std::string_view view = argv[arg_index];
		if (view.at(0) == '-')
		{
			// Either a long or short form
			// TODO: For now this allows any number of dashes before, this doesn't hurt too much but
			// is something we will want to revisit.
			const auto nodashes = view.substr(view.find_first_not_of('-'));

			// In case this is a key + value, parse out delimiters
			const auto delimiter_pos = nodashes.find_first_of(":=");
			const auto key = nodashes.substr(0, delimiter_pos);

			const auto handler_index = dispatcher::name_to_index_map.find(key);

			if (handler_index == dispatcher::name_to_index_map.end())
			{
				// error, arbitrary for now
				return compat::unexpected(error{
					.type = error_type::invalid_key,
					.arg_index = arg_index,
				});
			}

			const auto handler = dispatcher::index_to_handler_map[handler_index->second];

			const auto value = delimiter_pos == std::string_view::npos
			                       ? argv[++arg_index]
			                       : nodashes.substr(delimiter_pos + 1);

			const auto handler_result = handler(result, argc, argv, value, arg_index);

			if (!handler_result)
			{
				return compat::unexpected(handler_result.error());
			}
		}
		else
		{
			// Positional
		}
	}

	return result;
}

} // namespace cli151
