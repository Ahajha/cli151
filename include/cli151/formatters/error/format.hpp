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

		using cli151::compat::format_to;

		const auto& help_data = cli151::detail::help_data_of<T>::data;

		format_to(ctx.out(), "{}", err.argv[0]);
		// TODO: Currently this is assuming the order of the positional args
		for (const auto& data : help_data)
		{
			if (data.type == cli151::arg_type::positional_required)
			{
				// TODO: This is kebabbed, don't want that
				format_to(ctx.out(), " {}", data.name);
			}
		}
		// TODO: skip [OPTIONS] if no keyword args
		format_to(ctx.out(), " [OPTIONS]\n");

		// TODO: skip this one if no positional args
		format_to(ctx.out(), "Positional arguments:\n");
		for (const auto& data : help_data)
		{
			if (data.type == cli151::arg_type::positional_required)
			{
				// TODO: This is kebabbed, don't want that
				format_to(ctx.out(), "    {}", data.name);

				if (data.help != cli151::default_)
				{
					format_to(ctx.out(), ": {}", data.help);
				}
				format_to(ctx.out(), "\n");
			}
		}

		// TODO: Skip if no keyword args
		format_to(ctx.out(), "Options:\n");
		for (const auto& data : help_data)
		{
			if (data.type == cli151::arg_type::keyword)
			{
				// TODO: Only one might be available
				format_to(ctx.out(), "    -{}, --{}", data.abbr, data.name);

				if (data.help != cli151::default_)
				{
					format_to(ctx.out(), ": {}", data.help);
				}
				format_to(ctx.out(), "\n");
			}
		}

		return format_to(ctx.out(), "{} at position {} ({})",
		                 cli151::detail::error_to_string(err.err.type), err.err.arg_index,
		                 err.argv[err.err.arg_index]);
	}
};
