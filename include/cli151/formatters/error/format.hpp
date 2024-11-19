#pragma once

#include <cli151/common.hpp>
#include <cli151/detail/compat.hpp>
#include <cli151/detail/helpers.hpp>
#include <cli151/formatters/usage/format.hpp>

template <class CharType, class T>
struct cli151::compat::formatter<cli151::error_formatter<T>, CharType>
{
	template <class context>
	constexpr auto parse(context& ctx) -> typename context::iterator
	{
		return ctx.begin();
	}

	template <class context>
	constexpr auto format(const cli151::error_formatter<T>& err, context& ctx) const ->
		typename context::iterator
	{
		assert(err.err.arg_index < err.argc);

		using cli151::error_type;
		using cli151::compat::format_to;

		constexpr auto help_data = cli151::detail::help_data_of<T>::data;

		constexpr auto n_positional_args = std::count_if(
			help_data.begin(), help_data.end(),
			[](const auto& data) { return data.type == cli151::arg_type::positional_required; });

		constexpr auto n_keyword_args =
			std::count_if(help_data.begin(), help_data.end(),
		                  [](const auto& data) { return data.type == cli151::arg_type::keyword; });

		format_to(ctx.out(), "Error: ");
		switch (err.err.type)
		{
		case error_type::invalid_key:
			format_to(ctx.out(), "Unknown option \"{}\". Available options:\n",
			          err.argv[err.err.arg_index]);
			return format_options(help_data, ctx);
		case error_type::not_a_number:
			format_to(ctx.out(), "Not a number\n");
			// Let's not overcomplicate this. Just print which one is wrong, say it should be a
			// number. Print the usage.
			break;
		case error_type::number_out_of_range:
			format_to(ctx.out(), "Number out of range\n");
			// Similar to above
			break;
		case error_type::too_many_positional_args:
			// TODO: Print the range (min and max) and the number given (once positional optional
			// args are implemented)
			format_to(ctx.out(), "Too many positional args, expected {}, got {}\n",
			          n_positional_args, "too many");
			// Print short usage
			break;
		case error_type::not_enough_positional_args:
			format_to(ctx.out(), "Not enough positional args, expected {}, got {}\n",
			          n_positional_args, "not enough");
			// We don't actually check this yet
			break;
		case error_type::duplicate_arg:
			// TODO: Aliases might make this confusing
			format_to(ctx.out(), "Duplicate argument\n");
			break;
		}

		format_to(ctx.out(), "{}\n", cli151::usage_formatter<T>{err.argv[0]});

		if constexpr (n_positional_args != 0)
		{
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
		}

		if constexpr (n_keyword_args != 0)
		{
			format_to(ctx.out(), "Options:\n");
			format_options(help_data, ctx);
		}

		return ctx.out();
	}

  private:
	template <class context>
	constexpr auto format_options(const auto& help_data, context& ctx) const ->
		typename context::iterator
	{
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

		return ctx.out();
	}
};
