#include <cli151/cli151.hpp>
#include <cli151/detail/reflect.hpp>

#include <iostream>
#include <string_view>

namespace cli = cli151;

struct mycli
{
	int number;

	std::string_view name;

	constexpr static auto args = cli::args(cli::arg{&mycli::number, "number", "n"}, &mycli::name);

	constexpr static auto args1 = cli::args(cli::arg{&mycli::number}, cli::arg{&mycli::name});
	constexpr static auto args2 = cli::args(&mycli::number, &mycli::name);
};

struct visitor
{
	void operator()(std::string_view name, int& n) { n = 3; }
	void operator()(std::string_view name, std::string_view& s) { s = "Hello, World!"; }

	template <typename T>
	void operator()(const T&)
	{}
};

int main(int argc, char* argv[])
{
	auto result = cli::parse<mycli>(argc, argv);

	std::cout << cli::detail::get_member_name<&mycli::number>() << '\n';

	static_assert(cli::detail::get_member_name<&mycli::number>() == "number");
}
