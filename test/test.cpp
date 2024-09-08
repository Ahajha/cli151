#include <cli151/cli151.hpp>
#include <cli151/detail/reflect.hpp>

#include <iostream>
#include <string_view>

namespace cli = cli151;

struct mycli
{
	int number;
	std::string_view name;
};

template <>
struct cli::meta<mycli>
{
	using T = mycli;
	constexpr static auto value = args{
		// Conflict between name and number for short name
		arg{.ptr = &T::number, .help = "The number", .abbr = "r"},
		arg{.ptr = &T::name, .help = "The name"},
	};
};

int main(int argc, char* argv[])
{
	auto result = cli::parse<mycli>(argc, argv);

	std::cout << cli::detail::get_member_name<&mycli::number>() << '\n';

	static_assert(cli::detail::get_member_name<&mycli::number>() == "number");
}
