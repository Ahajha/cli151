#pragma once

#include <expected>

namespace cli151
{

// temporary
using error = int;

template <class T>
using expected = std::expected<T, error>;

} // namespace cli151
