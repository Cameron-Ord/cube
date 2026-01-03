#pragma once
#include "../alias.hpp"
#define FFT_SIZE 4096

#define EMA_LESS -1
#define EMA_DEFAULT 0
#define EMA_MORE 1

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


f64 ema_calculate(f64 val, f64 alpha);
void ema_update(f64 val);
i8 to_ema(f64 val);



struct transformer
{
    transformer(void);

    vec_compf64 output;
    vecf64 amplitudes;
    vecf64 hamming_values;

    f64 mean_to_db(f64 mean);
    f64 rms_to_db(f64 rms);
    f64 rms(f64 mean);
    f64 normalize_db(f64 db);
    void compf_to_float(void);
    f64 fft_exec(const vecf64 &fft_in, const i32& sample_rate);
    size_t bit_reverse(size_t index, size_t log2n);
    void iterative_fft(vecf64 &fft_in);
    void hamming_window(vecf64 &fft_in);
    void calculate_window(void);
    f64 mean_sum_in_range(const i32& sample_rate);

};


