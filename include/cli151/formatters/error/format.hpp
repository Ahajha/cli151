#pragma once

#include <cli151/common.hpp>
#include <cli151/detail/compat.hpp>
#include <cli151/detail/helpers.hpp>
#include <cli151/formatters/detail/error_to_string.hpp>

template <class CharType, class T>
struct cli151::compat::formatter<cli151::error::error_formatter<T>, CharType>
{
	template <class context>
	constexpr auto parse(context& ctx) -> typename context::iterator
	{
		return ctx.begin();
	}

	template <class context>
	constexpr auto format(const cli151::error::error_formatter<T>& err, context& ctx) const ->
		typename context::iterator
	{
		assert(err.err.arg_index < err.argc);

		// TODO: Print the names of all positional args
		cli151::compat::format_to(ctx.out(), "{} [OPTIONS]\n", err.argv[0]);

		// Then print help text for all args (split into keyword and positional later)
		for (const auto& info : cli151::detail::handler_dispatcher<T>::index_to_help_map)
		{
			if (info.help == cli151::default_)
			{
				cli151::compat::format_to(ctx.out(), "    -{}, --{}\n", info.abbr, info.name);
			}
			else
			{
				cli151::compat::format_to(ctx.out(), "    -{}, --{}: {}\n", info.abbr, info.name,
				                          info.help);
			}
		}

		return cli151::compat::format_to(ctx.out(), "{} at position {} ({})",
		                                 cli151::detail::error_to_string(err.err.type),
		                                 err.err.arg_index, err.argv[err.err.arg_index]);
	}
};
