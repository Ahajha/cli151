#pragma once

#include <cli151/common.hpp>
#include <cli151/detail/compat.hpp>
#include <cli151/detail/helpers.hpp>

template <class CharType, class T>
struct cli151::compat::formatter<cli151::usage_formatter<T>, CharType>
{
	template <class context>
	constexpr auto parse(context& ctx) -> typename context::iterator
	{
		return ctx.begin();
	}

	template <class context>
	constexpr auto format(const cli151::usage_formatter<T>& usage, context& ctx) const ->
		typename context::iterator
	{
		using cli151::compat::format_to;

		const auto& help_data = cli151::detail::help_data_of<T>::data;

		format_to(ctx.out(), "Usage: {}", usage.prog_name);
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
		return format_to(ctx.out(), " [OPTIONS]");
	}
};
