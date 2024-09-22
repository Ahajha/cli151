#pragma once

#include <type_traits>

namespace detail
{

template <class T>
struct is_member_pointer : std::false_type
{};

template <class Class, class Member>
struct is_member_pointer<Member Class::*> : std::true_type
{};

} // namespace detail
