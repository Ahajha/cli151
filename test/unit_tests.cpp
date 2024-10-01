#include <cli151/cli151.hpp>
namespace cli = cli151;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <string_view>

struct positional_only
{
	int arg1;
	std::string_view arg2;
	int arg3;
	std::string_view arg4;
	double arg5;
	std::string_view arg6;
};
template <>
struct cli::meta<positional_only>
{
	using T = positional_only;
	constexpr static auto value = args{&T::arg1, &T::arg2, &T::arg3, &T::arg4, &T::arg5, &T::arg6};
};

TEST_CASE("positional args")
{
	constexpr std::array args{"main", "123", "alice", "456", "bob", "7.89", "charlie"};
	const auto result = cli::parse<positional_only>(args.size(), args.data());
	REQUIRE(result);

	const auto [a1, a2, a3, a4, a5, a6] = result.value();
	CHECK(a1 == 123);
	CHECK(a2 == "alice");
	CHECK(a3 == 456);
	CHECK(a4 == "bob");
	CHECK(a5 == 7.89);
	CHECK(a6 == "charlie");
}

// TEST_CASE("positional args (not enough)")
// {
// 	constexpr std::array args{"main", "123"};
// 	const auto result = cli::parse<positional_only>(args.size(), args.data());
// 	REQUIRE(!result);
// }

TEST_CASE("positional args (too many)")
{
	constexpr std::array args{
		"main",    "123",    "alice",  "456",    "bob",    "7.89",
		"charlie", "steven", "jeremy", "alexis", "markus", "peter",
	};
	const auto result = cli::parse<positional_only>(args.size(), args.data());
	REQUIRE(!result);
}

struct keyword_only
{
	std::optional<int> arg1;
	std::optional<std::string_view> arg2;
	std::optional<int> arg3;
	std::optional<std::string_view> arg4;
	std::optional<double> arg5;
	std::optional<std::string_view> arg6;
};
template <>
struct cli::meta<keyword_only>
{
	using T = keyword_only;
	constexpr static auto value = args{
		arg{&T::arg1, {.abbr = "1"}}, arg{&T::arg2, {.abbr = "2"}}, arg{&T::arg3, {.abbr = "3"}},
		arg{&T::arg4, {.abbr = "4"}}, arg{&T::arg5, {.abbr = "5"}}, arg{&T::arg6, {.abbr = "6"}},
	};
};

TEST_CASE("keyword args")
{
	constexpr std::array args{"main",   "--arg1", "123",    "--arg2", "alice",  "--arg3", "456",
	                          "--arg4", "bob",    "--arg5", "7.89",   "--arg6", "charlie"};
	const auto result = cli::parse<keyword_only>(args.size(), args.data());
	REQUIRE(result);

	const auto [a1, a2, a3, a4, a5, a6] = result.value();
	CHECK(a1.value() == 123);
	CHECK(a2.value() == "alice");
	CHECK(a3.value() == 456);
	CHECK(a4.value() == "bob");
	CHECK(a5.value() == 7.89);
	CHECK(a6.value() == "charlie");
}

TEST_CASE("keyword args (abbreviations)")
{
	constexpr std::array args{
		"main", "-1", "123", "-2", "alice", "-3", "456", "-4", "bob", "-5", "7.89", "-6", "charlie",
	};
	const auto result = cli::parse<keyword_only>(args.size(), args.data());
	REQUIRE(result);

	const auto [a1, a2, a3, a4, a5, a6] = result.value();
	CHECK(a1.value() == 123);
	CHECK(a2.value() == "alice");
	CHECK(a3.value() == 456);
	CHECK(a4.value() == "bob");
	CHECK(a5.value() == 7.89);
	CHECK(a6.value() == "charlie");
}

TEST_CASE("keyword args (abbreviations, out of order)")
{
	constexpr std::array args{
		"main", "-5", "7.89", "-2", "alice", "-4", "bob", "-1", "123", "-6", "charlie", "-3", "456",
	};
	const auto result = cli::parse<keyword_only>(args.size(), args.data());
	REQUIRE(result);

	const auto [a1, a2, a3, a4, a5, a6] = result.value();
	CHECK(a1.value() == 123);
	CHECK(a2.value() == "alice");
	CHECK(a3.value() == 456);
	CHECK(a4.value() == "bob");
	CHECK(a5.value() == 7.89);
	CHECK(a6.value() == "charlie");
}

TEST_CASE("keyword args (some missing, out of order)")
{
	constexpr std::array args{
		"main", "-5", "7.89", "-4", "bob", "-1", "123",
	};
	const auto result = cli::parse<keyword_only>(args.size(), args.data());
	REQUIRE(result);

	const auto [a1, a2, a3, a4, a5, a6] = result.value();
	CHECK(a1.value() == 123);
	CHECK(!a2.has_value());
	CHECK(!a3.has_value());
	CHECK(a4.value() == "bob");
	CHECK(a5.value() == 7.89);
	CHECK(!a6.has_value());
}

TEST_CASE("keyword args (some missing, out of order, using =, mixed short and long)")
{
	constexpr std::array args{
		"main", "--arg5=7.89", "-4", "bob", "-1=123",
	};
	const auto result = cli::parse<keyword_only>(args.size(), args.data());
	REQUIRE(result);

	const auto [a1, a2, a3, a4, a5, a6] = result.value();
	CHECK(a1.value() == 123);
	CHECK(!a2.has_value());
	CHECK(!a3.has_value());
	CHECK(a4.value() == "bob");
	CHECK(a5.value() == 7.89);
	CHECK(!a6.has_value());
}

struct keyword_positional_mixed
{
	long arg1;
	std::string_view arg2;
	short arg3;
	std::optional<std::string_view> arg4;
	std::optional<double> arg5;
	std::optional<std::string_view> arg6;
	bool flag1 = false;
	bool flag2 = true;
};
template <>
struct cli::meta<keyword_positional_mixed>
{
	using T = keyword_positional_mixed;
	constexpr static auto value = args{
		&T::arg1,
		&T::arg2,
		&T::arg3,
		arg{&T::arg4, {.abbr = "4"}},
		arg{&T::arg5, {.abbr = "5"}},
		arg{&T::arg6, {.abbr = "6"}},
		arg{&T::flag1, {.abbr = "f"}},
		arg{&T::flag2, {.abbr = "g"}},
	};
};

TEST_CASE("keyword and positional args")
{
	constexpr std::array args{
		"main", "64", "hello", "81", "--arg5=7.89", "-4", "bob", "--flag1",
	};
	const auto result = cli::parse<keyword_positional_mixed>(args.size(), args.data());
	REQUIRE(result);

	const auto [a1, a2, a3, a4, a5, a6, f1, f2] = result.value();
	CHECK(a1 == 64);
	CHECK(a2 == "hello");
	CHECK(a3 == 81);
	CHECK(a4.value() == "bob");
	CHECK(a5.value() == 7.89);
	CHECK(!a6.has_value());
	CHECK(f1);
	CHECK(f2);
}
