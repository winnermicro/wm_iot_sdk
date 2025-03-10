/*
 * Copyright (c) 2021, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Internals for looping macros
 *
 * Repetitive or obscure helper macros needed by sys/util.h.
 */

#ifndef ZEPHYR_INCLUDE_SYS_UTIL_LOOPS_H_
#define ZEPHYR_INCLUDE_SYS_UTIL_LOOPS_H_

#define Z_FOR_LOOP_GET_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, \
				_12, _13, _14, _15, _16, _17, _18, _19, _20, \
				_21, _22, _23, _24, _25, _26, _27, _28, _29, \
				_30, _31, _32, _33, _34, _35, _36, _37, _38, \
				_39, _40, _41, _42, _43, _44, _45, _46, _47, \
				_48, _49, _50, _51, _52, _53, _54, _55, _56, \
				_57, _58, _59, _60, _61, _62, _63, _64, N, ...) N

#define Z_FOR_LOOP_0(z_call, sep, fixed_arg0, fixed_arg1, ...)

#define Z_FOR_LOOP_1(z_call, sep, fixed_arg0, fixed_arg1, x) \
	z_call(0, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_2(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_1(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(1, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_3(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_2(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(2, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_4(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_3(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(3, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_5(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_4(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(4, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_6(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_5(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(5, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_7(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_6(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(6, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_8(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_7(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(7, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_9(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_8(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(8, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_10(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_9(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(9, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_11(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_10(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(10, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_12(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_11(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(11, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_13(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_12(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(12, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_14(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_13(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(13, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_15(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_14(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(14, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_16(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_15(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(15, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_17(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_16(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(16, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_18(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_17(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(17, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_19(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_18(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(18, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_20(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_19(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(19, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_21(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_20(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(20, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_22(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_21(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(21, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_23(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_22(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(22, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_24(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_23(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(23, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_25(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_24(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(24, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_26(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_25(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(25, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_27(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_26(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(26, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_28(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_27(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(27, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_29(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_28(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(28, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_30(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_29(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(29, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_31(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_30(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(30, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_32(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_31(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(31, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_33(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_32(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(32, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_34(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_33(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(33, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_35(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_34(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(34, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_36(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_35(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(35, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_37(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_36(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(36, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_38(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_37(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(37, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_39(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_38(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(38, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_40(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_39(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(39, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_41(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_40(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(40, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_42(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_41(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(41, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_43(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_42(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(42, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_44(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_43(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(43, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_45(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_44(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(44, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_46(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_45(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(45, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_47(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_46(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(46, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_48(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_47(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(47, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_49(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_48(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(48, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_50(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_49(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(49, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_51(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_50(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(50, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_52(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_51(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(51, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_53(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_52(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(52, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_54(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_53(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(53, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_55(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_54(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(54, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_56(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_55(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(55, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_57(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_56(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(56, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_58(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_57(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(57, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_59(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_58(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(58, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_60(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_59(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(59, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_61(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_60(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(60, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_62(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_61(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(61, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_63(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_62(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(62, x, fixed_arg0, fixed_arg1)

#define Z_FOR_LOOP_64(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_63(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(63, x, fixed_arg0, fixed_arg1)

#define Z_FOR_EACH_ENGINE(x, sep, fixed_arg0, fixed_arg1, ...) \
	Z_FOR_LOOP_GET_ARG(__VA_ARGS__, \
		Z_FOR_LOOP_64, \
		Z_FOR_LOOP_63, \
		Z_FOR_LOOP_62, \
		Z_FOR_LOOP_61, \
		Z_FOR_LOOP_60, \
		Z_FOR_LOOP_59, \
		Z_FOR_LOOP_58, \
		Z_FOR_LOOP_57, \
		Z_FOR_LOOP_56, \
		Z_FOR_LOOP_55, \
		Z_FOR_LOOP_54, \
		Z_FOR_LOOP_53, \
		Z_FOR_LOOP_52, \
		Z_FOR_LOOP_51, \
		Z_FOR_LOOP_50, \
		Z_FOR_LOOP_49, \
		Z_FOR_LOOP_48, \
		Z_FOR_LOOP_47, \
		Z_FOR_LOOP_46, \
		Z_FOR_LOOP_45, \
		Z_FOR_LOOP_44, \
		Z_FOR_LOOP_43, \
		Z_FOR_LOOP_42, \
		Z_FOR_LOOP_41, \
		Z_FOR_LOOP_40, \
		Z_FOR_LOOP_39, \
		Z_FOR_LOOP_38, \
		Z_FOR_LOOP_37, \
		Z_FOR_LOOP_36, \
		Z_FOR_LOOP_35, \
		Z_FOR_LOOP_34, \
		Z_FOR_LOOP_33, \
		Z_FOR_LOOP_32, \
		Z_FOR_LOOP_31, \
		Z_FOR_LOOP_30, \
		Z_FOR_LOOP_29, \
		Z_FOR_LOOP_28, \
		Z_FOR_LOOP_27, \
		Z_FOR_LOOP_26, \
		Z_FOR_LOOP_25, \
		Z_FOR_LOOP_24, \
		Z_FOR_LOOP_23, \
		Z_FOR_LOOP_22, \
		Z_FOR_LOOP_21, \
		Z_FOR_LOOP_20, \
		Z_FOR_LOOP_19, \
		Z_FOR_LOOP_18, \
		Z_FOR_LOOP_17, \
		Z_FOR_LOOP_16, \
		Z_FOR_LOOP_15, \
		Z_FOR_LOOP_14, \
		Z_FOR_LOOP_13, \
		Z_FOR_LOOP_12, \
		Z_FOR_LOOP_11, \
		Z_FOR_LOOP_10, \
		Z_FOR_LOOP_9, \
		Z_FOR_LOOP_8, \
		Z_FOR_LOOP_7, \
		Z_FOR_LOOP_6, \
		Z_FOR_LOOP_5, \
		Z_FOR_LOOP_4, \
		Z_FOR_LOOP_3, \
		Z_FOR_LOOP_2, \
		Z_FOR_LOOP_1, \
		Z_FOR_LOOP_0)(x, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__)

#define Z_GET_ARG_1(_0, ...) _0

#define Z_GET_ARG_2(_0, _1, ...) _1

#define Z_GET_ARG_3(_0, _1, _2, ...) _2

#define Z_GET_ARG_4(_0, _1, _2, _3, ...) _3

#define Z_GET_ARG_5(_0, _1, _2, _3, _4, ...) _4

#define Z_GET_ARG_6(_0, _1, _2, _3, _4, _5, ...) _5

#define Z_GET_ARG_7(_0, _1, _2, _3, _4, _5, _6, ...) _6

#define Z_GET_ARG_8(_0, _1, _2, _3, _4, _5, _6, _7, ...) _7

#define Z_GET_ARG_9(_0, _1, _2, _3, _4, _5, _6, _7, _8, ...) _8

#define Z_GET_ARG_10(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, ...) _9

#define Z_GET_ARG_11(_0, _1, _2, _3, _4, _5, \
			  _6, _7, _8, _9, _10, ...) _10

#define Z_GET_ARG_12(_0, _1, _2, _3, _4, _5, _6,\
			  _7, _8, _9, _10, _11, ...) _11

#define Z_GET_ARG_13(_0, _1, _2, _3, _4, _5, _6, \
			  _7, _8, _9, _10, _11, _12, ...) _12

#define Z_GET_ARG_14(_0, _1, _2, _3, _4, _5, _6, \
			  _7, _8, _9, _10, _11, _12, _13, ...) _13

#define Z_GET_ARG_15(_0, _1, _2, _3, _4, _5, _6, _7, \
			  _8, _9, _10, _11, _12, _13, _14, ...) _14

#define Z_GET_ARG_16(_0, _1, _2, _3, _4, _5, _6, _7, \
			  _8, _9, _10, _11, _12, _13, _14, _15, ...) _15

#define Z_GET_ARG_17(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, ...) _16

#define Z_GET_ARG_18(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, ...) _17

#define Z_GET_ARG_19(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, ...) _18

#define Z_GET_ARG_20(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  ...) _19

#define Z_GET_ARG_21(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, ...) _20

#define Z_GET_ARG_22(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, ...) _21

#define Z_GET_ARG_23(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, ...) _22

#define Z_GET_ARG_24(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, ...) _23

#define Z_GET_ARG_25(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, ...) _24

#define Z_GET_ARG_26(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, ...) _25

#define Z_GET_ARG_27(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, ...) _26

#define Z_GET_ARG_28(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, ...) _27

#define Z_GET_ARG_29(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  ...) _28

#define Z_GET_ARG_30(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, ...) _29

#define Z_GET_ARG_31(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, ...) _30

#define Z_GET_ARG_32(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, ...) _31

#define Z_GET_ARG_33(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, ...) _32

#define Z_GET_ARG_34(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, ...) _33

#define Z_GET_ARG_35(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, ...) _34

#define Z_GET_ARG_36(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, ...) _35

#define Z_GET_ARG_37(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, ...) _36

#define Z_GET_ARG_38(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, ...) _37

#define Z_GET_ARG_39(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, ...) _38

#define Z_GET_ARG_40(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, ...) _39

#define Z_GET_ARG_41(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, ...) _40

#define Z_GET_ARG_42(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, ...) _41

#define Z_GET_ARG_43(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, ...) _42

#define Z_GET_ARG_44(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, ...) _43

#define Z_GET_ARG_45(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, ...) _44

#define Z_GET_ARG_46(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, ...) _45

#define Z_GET_ARG_47(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, ...) _46

#define Z_GET_ARG_48(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, \
			  _47, ...) _47

#define Z_GET_ARG_49(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, \
			  _47, _48, ...) _48

#define Z_GET_ARG_50(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, \
			  _47, _48, _49, ...) _49

#define Z_GET_ARG_51(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, \
			  _47, _48, _49, _50, ...) _50

#define Z_GET_ARG_52(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, \
			  _47, _48, _49, _50, _51, ...) _51

#define Z_GET_ARG_53(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, \
			  _47, _48, _49, _50, _51, _52, ...) _52

#define Z_GET_ARG_54(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, \
			  _47, _48, _49, _50, _51, _52, _53, ...) _53

#define Z_GET_ARG_55(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, \
			  _47, _48, _49, _50, _51, _52, _53, _54, ...) _54

#define Z_GET_ARG_56(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, \
			  _47, _48, _49, _50, _51, _52, _53, _54, _55, ...) _55

#define Z_GET_ARG_57(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, \
			  _47, _48, _49, _50, _51, _52, _53, _54, _55, \
			  _56, ...) _56

#define Z_GET_ARG_58(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, \
			  _47, _48, _49, _50, _51, _52, _53, _54, _55, \
			  _56, _57, ...) _57

#define Z_GET_ARG_59(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, \
			  _47, _48, _49, _50, _51, _52, _53, _54, _55, \
			  _56, _57, _58, ...) _58

#define Z_GET_ARG_60(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, \
			  _47, _48, _49, _50, _51, _52, _53, _54, _55, \
			  _56, _57, _58, _59, ...) _59

#define Z_GET_ARG_61(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, \
			  _47, _48, _49, _50, _51, _52, _53, _54, _55, \
			  _56, _57, _58, _59, _60, ...) _60

#define Z_GET_ARG_62(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, \
			  _47, _48, _49, _50, _51, _52, _53, _54, _55, \
			  _56, _57, _58, _59, _60, _61, ...) _61

#define Z_GET_ARG_63(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, \
			  _47, _48, _49, _50, _51, _52, _53, _54, _55, \
			  _56, _57, _58, _59, _60, _61, _62, ...) _62

#define Z_GET_ARG_64(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, \
			  _20, _21, _22, _23, _24, _25, _26, _27, _28, \
			  _29, _30, _31, _32, _33, _34, _35, _36, _37, \
			  _38, _39, _40, _41, _42, _43, _44, _45, _46, \
			  _47, _48, _49, _50, _51, _52, _53, _54, _55, \
			  _56, _57, _58, _59, _60, _61, _62, _63, ...) _63

#define Z_GET_ARGS_LESS_0(...) __VA_ARGS__

#define Z_GET_ARGS_LESS_1(_0, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_2(_0, _1, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_3(_0, _1, _2, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_4(_0, _1, _2, _3, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_5(_0, _1, _2, _3, _4, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_6(_0, _1, _2, _3, _4, _5, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_7(_0, _1, _2, _3, _4, _5, _6, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_8(_0, _1, _2, _3, _4, _5, \
				_6, _7, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_9(_0, _1, _2, _3, _4, _5, \
				_6, _7, _8, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_10(_0, _1, _2, _3, _4, _5, \
				_6, _7, _8, _9, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_11(_0, _1, _2, _3, _4, _5, \
				_6, _7, _8, _9, _10, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_12(_0, _1, _2, _3, _4, _5, _6,\
				_7, _8, _9, _10, _11, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_13(_0, _1, _2, _3, _4, _5, _6, \
				_7, _8, _9, _10, _11, _12, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_14(_0, _1, _2, _3, _4, _5, _6, \
				_7, _8, _9, _10, _11, _12, _13, \
				...) __VA_ARGS__

#define Z_GET_ARGS_LESS_15(_0, _1, _2, _3, _4, _5, _6, _7, \
				_8, _9, _10, _11, _12, _13, _14, \
				...) __VA_ARGS__

#define Z_GET_ARGS_LESS_16(_0, _1, _2, _3, _4, _5, _6, _7, \
				_8, _9, _10, _11, _12, _13, _14, _15, ...) \
				__VA_ARGS__

#define Z_GET_ARGS_LESS_17(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_18(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, ...) \
				__VA_ARGS__

#define Z_GET_ARGS_LESS_19(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, ...) \
				__VA_ARGS__

#define Z_GET_ARGS_LESS_20(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				...) __VA_ARGS__

#define Z_GET_ARGS_LESS_21(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_22(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_23(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_24(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_25(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_26(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_27(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, ...) \
				__VA_ARGS__

#define Z_GET_ARGS_LESS_28(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, \
				...) __VA_ARGS__

#define Z_GET_ARGS_LESS_29(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				...) __VA_ARGS__

#define Z_GET_ARGS_LESS_30(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_31(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_32(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_33(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_34(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_35(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_36(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_37(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_38(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_39(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_40(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_41(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_42(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_43(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_44(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_45(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_46(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_47(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_48(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, \
				_47, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_49(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, \
				_47, _48, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_50(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, \
				_47, _48, _49, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_51(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, \
				_47, _48, _49, _50, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_52(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, \
				_47, _48, _49, _50, _51, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_53(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, \
				_47, _48, _49, _50, _51, _52, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_54(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, \
				_47, _48, _49, _50, _51, _52, _53, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_55(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, \
				_47, _48, _49, _50, _51, _52, _53, _54, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_56(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, \
				_47, _48, _49, _50, _51, _52, _53, _54, _55, \
				...) __VA_ARGS__

#define Z_GET_ARGS_LESS_57(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, \
				_47, _48, _49, _50, _51, _52, _53, _54, _55, \
				_56, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_58(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, \
				_47, _48, _49, _50, _51, _52, _53, _54, _55, \
				_56, _57, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_59(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, \
				_47, _48, _49, _50, _51, _52, _53, _54, _55, \
				_56, _57, _58, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_60(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, \
				_47, _48, _49, _50, _51, _52, _53, _54, _55, \
				_56, _57, _58, _59, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_61(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, \
				_47, _48, _49, _50, _51, _52, _53, _54, _55, \
				_56, _57, _58, _59, _60, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_62(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, \
				_47, _48, _49, _50, _51, _52, _53, _54, _55, \
				_56, _57, _58, _59, _60, _61, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_63(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, \
				_47, _48, _49, _50, _51, _52, _53, _54, _55, \
				_56, _57, _58, _59, _60, _61, _62, ...) __VA_ARGS__

#define Z_GET_ARGS_LESS_64(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
				_11, _12, _13, _14, _15, _16, _17, _18, _19, \
				_20, _21, _22, _23, _24, _25, _26, _27, _28, \
				_29, _30, _31, _32, _33, _34, _35, _36, _37, \
				_38, _39, _40, _41, _42, _43, _44, _45, _46, \
				_47, _48, _49, _50, _51, _52, _53, _54, _55, \
				_56, _57, _58, _59, _60, _61, _62, _63, ...) __VA_ARGS__

#define Z_FOR_EACH_IDX_FIXED_ARG_EXEC(idx, x, fixed_arg0, fixed_arg1) \
	fixed_arg0(idx, x, fixed_arg1)

#define Z_FOR_EACH_IDX_FIXED_ARG(F, sep, fixed_arg, ...) \
	Z_FOR_EACH_ENGINE(Z_FOR_EACH_IDX_FIXED_ARG_EXEC, sep, \
			     F, fixed_arg, __VA_ARGS__)

#define Z_FOR_EACH_FIXED_ARG_EXEC(idx, x, fixed_arg0, fixed_arg1) \
	fixed_arg0(x, fixed_arg1)

#define Z_FOR_EACH_FIXED_ARG(F, sep, fixed_arg, ...) \
	Z_FOR_EACH_ENGINE(Z_FOR_EACH_FIXED_ARG_EXEC, sep, \
			     F, fixed_arg, __VA_ARGS__)

#define Z_FOR_EACH_IDX_EXEC(idx, x, fixed_arg0, fixed_arg1) \
	fixed_arg0(idx, x)

#define Z_FOR_EACH_IDX(F, sep, ...) \
	Z_FOR_EACH_ENGINE(Z_FOR_EACH_IDX_EXEC, sep, F, _, __VA_ARGS__)

#define Z_FOR_EACH_EXEC(idx, x, fixed_arg0, fixed_arg1) \
	fixed_arg0(x)

#define Z_FOR_EACH(F, sep, ...) \
	Z_FOR_EACH_ENGINE(Z_FOR_EACH_EXEC, sep, F, _, __VA_ARGS__)

#define Z_BYPASS(x) x

/* Set of UTIL_LISTIFY particles */
#define Z_UTIL_LISTIFY_0(F, sep, ...)

#define Z_UTIL_LISTIFY_1(F, sep, ...) \
	F(0, __VA_ARGS__)

#define Z_UTIL_LISTIFY_2(F, sep, ...) \
	Z_UTIL_LISTIFY_1(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(1, __VA_ARGS__)

#define Z_UTIL_LISTIFY_3(F, sep, ...) \
	Z_UTIL_LISTIFY_2(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(2, __VA_ARGS__)

#define Z_UTIL_LISTIFY_4(F, sep, ...) \
	Z_UTIL_LISTIFY_3(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(3, __VA_ARGS__)

#define Z_UTIL_LISTIFY_5(F, sep, ...) \
	Z_UTIL_LISTIFY_4(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(4, __VA_ARGS__)

#define Z_UTIL_LISTIFY_6(F, sep, ...) \
	Z_UTIL_LISTIFY_5(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(5, __VA_ARGS__)

#define Z_UTIL_LISTIFY_7(F, sep, ...) \
	Z_UTIL_LISTIFY_6(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(6, __VA_ARGS__)

#define Z_UTIL_LISTIFY_8(F, sep, ...) \
	Z_UTIL_LISTIFY_7(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(7, __VA_ARGS__)

#define Z_UTIL_LISTIFY_9(F, sep, ...) \
	Z_UTIL_LISTIFY_8(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(8, __VA_ARGS__)

#define Z_UTIL_LISTIFY_10(F, sep, ...) \
	Z_UTIL_LISTIFY_9(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(9, __VA_ARGS__)

#define Z_UTIL_LISTIFY_11(F, sep, ...) \
	Z_UTIL_LISTIFY_10(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(10, __VA_ARGS__)

#define Z_UTIL_LISTIFY_12(F, sep, ...) \
	Z_UTIL_LISTIFY_11(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(11, __VA_ARGS__)

#define Z_UTIL_LISTIFY_13(F, sep, ...) \
	Z_UTIL_LISTIFY_12(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(12, __VA_ARGS__)

#define Z_UTIL_LISTIFY_14(F, sep, ...) \
	Z_UTIL_LISTIFY_13(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(13, __VA_ARGS__)

#define Z_UTIL_LISTIFY_15(F, sep, ...) \
	Z_UTIL_LISTIFY_14(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(14, __VA_ARGS__)

#define Z_UTIL_LISTIFY_16(F, sep, ...) \
	Z_UTIL_LISTIFY_15(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(15, __VA_ARGS__)

#define Z_UTIL_LISTIFY_17(F, sep, ...) \
	Z_UTIL_LISTIFY_16(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(16, __VA_ARGS__)

#define Z_UTIL_LISTIFY_18(F, sep, ...) \
	Z_UTIL_LISTIFY_17(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(17, __VA_ARGS__)

#define Z_UTIL_LISTIFY_19(F, sep, ...) \
	Z_UTIL_LISTIFY_18(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(18, __VA_ARGS__)

#define Z_UTIL_LISTIFY_20(F, sep, ...) \
	Z_UTIL_LISTIFY_19(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(19, __VA_ARGS__)

#define Z_UTIL_LISTIFY_21(F, sep, ...) \
	Z_UTIL_LISTIFY_20(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(20, __VA_ARGS__)

#define Z_UTIL_LISTIFY_22(F, sep, ...) \
	Z_UTIL_LISTIFY_21(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(21, __VA_ARGS__)

#define Z_UTIL_LISTIFY_23(F, sep, ...) \
	Z_UTIL_LISTIFY_22(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(22, __VA_ARGS__)

#define Z_UTIL_LISTIFY_24(F, sep, ...) \
	Z_UTIL_LISTIFY_23(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(23, __VA_ARGS__)

#define Z_UTIL_LISTIFY_25(F, sep, ...) \
	Z_UTIL_LISTIFY_24(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(24, __VA_ARGS__)

#define Z_UTIL_LISTIFY_26(F, sep, ...) \
	Z_UTIL_LISTIFY_25(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(25, __VA_ARGS__)

#define Z_UTIL_LISTIFY_27(F, sep, ...) \
	Z_UTIL_LISTIFY_26(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(26, __VA_ARGS__)

#define Z_UTIL_LISTIFY_28(F, sep, ...) \
	Z_UTIL_LISTIFY_27(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(27, __VA_ARGS__)

#define Z_UTIL_LISTIFY_29(F, sep, ...) \
	Z_UTIL_LISTIFY_28(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(28, __VA_ARGS__)

#define Z_UTIL_LISTIFY_30(F, sep, ...) \
	Z_UTIL_LISTIFY_29(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(29, __VA_ARGS__)

#define Z_UTIL_LISTIFY_31(F, sep, ...) \
	Z_UTIL_LISTIFY_30(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(30, __VA_ARGS__)

#define Z_UTIL_LISTIFY_32(F, sep, ...) \
	Z_UTIL_LISTIFY_31(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(31, __VA_ARGS__)

#define Z_UTIL_LISTIFY_33(F, sep, ...) \
	Z_UTIL_LISTIFY_32(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(32, __VA_ARGS__)

#define Z_UTIL_LISTIFY_34(F, sep, ...) \
	Z_UTIL_LISTIFY_33(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(33, __VA_ARGS__)

#define Z_UTIL_LISTIFY_35(F, sep, ...) \
	Z_UTIL_LISTIFY_34(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(34, __VA_ARGS__)

#define Z_UTIL_LISTIFY_36(F, sep, ...) \
	Z_UTIL_LISTIFY_35(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(35, __VA_ARGS__)

#define Z_UTIL_LISTIFY_37(F, sep, ...) \
	Z_UTIL_LISTIFY_36(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(36, __VA_ARGS__)

#define Z_UTIL_LISTIFY_38(F, sep, ...) \
	Z_UTIL_LISTIFY_37(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(37, __VA_ARGS__)

#define Z_UTIL_LISTIFY_39(F, sep, ...) \
	Z_UTIL_LISTIFY_38(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(38, __VA_ARGS__)

#define Z_UTIL_LISTIFY_40(F, sep, ...) \
	Z_UTIL_LISTIFY_39(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(39, __VA_ARGS__)

#define Z_UTIL_LISTIFY_41(F, sep, ...) \
	Z_UTIL_LISTIFY_40(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(40, __VA_ARGS__)

#define Z_UTIL_LISTIFY_42(F, sep, ...) \
	Z_UTIL_LISTIFY_41(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(41, __VA_ARGS__)

#define Z_UTIL_LISTIFY_43(F, sep, ...) \
	Z_UTIL_LISTIFY_42(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(42, __VA_ARGS__)

#define Z_UTIL_LISTIFY_44(F, sep, ...) \
	Z_UTIL_LISTIFY_43(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(43, __VA_ARGS__)

#define Z_UTIL_LISTIFY_45(F, sep, ...) \
	Z_UTIL_LISTIFY_44(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(44, __VA_ARGS__)

#define Z_UTIL_LISTIFY_46(F, sep, ...) \
	Z_UTIL_LISTIFY_45(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(45, __VA_ARGS__)

#define Z_UTIL_LISTIFY_47(F, sep, ...) \
	Z_UTIL_LISTIFY_46(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(46, __VA_ARGS__)

#define Z_UTIL_LISTIFY_48(F, sep, ...) \
	Z_UTIL_LISTIFY_47(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(47, __VA_ARGS__)

#define Z_UTIL_LISTIFY_49(F, sep, ...) \
	Z_UTIL_LISTIFY_48(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(48, __VA_ARGS__)

#define Z_UTIL_LISTIFY_50(F, sep, ...) \
	Z_UTIL_LISTIFY_49(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(49, __VA_ARGS__)

#define Z_UTIL_LISTIFY_51(F, sep, ...) \
	Z_UTIL_LISTIFY_50(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(50, __VA_ARGS__)

#define Z_UTIL_LISTIFY_52(F, sep, ...) \
	Z_UTIL_LISTIFY_51(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(51, __VA_ARGS__)

#define Z_UTIL_LISTIFY_53(F, sep, ...) \
	Z_UTIL_LISTIFY_52(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(52, __VA_ARGS__)

#define Z_UTIL_LISTIFY_54(F, sep, ...) \
	Z_UTIL_LISTIFY_53(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(53, __VA_ARGS__)

#define Z_UTIL_LISTIFY_55(F, sep, ...) \
	Z_UTIL_LISTIFY_54(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(54, __VA_ARGS__)

#define Z_UTIL_LISTIFY_56(F, sep, ...) \
	Z_UTIL_LISTIFY_55(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(55, __VA_ARGS__)

#define Z_UTIL_LISTIFY_57(F, sep, ...) \
	Z_UTIL_LISTIFY_56(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(56, __VA_ARGS__)

#define Z_UTIL_LISTIFY_58(F, sep, ...) \
	Z_UTIL_LISTIFY_57(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(57, __VA_ARGS__)

#define Z_UTIL_LISTIFY_59(F, sep, ...) \
	Z_UTIL_LISTIFY_58(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(58, __VA_ARGS__)

#define Z_UTIL_LISTIFY_60(F, sep, ...) \
	Z_UTIL_LISTIFY_59(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(59, __VA_ARGS__)

#define Z_UTIL_LISTIFY_61(F, sep, ...) \
	Z_UTIL_LISTIFY_60(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(60, __VA_ARGS__)

#define Z_UTIL_LISTIFY_62(F, sep, ...) \
	Z_UTIL_LISTIFY_61(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(61, __VA_ARGS__)

#define Z_UTIL_LISTIFY_63(F, sep, ...) \
	Z_UTIL_LISTIFY_62(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(62, __VA_ARGS__)

#define Z_UTIL_LISTIFY_64(F, sep, ...) \
	Z_UTIL_LISTIFY_63(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(63, __VA_ARGS__)

#define Z_UTIL_LISTIFY_65(F, sep, ...) \
	Z_UTIL_LISTIFY_64(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(64, __VA_ARGS__)

#define Z_UTIL_LISTIFY_66(F, sep, ...) \
	Z_UTIL_LISTIFY_65(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(65, __VA_ARGS__)

#define Z_UTIL_LISTIFY_67(F, sep, ...) \
	Z_UTIL_LISTIFY_66(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(66, __VA_ARGS__)

#define Z_UTIL_LISTIFY_68(F, sep, ...) \
	Z_UTIL_LISTIFY_67(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(67, __VA_ARGS__)

#define Z_UTIL_LISTIFY_69(F, sep, ...) \
	Z_UTIL_LISTIFY_68(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(68, __VA_ARGS__)

#define Z_UTIL_LISTIFY_70(F, sep, ...) \
	Z_UTIL_LISTIFY_69(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(69, __VA_ARGS__)

#define Z_UTIL_LISTIFY_71(F, sep, ...) \
	Z_UTIL_LISTIFY_70(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(70, __VA_ARGS__)

#define Z_UTIL_LISTIFY_72(F, sep, ...) \
	Z_UTIL_LISTIFY_71(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(71, __VA_ARGS__)

#define Z_UTIL_LISTIFY_73(F, sep, ...) \
	Z_UTIL_LISTIFY_72(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(72, __VA_ARGS__)

#define Z_UTIL_LISTIFY_74(F, sep, ...) \
	Z_UTIL_LISTIFY_73(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(73, __VA_ARGS__)

#define Z_UTIL_LISTIFY_75(F, sep, ...) \
	Z_UTIL_LISTIFY_74(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(74, __VA_ARGS__)

#define Z_UTIL_LISTIFY_76(F, sep, ...) \
	Z_UTIL_LISTIFY_75(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(75, __VA_ARGS__)

#define Z_UTIL_LISTIFY_77(F, sep, ...) \
	Z_UTIL_LISTIFY_76(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(76, __VA_ARGS__)

#define Z_UTIL_LISTIFY_78(F, sep, ...) \
	Z_UTIL_LISTIFY_77(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(77, __VA_ARGS__)

#define Z_UTIL_LISTIFY_79(F, sep, ...) \
	Z_UTIL_LISTIFY_78(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(78, __VA_ARGS__)

#define Z_UTIL_LISTIFY_80(F, sep, ...) \
	Z_UTIL_LISTIFY_79(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(79, __VA_ARGS__)

#define Z_UTIL_LISTIFY_81(F, sep, ...) \
	Z_UTIL_LISTIFY_80(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(80, __VA_ARGS__)

#define Z_UTIL_LISTIFY_82(F, sep, ...) \
	Z_UTIL_LISTIFY_81(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(81, __VA_ARGS__)

#define Z_UTIL_LISTIFY_83(F, sep, ...) \
	Z_UTIL_LISTIFY_82(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(82, __VA_ARGS__)

#define Z_UTIL_LISTIFY_84(F, sep, ...) \
	Z_UTIL_LISTIFY_83(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(83, __VA_ARGS__)

#define Z_UTIL_LISTIFY_85(F, sep, ...) \
	Z_UTIL_LISTIFY_84(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(84, __VA_ARGS__)

#define Z_UTIL_LISTIFY_86(F, sep, ...) \
	Z_UTIL_LISTIFY_85(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(85, __VA_ARGS__)

#define Z_UTIL_LISTIFY_87(F, sep, ...) \
	Z_UTIL_LISTIFY_86(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(86, __VA_ARGS__)

#define Z_UTIL_LISTIFY_88(F, sep, ...) \
	Z_UTIL_LISTIFY_87(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(87, __VA_ARGS__)

#define Z_UTIL_LISTIFY_89(F, sep, ...) \
	Z_UTIL_LISTIFY_88(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(88, __VA_ARGS__)

#define Z_UTIL_LISTIFY_90(F, sep, ...) \
	Z_UTIL_LISTIFY_89(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(89, __VA_ARGS__)

#define Z_UTIL_LISTIFY_91(F, sep, ...) \
	Z_UTIL_LISTIFY_90(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(90, __VA_ARGS__)

#define Z_UTIL_LISTIFY_92(F, sep, ...) \
	Z_UTIL_LISTIFY_91(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(91, __VA_ARGS__)

#define Z_UTIL_LISTIFY_93(F, sep, ...) \
	Z_UTIL_LISTIFY_92(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(92, __VA_ARGS__)

#define Z_UTIL_LISTIFY_94(F, sep, ...) \
	Z_UTIL_LISTIFY_93(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(93, __VA_ARGS__)

#define Z_UTIL_LISTIFY_95(F, sep, ...) \
	Z_UTIL_LISTIFY_94(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(94, __VA_ARGS__)

#define Z_UTIL_LISTIFY_96(F, sep, ...) \
	Z_UTIL_LISTIFY_95(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(95, __VA_ARGS__)

#define Z_UTIL_LISTIFY_97(F, sep, ...) \
	Z_UTIL_LISTIFY_96(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(96, __VA_ARGS__)

#define Z_UTIL_LISTIFY_98(F, sep, ...) \
	Z_UTIL_LISTIFY_97(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(97, __VA_ARGS__)

#define Z_UTIL_LISTIFY_99(F, sep, ...) \
	Z_UTIL_LISTIFY_98(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(98, __VA_ARGS__)

#define Z_UTIL_LISTIFY_100(F, sep, ...) \
	Z_UTIL_LISTIFY_99(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(99, __VA_ARGS__)

#define Z_UTIL_LISTIFY_101(F, sep, ...) \
	Z_UTIL_LISTIFY_100(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(100, __VA_ARGS__)

#define Z_UTIL_LISTIFY_102(F, sep, ...) \
	Z_UTIL_LISTIFY_101(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(101, __VA_ARGS__)

#define Z_UTIL_LISTIFY_103(F, sep, ...) \
	Z_UTIL_LISTIFY_102(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(102, __VA_ARGS__)

#define Z_UTIL_LISTIFY_104(F, sep, ...) \
	Z_UTIL_LISTIFY_103(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(103, __VA_ARGS__)

#define Z_UTIL_LISTIFY_105(F, sep, ...) \
	Z_UTIL_LISTIFY_104(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(104, __VA_ARGS__)

#define Z_UTIL_LISTIFY_106(F, sep, ...) \
	Z_UTIL_LISTIFY_105(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(105, __VA_ARGS__)

#define Z_UTIL_LISTIFY_107(F, sep, ...) \
	Z_UTIL_LISTIFY_106(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(106, __VA_ARGS__)

#define Z_UTIL_LISTIFY_108(F, sep, ...) \
	Z_UTIL_LISTIFY_107(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(107, __VA_ARGS__)

#define Z_UTIL_LISTIFY_109(F, sep, ...) \
	Z_UTIL_LISTIFY_108(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(108, __VA_ARGS__)

#define Z_UTIL_LISTIFY_110(F, sep, ...) \
	Z_UTIL_LISTIFY_109(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(109, __VA_ARGS__)

#define Z_UTIL_LISTIFY_111(F, sep, ...) \
	Z_UTIL_LISTIFY_110(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(110, __VA_ARGS__)

#define Z_UTIL_LISTIFY_112(F, sep, ...) \
	Z_UTIL_LISTIFY_111(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(111, __VA_ARGS__)

#define Z_UTIL_LISTIFY_113(F, sep, ...) \
	Z_UTIL_LISTIFY_112(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(112, __VA_ARGS__)

#define Z_UTIL_LISTIFY_114(F, sep, ...) \
	Z_UTIL_LISTIFY_113(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(113, __VA_ARGS__)

#define Z_UTIL_LISTIFY_115(F, sep, ...) \
	Z_UTIL_LISTIFY_114(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(114, __VA_ARGS__)

#define Z_UTIL_LISTIFY_116(F, sep, ...) \
	Z_UTIL_LISTIFY_115(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(115, __VA_ARGS__)

#define Z_UTIL_LISTIFY_117(F, sep, ...) \
	Z_UTIL_LISTIFY_116(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(116, __VA_ARGS__)

#define Z_UTIL_LISTIFY_118(F, sep, ...) \
	Z_UTIL_LISTIFY_117(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(117, __VA_ARGS__)

#define Z_UTIL_LISTIFY_119(F, sep, ...) \
	Z_UTIL_LISTIFY_118(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(118, __VA_ARGS__)

#define Z_UTIL_LISTIFY_120(F, sep, ...) \
	Z_UTIL_LISTIFY_119(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(119, __VA_ARGS__)

#define Z_UTIL_LISTIFY_121(F, sep, ...) \
	Z_UTIL_LISTIFY_120(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(120, __VA_ARGS__)

#define Z_UTIL_LISTIFY_122(F, sep, ...) \
	Z_UTIL_LISTIFY_121(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(121, __VA_ARGS__)

#define Z_UTIL_LISTIFY_123(F, sep, ...) \
	Z_UTIL_LISTIFY_122(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(122, __VA_ARGS__)

#define Z_UTIL_LISTIFY_124(F, sep, ...) \
	Z_UTIL_LISTIFY_123(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(123, __VA_ARGS__)

#define Z_UTIL_LISTIFY_125(F, sep, ...) \
	Z_UTIL_LISTIFY_124(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(124, __VA_ARGS__)

#define Z_UTIL_LISTIFY_126(F, sep, ...) \
	Z_UTIL_LISTIFY_125(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(125, __VA_ARGS__)

#define Z_UTIL_LISTIFY_127(F, sep, ...) \
	Z_UTIL_LISTIFY_126(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(126, __VA_ARGS__)

#define Z_UTIL_LISTIFY_128(F, sep, ...) \
	Z_UTIL_LISTIFY_127(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(127, __VA_ARGS__)

#define Z_UTIL_LISTIFY_129(F, sep, ...) \
	Z_UTIL_LISTIFY_128(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(128, __VA_ARGS__)

#define Z_UTIL_LISTIFY_130(F, sep, ...) \
	Z_UTIL_LISTIFY_129(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(129, __VA_ARGS__)

#define Z_UTIL_LISTIFY_131(F, sep, ...) \
	Z_UTIL_LISTIFY_130(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(130, __VA_ARGS__)

#define Z_UTIL_LISTIFY_132(F, sep, ...) \
	Z_UTIL_LISTIFY_131(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(131, __VA_ARGS__)

#define Z_UTIL_LISTIFY_133(F, sep, ...) \
	Z_UTIL_LISTIFY_132(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(132, __VA_ARGS__)

#define Z_UTIL_LISTIFY_134(F, sep, ...) \
	Z_UTIL_LISTIFY_133(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(133, __VA_ARGS__)

#define Z_UTIL_LISTIFY_135(F, sep, ...) \
	Z_UTIL_LISTIFY_134(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(134, __VA_ARGS__)

#define Z_UTIL_LISTIFY_136(F, sep, ...) \
	Z_UTIL_LISTIFY_135(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(135, __VA_ARGS__)

#define Z_UTIL_LISTIFY_137(F, sep, ...) \
	Z_UTIL_LISTIFY_136(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(136, __VA_ARGS__)

#define Z_UTIL_LISTIFY_138(F, sep, ...) \
	Z_UTIL_LISTIFY_137(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(137, __VA_ARGS__)

#define Z_UTIL_LISTIFY_139(F, sep, ...) \
	Z_UTIL_LISTIFY_138(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(138, __VA_ARGS__)

#define Z_UTIL_LISTIFY_140(F, sep, ...) \
	Z_UTIL_LISTIFY_139(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(139, __VA_ARGS__)

#define Z_UTIL_LISTIFY_141(F, sep, ...) \
	Z_UTIL_LISTIFY_140(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(140, __VA_ARGS__)

#define Z_UTIL_LISTIFY_142(F, sep, ...) \
	Z_UTIL_LISTIFY_141(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(141, __VA_ARGS__)

#define Z_UTIL_LISTIFY_143(F, sep, ...) \
	Z_UTIL_LISTIFY_142(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(142, __VA_ARGS__)

#define Z_UTIL_LISTIFY_144(F, sep, ...) \
	Z_UTIL_LISTIFY_143(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(143, __VA_ARGS__)

#define Z_UTIL_LISTIFY_145(F, sep, ...) \
	Z_UTIL_LISTIFY_144(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(144, __VA_ARGS__)

#define Z_UTIL_LISTIFY_146(F, sep, ...) \
	Z_UTIL_LISTIFY_145(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(145, __VA_ARGS__)

#define Z_UTIL_LISTIFY_147(F, sep, ...) \
	Z_UTIL_LISTIFY_146(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(146, __VA_ARGS__)

#define Z_UTIL_LISTIFY_148(F, sep, ...) \
	Z_UTIL_LISTIFY_147(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(147, __VA_ARGS__)

#define Z_UTIL_LISTIFY_149(F, sep, ...) \
	Z_UTIL_LISTIFY_148(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(148, __VA_ARGS__)

#define Z_UTIL_LISTIFY_150(F, sep, ...) \
	Z_UTIL_LISTIFY_149(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(149, __VA_ARGS__)

#define Z_UTIL_LISTIFY_151(F, sep, ...) \
	Z_UTIL_LISTIFY_150(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(150, __VA_ARGS__)

#define Z_UTIL_LISTIFY_152(F, sep, ...) \
	Z_UTIL_LISTIFY_151(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(151, __VA_ARGS__)

#define Z_UTIL_LISTIFY_153(F, sep, ...) \
	Z_UTIL_LISTIFY_152(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(152, __VA_ARGS__)

#define Z_UTIL_LISTIFY_154(F, sep, ...) \
	Z_UTIL_LISTIFY_153(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(153, __VA_ARGS__)

#define Z_UTIL_LISTIFY_155(F, sep, ...) \
	Z_UTIL_LISTIFY_154(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(154, __VA_ARGS__)

#define Z_UTIL_LISTIFY_156(F, sep, ...) \
	Z_UTIL_LISTIFY_155(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(155, __VA_ARGS__)

#define Z_UTIL_LISTIFY_157(F, sep, ...) \
	Z_UTIL_LISTIFY_156(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(156, __VA_ARGS__)

#define Z_UTIL_LISTIFY_158(F, sep, ...) \
	Z_UTIL_LISTIFY_157(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(157, __VA_ARGS__)

#define Z_UTIL_LISTIFY_159(F, sep, ...) \
	Z_UTIL_LISTIFY_158(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(158, __VA_ARGS__)

#define Z_UTIL_LISTIFY_160(F, sep, ...) \
	Z_UTIL_LISTIFY_159(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(159, __VA_ARGS__)

#define Z_UTIL_LISTIFY_161(F, sep, ...) \
	Z_UTIL_LISTIFY_160(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(160, __VA_ARGS__)

#define Z_UTIL_LISTIFY_162(F, sep, ...) \
	Z_UTIL_LISTIFY_161(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(161, __VA_ARGS__)

#define Z_UTIL_LISTIFY_163(F, sep, ...) \
	Z_UTIL_LISTIFY_162(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(162, __VA_ARGS__)

#define Z_UTIL_LISTIFY_164(F, sep, ...) \
	Z_UTIL_LISTIFY_163(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(163, __VA_ARGS__)

#define Z_UTIL_LISTIFY_165(F, sep, ...) \
	Z_UTIL_LISTIFY_164(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(164, __VA_ARGS__)

#define Z_UTIL_LISTIFY_166(F, sep, ...) \
	Z_UTIL_LISTIFY_165(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(165, __VA_ARGS__)

#define Z_UTIL_LISTIFY_167(F, sep, ...) \
	Z_UTIL_LISTIFY_166(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(166, __VA_ARGS__)

#define Z_UTIL_LISTIFY_168(F, sep, ...) \
	Z_UTIL_LISTIFY_167(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(167, __VA_ARGS__)

#define Z_UTIL_LISTIFY_169(F, sep, ...) \
	Z_UTIL_LISTIFY_168(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(168, __VA_ARGS__)

#define Z_UTIL_LISTIFY_170(F, sep, ...) \
	Z_UTIL_LISTIFY_169(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(169, __VA_ARGS__)

#define Z_UTIL_LISTIFY_171(F, sep, ...) \
	Z_UTIL_LISTIFY_170(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(170, __VA_ARGS__)

#define Z_UTIL_LISTIFY_172(F, sep, ...) \
	Z_UTIL_LISTIFY_171(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(171, __VA_ARGS__)

#define Z_UTIL_LISTIFY_173(F, sep, ...) \
	Z_UTIL_LISTIFY_172(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(172, __VA_ARGS__)

#define Z_UTIL_LISTIFY_174(F, sep, ...) \
	Z_UTIL_LISTIFY_173(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(173, __VA_ARGS__)

#define Z_UTIL_LISTIFY_175(F, sep, ...) \
	Z_UTIL_LISTIFY_174(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(174, __VA_ARGS__)

#define Z_UTIL_LISTIFY_176(F, sep, ...) \
	Z_UTIL_LISTIFY_175(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(175, __VA_ARGS__)

#define Z_UTIL_LISTIFY_177(F, sep, ...) \
	Z_UTIL_LISTIFY_176(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(176, __VA_ARGS__)

#define Z_UTIL_LISTIFY_178(F, sep, ...) \
	Z_UTIL_LISTIFY_177(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(177, __VA_ARGS__)

#define Z_UTIL_LISTIFY_179(F, sep, ...) \
	Z_UTIL_LISTIFY_178(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(178, __VA_ARGS__)

#define Z_UTIL_LISTIFY_180(F, sep, ...) \
	Z_UTIL_LISTIFY_179(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(179, __VA_ARGS__)

#define Z_UTIL_LISTIFY_181(F, sep, ...) \
	Z_UTIL_LISTIFY_180(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(180, __VA_ARGS__)

#define Z_UTIL_LISTIFY_182(F, sep, ...) \
	Z_UTIL_LISTIFY_181(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(181, __VA_ARGS__)

#define Z_UTIL_LISTIFY_183(F, sep, ...) \
	Z_UTIL_LISTIFY_182(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(182, __VA_ARGS__)

#define Z_UTIL_LISTIFY_184(F, sep, ...) \
	Z_UTIL_LISTIFY_183(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(183, __VA_ARGS__)

#define Z_UTIL_LISTIFY_185(F, sep, ...) \
	Z_UTIL_LISTIFY_184(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(184, __VA_ARGS__)

#define Z_UTIL_LISTIFY_186(F, sep, ...) \
	Z_UTIL_LISTIFY_185(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(185, __VA_ARGS__)

#define Z_UTIL_LISTIFY_187(F, sep, ...) \
	Z_UTIL_LISTIFY_186(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(186, __VA_ARGS__)

#define Z_UTIL_LISTIFY_188(F, sep, ...) \
	Z_UTIL_LISTIFY_187(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(187, __VA_ARGS__)

#define Z_UTIL_LISTIFY_189(F, sep, ...) \
	Z_UTIL_LISTIFY_188(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(188, __VA_ARGS__)

#define Z_UTIL_LISTIFY_190(F, sep, ...) \
	Z_UTIL_LISTIFY_189(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(189, __VA_ARGS__)

#define Z_UTIL_LISTIFY_191(F, sep, ...) \
	Z_UTIL_LISTIFY_190(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(190, __VA_ARGS__)

#define Z_UTIL_LISTIFY_192(F, sep, ...) \
	Z_UTIL_LISTIFY_191(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(191, __VA_ARGS__)

#define Z_UTIL_LISTIFY_193(F, sep, ...) \
	Z_UTIL_LISTIFY_192(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(192, __VA_ARGS__)

#define Z_UTIL_LISTIFY_194(F, sep, ...) \
	Z_UTIL_LISTIFY_193(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(193, __VA_ARGS__)

#define Z_UTIL_LISTIFY_195(F, sep, ...) \
	Z_UTIL_LISTIFY_194(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(194, __VA_ARGS__)

#define Z_UTIL_LISTIFY_196(F, sep, ...) \
	Z_UTIL_LISTIFY_195(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(195, __VA_ARGS__)

#define Z_UTIL_LISTIFY_197(F, sep, ...) \
	Z_UTIL_LISTIFY_196(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(196, __VA_ARGS__)

#define Z_UTIL_LISTIFY_198(F, sep, ...) \
	Z_UTIL_LISTIFY_197(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(197, __VA_ARGS__)

#define Z_UTIL_LISTIFY_199(F, sep, ...) \
	Z_UTIL_LISTIFY_198(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(198, __VA_ARGS__)

#define Z_UTIL_LISTIFY_200(F, sep, ...) \
	Z_UTIL_LISTIFY_199(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(199, __VA_ARGS__)

#define Z_UTIL_LISTIFY_201(F, sep, ...) \
	Z_UTIL_LISTIFY_200(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(200, __VA_ARGS__)

#define Z_UTIL_LISTIFY_202(F, sep, ...) \
	Z_UTIL_LISTIFY_201(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(201, __VA_ARGS__)

#define Z_UTIL_LISTIFY_203(F, sep, ...) \
	Z_UTIL_LISTIFY_202(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(202, __VA_ARGS__)

#define Z_UTIL_LISTIFY_204(F, sep, ...) \
	Z_UTIL_LISTIFY_203(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(203, __VA_ARGS__)

#define Z_UTIL_LISTIFY_205(F, sep, ...) \
	Z_UTIL_LISTIFY_204(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(204, __VA_ARGS__)

#define Z_UTIL_LISTIFY_206(F, sep, ...) \
	Z_UTIL_LISTIFY_205(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(205, __VA_ARGS__)

#define Z_UTIL_LISTIFY_207(F, sep, ...) \
	Z_UTIL_LISTIFY_206(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(206, __VA_ARGS__)

#define Z_UTIL_LISTIFY_208(F, sep, ...) \
	Z_UTIL_LISTIFY_207(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(207, __VA_ARGS__)

#define Z_UTIL_LISTIFY_209(F, sep, ...) \
	Z_UTIL_LISTIFY_208(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(208, __VA_ARGS__)

#define Z_UTIL_LISTIFY_210(F, sep, ...) \
	Z_UTIL_LISTIFY_209(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(209, __VA_ARGS__)

#define Z_UTIL_LISTIFY_211(F, sep, ...) \
	Z_UTIL_LISTIFY_210(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(210, __VA_ARGS__)

#define Z_UTIL_LISTIFY_212(F, sep, ...) \
	Z_UTIL_LISTIFY_211(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(211, __VA_ARGS__)

#define Z_UTIL_LISTIFY_213(F, sep, ...) \
	Z_UTIL_LISTIFY_212(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(212, __VA_ARGS__)

#define Z_UTIL_LISTIFY_214(F, sep, ...) \
	Z_UTIL_LISTIFY_213(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(213, __VA_ARGS__)

#define Z_UTIL_LISTIFY_215(F, sep, ...) \
	Z_UTIL_LISTIFY_214(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(214, __VA_ARGS__)

#define Z_UTIL_LISTIFY_216(F, sep, ...) \
	Z_UTIL_LISTIFY_215(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(215, __VA_ARGS__)

#define Z_UTIL_LISTIFY_217(F, sep, ...) \
	Z_UTIL_LISTIFY_216(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(216, __VA_ARGS__)

#define Z_UTIL_LISTIFY_218(F, sep, ...) \
	Z_UTIL_LISTIFY_217(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(217, __VA_ARGS__)

#define Z_UTIL_LISTIFY_219(F, sep, ...) \
	Z_UTIL_LISTIFY_218(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(218, __VA_ARGS__)

#define Z_UTIL_LISTIFY_220(F, sep, ...) \
	Z_UTIL_LISTIFY_219(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(219, __VA_ARGS__)

#define Z_UTIL_LISTIFY_221(F, sep, ...) \
	Z_UTIL_LISTIFY_220(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(220, __VA_ARGS__)

#define Z_UTIL_LISTIFY_222(F, sep, ...) \
	Z_UTIL_LISTIFY_221(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(221, __VA_ARGS__)

#define Z_UTIL_LISTIFY_223(F, sep, ...) \
	Z_UTIL_LISTIFY_222(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(222, __VA_ARGS__)

#define Z_UTIL_LISTIFY_224(F, sep, ...) \
	Z_UTIL_LISTIFY_223(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(223, __VA_ARGS__)

#define Z_UTIL_LISTIFY_225(F, sep, ...) \
	Z_UTIL_LISTIFY_224(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(224, __VA_ARGS__)

#define Z_UTIL_LISTIFY_226(F, sep, ...) \
	Z_UTIL_LISTIFY_225(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(225, __VA_ARGS__)

#define Z_UTIL_LISTIFY_227(F, sep, ...) \
	Z_UTIL_LISTIFY_226(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(226, __VA_ARGS__)

#define Z_UTIL_LISTIFY_228(F, sep, ...) \
	Z_UTIL_LISTIFY_227(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(227, __VA_ARGS__)

#define Z_UTIL_LISTIFY_229(F, sep, ...) \
	Z_UTIL_LISTIFY_228(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(228, __VA_ARGS__)

#define Z_UTIL_LISTIFY_230(F, sep, ...) \
	Z_UTIL_LISTIFY_229(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(229, __VA_ARGS__)

#define Z_UTIL_LISTIFY_231(F, sep, ...) \
	Z_UTIL_LISTIFY_230(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(230, __VA_ARGS__)

#define Z_UTIL_LISTIFY_232(F, sep, ...) \
	Z_UTIL_LISTIFY_231(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(231, __VA_ARGS__)

#define Z_UTIL_LISTIFY_233(F, sep, ...) \
	Z_UTIL_LISTIFY_232(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(232, __VA_ARGS__)

#define Z_UTIL_LISTIFY_234(F, sep, ...) \
	Z_UTIL_LISTIFY_233(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(233, __VA_ARGS__)

#define Z_UTIL_LISTIFY_235(F, sep, ...) \
	Z_UTIL_LISTIFY_234(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(234, __VA_ARGS__)

#define Z_UTIL_LISTIFY_236(F, sep, ...) \
	Z_UTIL_LISTIFY_235(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(235, __VA_ARGS__)

#define Z_UTIL_LISTIFY_237(F, sep, ...) \
	Z_UTIL_LISTIFY_236(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(236, __VA_ARGS__)

#define Z_UTIL_LISTIFY_238(F, sep, ...) \
	Z_UTIL_LISTIFY_237(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(237, __VA_ARGS__)

#define Z_UTIL_LISTIFY_239(F, sep, ...) \
	Z_UTIL_LISTIFY_238(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(238, __VA_ARGS__)

#define Z_UTIL_LISTIFY_240(F, sep, ...) \
	Z_UTIL_LISTIFY_239(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(239, __VA_ARGS__)

#define Z_UTIL_LISTIFY_241(F, sep, ...) \
	Z_UTIL_LISTIFY_240(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(240, __VA_ARGS__)

#define Z_UTIL_LISTIFY_242(F, sep, ...) \
	Z_UTIL_LISTIFY_241(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(241, __VA_ARGS__)

#define Z_UTIL_LISTIFY_243(F, sep, ...) \
	Z_UTIL_LISTIFY_242(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(242, __VA_ARGS__)

#define Z_UTIL_LISTIFY_244(F, sep, ...) \
	Z_UTIL_LISTIFY_243(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(243, __VA_ARGS__)

#define Z_UTIL_LISTIFY_245(F, sep, ...) \
	Z_UTIL_LISTIFY_244(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(244, __VA_ARGS__)

#define Z_UTIL_LISTIFY_246(F, sep, ...) \
	Z_UTIL_LISTIFY_245(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(245, __VA_ARGS__)

#define Z_UTIL_LISTIFY_247(F, sep, ...) \
	Z_UTIL_LISTIFY_246(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(246, __VA_ARGS__)

#define Z_UTIL_LISTIFY_248(F, sep, ...) \
	Z_UTIL_LISTIFY_247(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(247, __VA_ARGS__)

#define Z_UTIL_LISTIFY_249(F, sep, ...) \
	Z_UTIL_LISTIFY_248(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(248, __VA_ARGS__)

#define Z_UTIL_LISTIFY_250(F, sep, ...) \
	Z_UTIL_LISTIFY_249(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(249, __VA_ARGS__)

#define Z_UTIL_LISTIFY_251(F, sep, ...) \
	Z_UTIL_LISTIFY_250(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(250, __VA_ARGS__)

#define Z_UTIL_LISTIFY_252(F, sep, ...) \
	Z_UTIL_LISTIFY_251(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(251, __VA_ARGS__)

#define Z_UTIL_LISTIFY_253(F, sep, ...) \
	Z_UTIL_LISTIFY_252(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(252, __VA_ARGS__)

#define Z_UTIL_LISTIFY_254(F, sep, ...) \
	Z_UTIL_LISTIFY_253(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(253, __VA_ARGS__)

#define Z_UTIL_LISTIFY_255(F, sep, ...) \
	Z_UTIL_LISTIFY_254(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(254, __VA_ARGS__)

#define Z_UTIL_LISTIFY_256(F, sep, ...) \
	Z_UTIL_LISTIFY_255(F, sep, __VA_ARGS__) __DEBRACKET sep \
	F(255, __VA_ARGS__)

#endif /* ZEPHYR_INCLUDE_SYS_UTIL_LOOPS_H_ */
