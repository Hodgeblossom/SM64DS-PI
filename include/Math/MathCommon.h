#pragma once

#include <cstdint>

using s8  = int8_t;
using u8  = uint8_t;
using s16 = short;
using u16 = uint16_t;
using s32 = int;
using u32 = unsigned;
using s64 = int64_t;
using u64 = uint64_t;

// returns whether the counter reached its destination
bool ApproachLinear(s16& counter, s16 dest, s16 step);
bool ApproachLinear(s32& counter, s32 dest, s32 step);
bool ApproachLinear2(s16& counter, s16 dest, s16 step);
bool ApproachLinear2(s32& counter, s32 dest, s32 step);

u16 CountDownToZero(u16& counter); // returns the counter's new value
u8  CountDownToZero(u8&  counter); // returns the counter's new value
