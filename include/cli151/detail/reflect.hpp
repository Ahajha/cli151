#pragma once

// Borrowed from reflect-cpp. We only need a small part, and is otherwise a
// large dependency (lots of subdependencies).
// https://github.com/getml/reflect-cpp/blob/main/include/rfl/internal/get_field_names.hpp

#include <string_view>

#if __has_include(<source_location>)
#include <source_location>
#endif

namespace cli151::detail
{

template <class T, auto ptr>
consteval auto get_field_name_str_view()
{
#if __cpp_lib_source_location >= 201907L
	const auto func_name = std::string_view{std::source_location::current().function_name()};
#elif defined(_MSC_VER)
	// Officially, we only support MSVC versions that are modern enough to contain
	// <source_location>, but inofficially, this might work.
	const auto func_name = std::string_view{__FUNCSIG__};
#else
	const auto func_name = std::string_view{__PRETTY_FUNCTION__};
#endif
#if defined(__clang__)
	const auto split = func_name.substr(0, func_name.size() - 1);
	return split.substr(split.find_last_of(":.") + 1);
#elif defined(__GNUC__)
	const auto split = func_name.substr(0, func_name.size() - 1);
	return split.substr(split.find_last_of(":") + 1);
#elif defined(_MSC_VER)
	const auto split = func_name.substr(0, func_name.size() - 7);
	return split.substr(split.rfind("->") + 2);
#else
	static_assert(false, "You are using an unsupported compiler. Please use GCC, Clang, or MSVC.");
#endif
}

template <class T, auto ptr>
consteval auto get_source_location()
{
	return std::string_view{std::source_location::current().function_name()};
}

} // namespace cli151::detail
