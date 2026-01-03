#pragma once
#include "../alias.hpp"
#define FREQUENCY_BINS 64
#define FFT_SIZE 4096

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


void print_bins(const std::vector<freq_range>& ranges);
std::vector<freq_range> gen_bins(void);

struct transformer
{
    transformer(void);

    vec_compf64 output;
    vecf64 amplitudes;
    vecf64 hamming_values;

    f64 freq_range_sum(const f64& MIN_FREQ, const f64& MAX_FREQ, const i32& sample_rate);
    void compf_to_float(void);
    std::vector<f64> fft_exec(const vecf64 &fft_in, const i32& sample_rate, const std::vector<freq_range>& ranges);
    size_t bit_reverse(size_t index, size_t log2n);
    void iterative_fft(vecf64 &fft_in);
    void hamming_window(vecf64 &fft_in);
    void calculate_window(void);
    std::vector<f64> sum_ranges(const i32& sample_rate, const std::vector<freq_range>& ranges);
    std::vector<f64> nsum_ranges(const i32& sample_rate, const std::vector<freq_range>& ranges);
    void bins_print(std::vector<f64>& bins);
};


