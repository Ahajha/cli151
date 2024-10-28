#pragma once

#include <cli151/common.hpp>
#include <cli151/detail/compat.hpp>
#include <cli151/formatters/detail/error_to_string.hpp>

template <class CharType>
struct cli151::compat::formatter<cli151::error::error_formatter, CharType>
{
	template <class context>
	constexpr auto parse(context& ctx) -> typename context::iterator
	{
		return ctx.begin();
	}

	template <class context>
	constexpr auto format(const cli151::error::error_formatter& err, context& ctx) const ->
		typename context::iterator
	{
		assert(err.err.arg_index < err.argc);
		return cli151::compat::format_to(ctx.out(), "{} at position {} ({})",
		                                 cli151::detail::error_to_string(err.err.type),
		                                 err.err.arg_index, err.argv[err.err.arg_index]);
	}
};
