#pragma once

#include <cli151/detail/compat.hpp>
#include <cstdio>

namespace cli151::detail
{

// Wrapper to unify format_to() and print()

template <class OutputIt, class... Args>
OutputIt output(OutputIt out, compat::format_string<Args...> fmt, Args&&... args)
{
	return compat::format_to(out, fmt, std::forward<Args>(args)...);
}

template <class... Args>
std::FILE* output(std::FILE* out, compat::format_string<Args...> fmt, Args&&... args)
{
	compat::print(out, fmt, std::forward<Args>(args)...);
	return out;
}

} // namespace cli151::detail
