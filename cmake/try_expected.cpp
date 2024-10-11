#include <expected>

int main()
{
	std::expected<int, int> foo;
	[[maybe_unused]] bar = foo.transform([](int) { return "Hello"; });
}
