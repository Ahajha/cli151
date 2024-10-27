#pragma once

// Also check for monadic operations support
#if !__has_include(<expected>)
#	define CLI151_HAS_STD_EXPECTED false
#else
#	include <expected>
#	if !defined __cpp_lib_expected || __cpp_lib_expected < 202211L
#		define CLI151_HAS_STD_EXPECTED false
#	endif
#endif

#ifdef CLI151_HAS_STD_EXPECTED
#	include <tl/expected.hpp>
namespace cli151::compat
{
using tl::expected;
using tl::unexpected;
} // namespace cli151::compat
#else
#	define CLI151_HAS_STD_EXPECTED true
namespace cli151::compat
{
using std::expected;
using std::unexpected;
} // namespace cli151::compat
#endif

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
#	if !defined __cpp_lib_print
#		define CLI151_HAS_PRINT false
#	endif
#endif

#ifdef CLI151_HAS_PRINT
#	include <fmt/base.h>
namespace cli151::compat
{
using fmt::formatter;
using fmt::print;
using fmt::println;
} // namespace cli151::compat
#else
#	define CLI151_HAS_PRINT true
namespace cli151::compat
{
using std::formatter;
using std::print;
using std::println;
} // namespace cli151::compat
#endif
