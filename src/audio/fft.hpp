#pragma once
#include "../alias.hpp"
#define FFT_SIZE 4096
#define FREQUENCY_BINS 32

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
    vecf64 magnitudes;
    vecf64 hamming_values;
    std::vector<freq_range> ranges;

    void compf_to_float(void);
    std::vector<f64> fft_exec(const vecf64 &fft_in, const i32& sample_rate);
    size_t bit_reverse(size_t index, size_t log2n);
    void iterative_fft(vecf64 &fft_in);
    void hamming_window(vecf64 &fft_in);
    void calculate_window(void);
    f64 msum_compress_positive(f64 msum);

    f64 sum_in_range(const f64& min, const f64& max, const i32& sample_rate);
    std::vector<f64> nsum_in_ranges(const i32& sample_rate);
};


