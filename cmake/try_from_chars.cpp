#include <charconv>
#include <cstdint>
#include <string_view>

const std::string_view str = "42";

template <class T>
void test()
{
	T out;
	[[maybe_unused]] const auto result = std::from_chars(str.data(), str.data() + str.size(), out);
}

int main()
{
	test<std::int8_t>();
	test<std::uint8_t>();
	test<std::int16_t>();
	test<std::uint16_t>();
	test<std::int32_t>();
	test<std::uint32_t>();
	test<std::int64_t>();
	test<std::uint64_t>();
	test<float>();
	test<double>();
	test<long double>();
}
