#pragma once

// https://github.com/llvm/llvm-project/issues/62801
// We want <expected> and C++23, unless we're on clang < 19 with libstdc++
#if !__has_include(<expected>)
#define CLI151_HAS_STD_EXPECTED false
#else
#include <expected>
#if !defined __cpp_lib_expected
#define CLI151_HAS_STD_EXPECTED false
#elif defined __clang__ && defined __GLIBCXX__ && __clang_major__ < 19
#define CLI151_HAS_STD_EXPECTED false
#endif
#endif

#ifdef CLI151_HAS_STD_EXPECTED
#include <tl/expected.hpp>
namespace cli151::compat
{
using tl::expected;
}
#else
#define CLI151_HAS_STD_EXPECTED true
namespace cli151::compat
{
using std::expected;
}
#endif
