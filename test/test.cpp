#include <cli151/cli151.hpp>
#include <cli151/formatters/error/format.hpp>

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

int main(int argc, char* argv[])
{
	auto result = cli::parse<mycli>(argc, argv);

	if (!result)
	{
		cli::compat::println("Error: {}", result.error().formatter<mycli>(argc, argv));
	}
	else
	{
		const auto& out = result.value();
		cli::compat::println("number: {}", out.number);
		cli::compat::println("name: {}", out.name);
		cli::compat::println("author: {}", out.author.value_or("No author"));
		cli::compat::println("other_number: {}", out.other_number);
		cli::compat::println("flag: {}", out.flag);
	}
}
