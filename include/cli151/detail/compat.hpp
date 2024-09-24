#pragma once

// https://github.com/llvm/llvm-project/issues/62801
// We want <expected> and C++23, unless we're on clang < 19 with libstdc++
#if __has_include(<expected>) && __cplusplus > 202300L && \
    !(defined __clang__&& defined __GLIBCXX__&& __clang_major__ < 19)
#include <expected>
namespace cli151::compat
{
using std::expected;
}
#else
#include <tl/expected.hpp>
namespace cli151::compat
{
using tl::expected;
}
#endif
