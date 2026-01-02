#pragma once
#include "../alias.hpp"
#include <array>

#define FREQUENCY_BINS 8
#define FFT_SIZE 1024
typedef struct
{
    f64 real;
    f64 imag;
} compf64;
using vec_compf64 = std::vector<compf64>;

struct freq_range {
  freq_range(f64 l, f64 h) : low(l), high(h) {}
  f64 low;
  f64 high;
};

struct transformer
{
    transformer(void);

    vec_compf64 output;
    vecf64 amplitudes;
    vecf64 hamming_values;

    f64 freq_range_sum(const f64& MIN_FREQ, const f64& MAX_FREQ, const i32& sample_rate);
    void compf_to_float(void);
    std::array<f64, FREQUENCY_BINS> fft_exec(const vecf64 &fft_in, const i32& sample_rate);
    size_t bit_reverse(size_t index, size_t log2n);
    void iterative_fft(vecf64 &fft_in);
    void hamming_window(vecf64 &fft_in);
    void calculate_window(void);
    std::array<f64, FREQUENCY_BINS> sum_ranges(const i32& sample_rate);
    std::array<f64, FREQUENCY_BINS> nsum_ranges(const i32& sample_rate);
    void bins_print(std::array<f64, FREQUENCY_BINS>& bins);
};

struct processor
{
    f32 interpolate(const f32 &target_scale, const f32 &prev, const f32 &alpha);
};

