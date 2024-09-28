#include <cli151/cli151.hpp>
#include <cli151/detail/reflect.hpp>

#include <iostream>
#include <optional>
#include <string_view>

namespace cli = cli151;

struct mycli
{
	int number;
	std::string_view name;
	std::optional<std::string_view> author;
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
		//&T::author,
		arg{&T::other_number, {.help = "Another number"}},
	};
};

int main(int argc, char* argv[])
{
	auto result = cli::parse<mycli>(argc, argv);

	if (!result)
	{
		std::cerr << "Error!\n";
	}
	else
	{
		const auto& out = result.value();
		std::cout << out.number << '\n';
		std::cout << out.name << '\n';
		std::cout << out.author.value_or("No author") << '\n';
		std::cout << out.other_number << '\n';
	}

	std::cout << cli::detail::get_member_name<&mycli::number>() << '\n';

	static_assert(cli::detail::get_member_name<&mycli::number>() == "number");

	std::cout << cli::detail::kebabbed_name<mycli, 0>::name << '\n';
	std::cout << cli::detail::kebabbed_name<mycli, 1>::name << '\n';
	std::cout << cli::detail::kebabbed_name<mycli, 2>::name << '\n';
}
