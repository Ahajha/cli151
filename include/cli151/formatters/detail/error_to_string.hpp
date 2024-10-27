#pragma once

#include <cli151/common.hpp>

namespace cli151::detail
{

constexpr inline auto error_to_string(const error_type err) -> std::string_view
{
	switch (err)
	{
	case error_type::invalid_key:
		return "Invalid key";
	case error_type::not_a_number:
		return "Not a number";
	case error_type::number_out_of_range:
		return "Number out of range";
	case error_type::no_more_positional_args:
		return "No more positional args";
	case error_type::missing_args:
		return "Missing args";
	case error_type::duplicate_arg:
		return "Duplicate argument";
	}
}

} // namespace cli151::detail
