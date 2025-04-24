#pragma once

#if !__has_include(<charconv>)
#	define CLI151_HAS_FROM_CHARS false
#else
#	include <charconv> // IWYU pragma: keep (need charconv for __cpp_lib_to_chars)
#	if !defined __cpp_lib_to_chars
#		define CLI151_HAS_FROM_CHARS false
#	endif
#endif

#ifdef CLI151_HAS_FROM_CHARS
#	include <fast_float/fast_float.h>
namespace cli151::compat
{
using fast_float::from_chars;
}
#else
#	define CLI151_HAS_FROM_CHARS true
namespace cli151::compat
{
using std::from_chars;
}
#endif

#if !__has_include(<print>)
#	define CLI151_HAS_PRINT false
#else
#	include <print>
// Apple-clang 15 doesn't define __cpp_lib_print, even though it should.
// Having <print> and having >C++20 should be sufficient to test instead.
// We'll just test some arbitrary number higher than 202002L, since
// apple-clang 15 doesn't set it to 202302L in C++23 mode.
#	if __cplusplus < 202003L
#		define CLI151_HAS_PRINT false
#	endif
#endif

#ifdef CLI151_HAS_PRINT
#	include <fmt/base.h>
namespace cli151::compat
{
using fmt::format_string;
using fmt::format_to;
using fmt::formatter;
using fmt::print;
using fmt::println;
} // namespace cli151::compat
#else
#	define CLI151_HAS_PRINT true
namespace cli151::compat
{
using std::format_string;
using std::format_to;
using std::formatter;
using std::print;
using std::println;
} // namespace cli151::compat
#endif
