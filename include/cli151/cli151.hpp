#pragma once

#include <cli151/common.hpp>
#include <cli151/detail/compat.hpp>
#include <cli151/detail/helpers.hpp>
#include <optional>

namespace cli151
{

template <class T, class Stream = FILE*>
auto parse(int argc, const char* const* argv, [[maybe_unused]] Stream errstream = stderr)
	-> std::optional<T>
{
	// T is probably an aggregate. We 0-initialize (or whichever type of initialization C++ calls
	// this) the result to prevent random values from appearing. Not sure if this should be part of
	// the public API, but could prevent some cryptic bugs.
	T result{};

	using dispatcher = detail::handler_dispatcher<T, Stream>;

	std::array<bool, dispatcher::index_to_handler_map.size()> used{};

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

		if (view.starts_with("--"))
		{
			// Long form

			const auto kw_result = detail::parse_long_keyword<T>(view, arg_index, errstream);
			if (!kw_result)
			{
				return {};
			}
			const auto [handler_index, value] = *kw_result;
			const auto handler = dispatcher::index_to_handler_map[handler_index];
			if (!handler(result, argc, argv, value, arg_index, used[handler_index], errstream))
			{
				return {};
			}
		}
		else if (view.starts_with('-'))
		{
			// Short form

			const auto kw_result = detail::parse_short_keyword<T>(view, arg_index, errstream);
			if (!kw_result)
			{
				return {};
			}
			const auto [handler_index, value] = *kw_result;
			const auto handler = dispatcher::index_to_handler_map[handler_index];
			if (!handler(result, argc, argv, value, arg_index, used[handler_index], errstream))
			{
				return {};
			}
		}
		else
		{
			// Positional

			if (next_positional_arg_to_parse >= dispatcher::positional_args_indexes.size())
			{
				// TODO: This may behave differently if there is a trailing container in T
				detail::output(errstream, "Too many positional args");
				return {};
			}

			const auto handler_index =
				dispatcher::positional_args_indexes[next_positional_arg_to_parse++];

			const auto handler = dispatcher::index_to_handler_map[handler_index];

			if (!handler(result, argc, argv, {}, arg_index, used[handler_index], errstream))
			{
				return {};
			}
		}
	}

	return result;
}

} // namespace cli151
