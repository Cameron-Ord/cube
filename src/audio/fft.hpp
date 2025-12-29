#pragma once

#include "../alias.hpp"

#define FFT_SIZE 4096
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
  f64 bass_freq_sum(const i32 sample_rate);
  void compf_to_float(void);
  f64 fft_exec(const vecf64& fft_in, const i32 sample_rate);
  size_t bit_reverse(size_t index, size_t log2n);
  void iterative_fft(vecf64& fft_in);
  void hamming_window(vecf64& fft_in);
  void calculate_window(void);
  f64 linear_smooth(f64 base, f64 sm, i32 amt, i32 frames);
  void interpolate(vecf64& sums, vecf64& ssmooth, vecf64& ssmear, const i32 frames);
};

struct rythm_interpreter {
  rythm_interpreter(void) : avg(0.0), smoothed_scale(1.0f), smeared_scale(1.0f) {}
  f64 avg;
  f32 smoothed_scale;
  f32 smeared_scale;

  void interpolate_apply(f32& interpolated, f32 scale) { interpolated += scale; }
  f32 scale_interpolate(const f32& target_scale, const f32& prev, const f32& smoothing_amount, const i32& fps);
  bool is_less(const f64& sum); 
  bool is_more(const f64& sum);
  void ema_update(f64 averaged) { avg = averaged; }
  f64 ema_calculate(f64 sum, f64 alpha);
  void print_ema(f64 sum);
};
