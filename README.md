# CLI151

A header-only C++20 library for constructing CLI parsers at compile time. CLI151 supports reflection of field names, has no dynamic memory allocation, and works without exceptions.

CLI151 is still early in development, but actively worked on. Expect bugs, but also expect issues to be responded to.

## How to use

```c++
#include <cli151/cli151.hpp>
namespace cli = cli151;

#include <optional>
#include <string_view>

// Define the struct
struct mycli
{
    int number;
    std::optional<std::string_view> author;
    bool cool_flag = false;
};

// Define the meta information
template <>
struct cli::meta<mycli>
{
    using T = mycli;
    constexpr static auto value = args{
        // Anything up to the first std::optional or bool is positional and required
        &T::number,
        // From the first std::optional or bool onward, the options are keyword and optional
        &T::author,
        &T::cool_flag,
    };
    // The order is not required to be the same as in the struct, but it is generally assumed they are.
};

// Parse
int main(int argc, char* argv[])
{
    // Returns a cli151::expected<mycli>
    const auto result = cli::parse<mycli>(argc, argv);

    if (result)
    {
        const auto [number, author, cool_flag] = result.value();

        // Use the result
    }
    else
    {
        // Error!
    }
}
```

There also exists a convenience macro in a separate header. The following is an equivalent way to define the meta information (T is automatically added as an alias to the given type):

```c++
#include <cli151/macros.hpp>

CLI151_CLI(mycli, &T::number, &T::author, &T::cool_flag)
```

Pass the arguments as:
`./program 123 --author Ahajha --cool-flag`
- `cool_flag` is flipped when specified
- Note that underscores are converted to hyphens in field names

It also supports short keywords:
`./program 123 -a Ahajha -c`

Supports out of order args:
`./program -a Ahajha 123 -c`

Optional args aren't necessary:
`./program 123`
- `cool_flag` is kept as the default, author is `std::nullopt`

Args can also be specified with `=`
`./program 123 --author=Ahajha`
`./program 123 -a=Ahajha`

## Notes

- Prefer `std::string_view` (or `const char*`) over `std::string` in your cli structs, unless you plan to do additional modifications to the argument after parsing. If using these types, they will reference the contents of `argv` directly, with no intermediate allocations.

## Including in your project

Work in progress - Future goals are to directly support FetchContent and Conan.

## Compiler support

GCC 10+
Clang 10+ (with libstdc++ or libc++)
MSVC 2019+
Apple clang 13+ (12 _might_ work, but is untested)

## Advanced usage

Additional information for each field can be given:
```c++
template <>
struct cli::meta<mycli>
{
    using T = mycli;
    constexpr static auto value = args{
        // Member pointers and `arg` structs can be mixed and matched
        &T::number,
        &T::author,
        arg{&T::cool_flag, { // Note that this is a substruct
            // Help text
            .help = "This program is really cool if you pass this in!",
            // Short name - by default is the first letter of arg_name
            // (which in turn defaults to the field name).
            .abbr = "f",
            // Override the field name, also note that this overrides the underscore-to-hyphen conversion.
            .arg_name = "really_cool_flag",
        }},
    };
};
```
`abbr` and `arg_name` can be set to `""` to disable it, this is useful if you want a keyword arg that only accepts a long or short form, for example allowing `--cool-flag` but disallowing `-c`.

## Troubleshooting

If you're getting cryptic template errors about constexpr recursion depth, it's likely you have duplicate short field names. See [frozen's troubleshooting guide](https://github.com/serge-sans-paille/frozen?tab=readme-ov-file#troubleshooting).

For example, the struct
```c++
struct mycli
{
    std::string_view name;
    int number;
};
```
would cause this error since both fields abbreviate to `-n`.

## Dependencies

- `frozen` for compile time hash maps
- `tl::expected` when `std::expected` isn't available (either if the compiler doesn't support it or if in C++20 mode)
- `fast_float` when `std::from_chars` isn't available
- `fmt` when `std::format` or `std::print(ln)` isn't available (either if the compiler doesn't support it or if in C++20 mode)
- `doctest` for unit tests

## Inspirations

- [structopt](https://github.com/p-ranav/structopt) - For the idea of specifying CLI information declaratively rather than constructing a parser iteratively.
- [glaze](https://github.com/stephenberry/glaze) - For the idea of using `meta` structs to add information about a struct at compile time.
- [reflect-cpp](https://github.com/getml/reflect-cpp) - For the idea of how to implement compile time reflection of field names from member function pointers.

## Future plans

- Supporting more types (tuple, vector, set, map, array, enums(!), etc.)
- Autogenerated help text
- Compile time, runtime, and memory footprint improvements benchmarks (A cli parser doesn't _need_ to be fast, but why not?)
- More convenience functions and macros
- More configuration options
- Testing, testing, testing
- Likely more to come!

Contributions are welcome!
