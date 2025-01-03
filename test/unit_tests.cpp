#include <cli151/cli151.hpp>
namespace cli = cli151;

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <array>
#include <cstdint>
#include <set>
#include <string_view>
#include <tuple>
#include <unordered_set>
#include <utility>

// For now, error tests only check that the parse failed. These tests should be extended once the
// error interface is more stable.

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

TEST_CASE("keyword args (wrong param)")
{
	constexpr std::array args{
		"main", "--steven=7.89", "-9", "bob", "--1234567890=123",
	};
	const auto result = cli::parse<keyword_only>(args.size(), args.data());
	REQUIRE(!result);
}

TEST_CASE("keyword args (missing arg)")
{
	constexpr std::array args{"main", "-1"};
	const auto result = cli::parse<keyword_only>(args.size(), args.data());
	REQUIRE(!result);
}

TEST_CASE("keyword args (duplicate arg)")
{
	constexpr std::array args{"main", "-1=345", "--arg1", "456"};
	const auto result = cli::parse<keyword_only>(args.size(), args.data());
	REQUIRE(!result);
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

template <class Int>
struct int_only
{
	Int number;
};
template <class Int>
struct cli::meta<int_only<Int>>
{
	using T = int_only<Int>;
	constexpr static auto value = args{&T::number};
};

TEST_CASE_TEMPLATE("Integrals and floats", T, std::uint8_t, std::int8_t, std::uint16_t,
                   std::int16_t, std::uint32_t, std::int32_t, std::uint64_t, std::int64_t, float,
                   double)
{
	constexpr std::array args{"main", "1"};
	const auto result = cli::parse<int_only<T>>(args.size(), args.data());
	REQUIRE(result);

	CHECK(result.value().number == T(1));
}

// Skip std::uint64_t since we're testing with the max of that type
TEST_CASE_TEMPLATE("Overflow", Int, std::uint8_t, std::int8_t, std::uint16_t, std::int16_t,
                   std::uint32_t, std::int32_t, std::int64_t)
{
	constexpr std::array args{"main", "18446744073709551615"};
	const auto result = cli::parse<int_only<Int>>(args.size(), args.data());
	REQUIRE(!result);
}

TEST_CASE_TEMPLATE("Not a number", Int, std::uint8_t, std::int8_t, std::uint16_t, std::int16_t,
                   std::uint32_t, std::int32_t, std::uint64_t, std::int64_t, float, double)
{
	constexpr std::array args{"main", "not_a_number"};
	const auto result = cli::parse<int_only<Int>>(args.size(), args.data());
	REQUIRE(!result);
}

struct underscores
{
	std::optional<int> this_keyword_has_a_lot_of_underscores;
};
template <>
struct cli::meta<underscores>
{
	using T = underscores;
	constexpr static auto value = args{&T::this_keyword_has_a_lot_of_underscores};
};

TEST_CASE("Autokebabbing of names")
{
	constexpr std::array args{"main", "--this-keyword-has-a-lot-of-underscores", "234567"};
	const auto result = cli::parse<underscores>(args.size(), args.data());
	REQUIRE(result);

	CHECK(result.value().this_keyword_has_a_lot_of_underscores == 234567);
}

struct sets
{
	std::set<int> ints;
	std::set<std::string_view> strs;
};
template <>
struct cli::meta<sets>
{
	using T = sets;
	constexpr static auto value = args{&T::ints, &T::strs};
};

TEST_CASE("std::set")
{
	constexpr std::array args{"main", "--ints", "123", "-s",     "hello",     "--ints",
	                          "456",  "-i",     "789", "-i=234", "--ints=234"};
	const auto result = cli::parse<sets>(args.size(), args.data());
	REQUIRE(result);

	CHECK(result.value().ints == std::set<int>{123, 456, 789, 234});
	CHECK(result.value().strs == std::set<std::string_view>{"hello"});
}

TEST_CASE("std::set (failure)")
{
	constexpr std::array args{"main", "--ints", "notanint"};
	const auto result = cli::parse<sets>(args.size(), args.data());
	REQUIRE(!result);
}

struct unordered_sets
{
	std::unordered_set<int> ints;
	std::unordered_set<std::string_view> strs;
};
template <>
struct cli::meta<unordered_sets>
{
	using T = unordered_sets;
	constexpr static auto value = args{&T::ints, &T::strs};
};

TEST_CASE("std::unordered_set")
{
	constexpr std::array args{"main", "--ints", "123", "-s",     "hello",     "--ints",
	                          "456",  "-i",     "789", "-i=234", "--ints=234"};
	const auto result = cli::parse<unordered_sets>(args.size(), args.data());
	REQUIRE(result);

	CHECK(result.value().ints == std::unordered_set<int>{123, 456, 789, 234});
	CHECK(result.value().strs == std::unordered_set<std::string_view>{"hello"});
}

TEST_CASE("std::unordered_set (failure)")
{
	constexpr std::array args{"main", "--ints", "notanint"};
	const auto result = cli::parse<unordered_sets>(args.size(), args.data());
	REQUIRE(!result);
}

struct pairs
{
	std::pair<int, std::string_view> first;
	std::optional<std::pair<int, double>> second;
};
template <>
struct cli::meta<pairs>
{
	using T = pairs;
	constexpr static auto value = args{&T::first, &T::second};
};

TEST_CASE("std::pair")
{
	constexpr std::array args{"main", "123", "hello", "--second", "456", "34.5"};
	const auto result = cli::parse<pairs>(args.size(), args.data());
	REQUIRE(result);

	CHECK(result.value().first.first == 123);
	CHECK(result.value().first.second == "hello");
	CHECK(result.value().second.value().first == 456);
	CHECK(result.value().second.value().second == 34.5);
}

TEST_CASE("std::pair (failure, not enough elements)")
{
	constexpr std::array args{"main", "123"};
	const auto result = cli::parse<pairs>(args.size(), args.data());
	REQUIRE(!result);
}

TEST_CASE("std::pair (failure, parse failure of element)")
{
	constexpr std::array args{"main", "hello", "world"};
	const auto result = cli::parse<pairs>(args.size(), args.data());
	REQUIRE(!result);
}

struct tuples
{
	std::optional<std::tuple<int, std::string_view, double>> value;
};
template <>
struct cli::meta<tuples>
{
	using T = tuples;
	constexpr static auto value = args{&T::value};
};

TEST_CASE("std::tuple")
{
	constexpr std::array args{"main", "--value", "123", "hello", "34.5"};
	const auto result = cli::parse<tuples>(args.size(), args.data());
	REQUIRE(result);

	CHECK(std::get<0>(result.value().value.value()) == 123);
	CHECK(std::get<1>(result.value().value.value()) == "hello");
	CHECK(std::get<2>(result.value().value.value()) == 34.5);
}

struct arrays
{
	std::array<int, 5> values;
};
template <>
struct cli::meta<arrays>
{
	using T = arrays;
	constexpr static auto value = args{&T::values};
};

TEST_CASE("std::array")
{
	constexpr std::array args{"main", "10", "20", "30", "40", "50"};
	const auto result = cli::parse<arrays>(args.size(), args.data());
	REQUIRE(result);

	CHECK(result.value().values == std::array<int, 5>{10, 20, 30, 40, 50});
}

#include <cli151/macros.hpp>

struct cli2
{
	std::string_view first, middle, last;
};
CLI151_CLI(cli2, &T::first, &T::middle, &T::last)

TEST_CASE("Macro definition")
{
	constexpr std::array args{"main", "Francesco", "Von", "Pellegreno"};
	const auto result = cli::parse<cli2>(args.size(), args.data());
	REQUIRE(result);

	CHECK(result.value().first == "Francesco");
	CHECK(result.value().middle == "Von");
	CHECK(result.value().last == "Pellegreno");
}
