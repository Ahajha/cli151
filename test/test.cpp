#include <cli151/cli151.hpp>
#include <cli151/detail/reflect.hpp>
#include <cli151/formatters/error/format.hpp>

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
	bool flag = true;
};

template <>
struct cli::meta<mycli>
{
	using T = mycli;
	constexpr static auto value = args{
		// Conflict between name and number for short name
		arg{&T::number, {.help = "The number", .abbr = "r", .arg_name = "first_number"}},
		&T::name,
		&T::author,
		arg{&T::other_number, {.help = "Another number"}},
		&T::flag,
	};
};

auto to_string_view(frozen::string str) -> std::string_view { return {str.data(), str.size()}; }

auto parse_and_print_results(const int argc, const char* const* argv) -> void
{
	auto result = cli::parse<mycli>(argc, argv);

	if (!result)
	{
		cli::compat::println("Error: {}", result.error().formatter(argc, argv));
	}
	else
	{
		const auto& out = result.value();
		std::cout << "number: " << out.number << '\n';
		std::cout << "name: " << out.name << '\n';
		std::cout << "author: " << out.author.value_or("No author") << '\n';
		std::cout << "other_number: " << out.other_number << '\n';
		std::cout << "flag: " << out.flag << '\n';
	}
}

int main(int argc, char* argv[])
{
	parse_and_print_results(argc, argv);
	// constexpr std::array args{"main", "123",      "--other-number", "456",
	//                           "bob",  "--author", "steve",          "--flag"};

	// parse_and_print_results(args.size(), args.data());

	std::cout << cli::detail::get_member_name<&mycli::number>() << '\n';

	static_assert(cli::detail::get_member_name<&mycli::number>() == "number");

	std::cout << cli::detail::kebabbed_name<mycli, 0>::name << '\n';
	std::cout << cli::detail::kebabbed_name<mycli, 1>::name << '\n';
	std::cout << cli::detail::kebabbed_name<mycli, 2>::name << '\n';
	std::cout << cli::detail::kebabbed_name<mycli, 3>::name << '\n';

	static_assert(cli::detail::type_of_arg<mycli, 0>() == cli::arg_type::positional_required);
	static_assert(cli::detail::type_of_arg<mycli, 1>() == cli::arg_type::positional_required);
	static_assert(cli::detail::type_of_arg<mycli, 2>() == cli::arg_type::keyword);
	static_assert(cli::detail::type_of_arg<mycli, 3>() == cli::arg_type::keyword);

	for (const auto& [key, value] : cli::detail::handler_dispatcher<mycli>::long_name_to_index_map)
	{
		std::cout << "Key: " << to_string_view(key) << ", Value: " << value << '\n';
	}
}
