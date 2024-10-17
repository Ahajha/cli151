#pragma once

#include <optional>
#include <type_traits>

namespace cli151::detail
{

template <class T>
struct is_optional : std::false_type
{};

template <class T>
struct is_optional<std::optional<T>> : std::true_type
{};

template <class T>
struct pointer_to_member
{};

template <class C, class M>
struct pointer_to_member<M C::*>
{
	using class_ = C;
	using member = M;
};

template <class T>
concept set_like = requires(T t, typename T::value_type e) { t.insert(e); };

} // namespace cli151::detail
