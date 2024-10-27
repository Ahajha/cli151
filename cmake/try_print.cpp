#include <print>

// FIXME: Apple clang 15 should support this, but this feature test macro isn't set.
#if !defined __cpp_lib_print
#	error "Not sure why this is the case"
#endif

int main() { std::println("Hello, {}!", "world"); }
