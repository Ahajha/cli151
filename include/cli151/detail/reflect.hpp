#pragma once

#include <cli151/detail/concepts.hpp>

#include <string_view>

namespace cli151::detail2
{

#ifndef __GNUC__
struct ptr
{
	const void* ptr;
};

template <class T>
extern T external;

// MSVC needs a little extra help with inserting the field name into the function name
template <auto Memptr>
[[nodiscard]] consteval auto get_function_name() -> std::string_view
{
	return std::string_view{__FUNCSIG__};
}
#endif

template <auto Memptr>
[[nodiscard]] consteval auto get_full_function_name() -> std::string_view
{
#ifdef __GNUC__
	return std::string_view{__PRETTY_FUNCTION__};
#else
	using T = detail::pointer_to_member<decltype(Memptr)>::class_;
	return get_function_name<ptr{&(external<T>.*Memptr)}>();
#endif
}

} // namespace cli151::detail2

namespace cli151::detail
{

template <auto Ptr>
[[nodiscard]] consteval auto get_member_name() -> std::string_view
{
	// Programmatically figure out where the field name is within the string
	struct field_name_detector
	{
		void* dummy;
	};
	constexpr auto detector_name = detail2::get_full_function_name<&field_name_detector::dummy>();
	constexpr auto dummy_begin = detector_name.rfind(std::string_view("dummy"));
	constexpr auto suffix = detector_name.substr(dummy_begin + std::string_view("dummy").size());
	constexpr auto begin_sentinel = detector_name[dummy_begin - 1];

	// Then look there in the general case
	const auto field_name_raw = detail2::get_full_function_name<Ptr>();
	const auto last = field_name_raw.rfind(suffix);
	const auto begin = field_name_raw.rfind(begin_sentinel, last - 1) + 1;

	return field_name_raw.substr(begin, last - begin);
}

} // namespace cli151::detail
