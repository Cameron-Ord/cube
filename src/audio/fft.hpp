#pragma once

#include "../alias.hpp"
typedef struct
{
    f64 real;
    f64 imag;
} compf64;
using vec_compf64 = std::vector<compf64>;

struct transformer {
  transformer(void);
  
  vec_compf64 output;
  vecf64 amplitudes;
  vecf64 hamming_values;
  f64 avg;
 
  void ema_update(f64 averaged) { avg = averaged; }
  f64 ema_calculate(f64 sum, f64 alpha);
  f64 bass_freq_sum(const i32 sample_rate);
  void compf_to_float(void);
  void fft_exec(const vecf64& fft_in, const i32 sample_rate);
  size_t bit_reverse(size_t index, size_t log2n);
  void iterative_fft(vecf64& fft_in);
  void hamming_window(vecf64& fft_in);
  void calculate_window(void);
  f64 linear_smooth(f64 base, f64 sm, i32 amt, i32 frames);
  void interpolate(vecf64& sums, vecf64& ssmooth, vecf64& ssmear, const i32 frames);
  void print_ema(f64 sum);
};


#define FFT_SIZE 2048
