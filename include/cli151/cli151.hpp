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
	// TODO: Make sure args aren't parsed twice

	// High level overview:
	// - Figure out if this is positional or keyword
	// - In either case, get the handler and the value to handle
	// - Call the handler (which should advance the position to the next arg to look at)

	// Eventual TODO: We can probably compile out some of this code when one or more types of args
	// are entirely missing, for example where all args are positional_required.

	// Skip over argv[0]
	int arg_index = 1;
	std::size_t next_positional_arg_to_parse = 0;
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

			// FIXME: If we run out of args, the ++ will cause an out of bounds.
			// May need a pre-check and error. Or maybe error in the handler, since it may be a
			// bool?

			const auto value = delimiter_pos == std::string_view::npos
			                       ? std::optional<std::string_view>{}
			                       : nodashes.substr(delimiter_pos + 1);

			++arg_index;

			// TODO: This code is identical in both branches
			const auto handler = dispatcher::index_to_handler_map[handler_index->second];

			const auto handler_result = handler(result, argc, argv, value, arg_index);

			if (!handler_result)
			{
				return compat::unexpected(handler_result.error());
			}
		}
		else
		{
			// Positional

			if (next_positional_arg_to_parse >= dispatcher::positional_args_indexes.size())
			{
				// TODO: This may behave differently if there is a trailing container in T
				return compat::unexpected(error{
					.type = error_type::no_more_positional_args,
					.arg_index = arg_index,
				});
			}

			const auto handler_index =
				dispatcher::positional_args_indexes[next_positional_arg_to_parse++];

			// TODO: This code is identical in both branches
			const auto handler = dispatcher::index_to_handler_map[handler_index];

			const auto handler_result = handler(result, argc, argv, {}, arg_index);

			if (!handler_result)
			{
				return compat::unexpected(handler_result.error());
			}
		}
	}

	return result;
}

} // namespace cli151
