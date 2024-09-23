#include <cli151/cli151.hpp>
#include <cli151/detail/reflect.hpp>

#include <iostream>
#include <string_view>

namespace cli = cli151;

struct mycli
{
	int number;
	std::string_view name;
	int other_number;
};

template <>
struct cli::meta<mycli>
{
	using T = mycli;
	constexpr static auto value = args{
		// Conflict between name and number for short name
		arg{&T::number, {.help = "The number", .abbr = "r", .arg_name = "first_number"}},
		&T::name,
		arg{&T::other_number, {.help = "Another number"}},
	};
};

int main(int argc, char* argv[])
{
	auto result = cli::parse<mycli>(argc, argv);

	std::cout << cli::detail::get_member_name<&mycli::number>() << '\n';

	static_assert(cli::detail::get_member_name<&mycli::number>() == "number");

	std::cout << cli::detail::kebabbed_name<mycli, 0>::name << '\n';
	std::cout << cli::detail::kebabbed_name<mycli, 1>::name << '\n';
	std::cout << cli::detail::kebabbed_name<mycli, 2>::name << '\n';
}
