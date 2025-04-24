#pragma once

#include <cli151/detail/compat.hpp>
#include <cstdio>
#include <type_traits>
#include <utility>

namespace cli151::detail
{

// Wrapper to unify format_to() and print()

template <class OutputIt, class... Args>
	requires(!std::is_same_v<OutputIt, FILE*>)
OutputIt output(OutputIt out, compat::format_string<Args...> fmt, Args&&... args)
{
	return compat::format_to(out, fmt, std::forward<Args>(args)...);
}

template <class... Args>
FILE* output(FILE* out, compat::format_string<Args...> fmt, Args&&... args)
{
	compat::print(out, fmt, std::forward<Args>(args)...);
	return out;
}

} // namespace cli151::detail
