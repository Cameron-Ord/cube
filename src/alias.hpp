#pragma once

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint64_t u64;
typedef uint32_t u32;
typedef float f32;
typedef double f64;

using strvec = std::vector<std::string>;
using veci32 = std::vector<i32>;
using vecf32 = std::vector<f32>;
using vecf64 = std::vector<f64>;
using unique_vecf32 = std::unique_ptr<vecf32>;
