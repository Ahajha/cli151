#pragma once

#define CLI151_CLI(STRUCT_NAME, ...)                                                               \
	template <>                                                                                    \
	struct ::cli151::meta<STRUCT_NAME>                                                             \
	{                                                                                              \
		using T = STRUCT_NAME;                                                                     \
		constexpr static auto value = args{__VA_ARGS__};                                           \
	};
