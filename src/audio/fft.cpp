#include "fft.hpp"
#include "../util.hpp"
#include <algorithm> 
#include <cmath>

const f64 FREQ_RANGE_MIN = 40.0;
const f64 FREQ_RANGE_MAX = 8000.0;
const f64 DB_MIN = -60.0;
const f64 DB_MAX = -8.0;

float avg = 0.0;

f64 ema_calculate(f64 val, f64 alpha){
  return alpha * val + (1.0 - alpha) * avg;
}

void ema_update(f64 val){
  avg = val;
}

i8 to_ema(f64 val){
  if(val < avg){
    return EMA_LESS;
  }  else if (val > avg){
    return EMA_MORE;
  } else {
    return EMA_DEFAULT;
  }
}

transformer::transformer(void) : output(FFT_SIZE), amplitudes(FFT_SIZE / 2), hamming_values(FFT_SIZE)
{
    calculate_window();
}

f64 transformer::fft_exec(const vecf64 &fft_in, const i32& sample_rate)
{
    vecf64 samples = vecf64(fft_in);
    hamming_window(samples);
    iterative_fft(samples);
    compf_to_float();
    return normalize_db(mean_to_db(mean_sum_in_range(sample_rate)));
}

static compf64 c_from_real(const f64 real)
{
    compf64 _complex;
    _complex.real = real;
    _complex.imag = 0.0;
    return _complex;
}

static compf64 c_from_imag(const f64 imag)
{
    compf64 _complex;
    _complex.real = 0.0;
    _complex.imag = imag;
    return _complex;
}

static compf64 compf64_exp(const compf64 &c)
{
    compf64 res;
    f64 exp_real = exp(c.real);
    res.real = exp_real * cos(c.imag);
    res.imag = exp_real * sin(c.imag);
    return res;
}

static compf64 compf64_subtract(const compf64 &a, const compf64 &b)
{
    compf64 sub;
    sub.real = a.real - b.real;
    sub.imag = a.imag - b.imag;
    return sub;
}

static compf64 compf64_add(const compf64 &a, const compf64 &b)
{
    compf64 add;
    add.real = a.real + b.real;
    add.imag = a.imag + b.imag;
    return add;
}

static compf64 compf64_mult(const compf64 &a, const compf64 &b)
{
    compf64 mult;
    mult.real = a.real * b.real - a.imag * b.imag;
    mult.imag = a.real * b.imag + a.imag * b.real;
    return mult;
}

static compf64 compf64_step(const size_t &half_len, const compf64 &iota)
{
    compf64 step;
    f64 theta = (f64)PI / half_len;

    step.real = iota.real * theta;
    step.imag = iota.imag * theta;

    step = compf64_exp(step);
    return step;
}

size_t transformer::bit_reverse(size_t index, size_t log2n)
{
    size_t reversed = 0;
    for (size_t i = 0; i < log2n; i++) {
        reversed <<= 1;
        reversed |= (index & 1);
        index >>= 1;
    }
    return reversed;
}

void transformer::iterative_fft(vecf64 &fft_in)
{
    for (size_t i = 0; i < FFT_SIZE; i++) {
        i32 rev_index = bit_reverse(i, log2(FFT_SIZE));
        output[i] = c_from_real(fft_in[rev_index]);
    }

    const compf64 iota = c_from_imag(1.0f);
    for (size_t stage = 1; stage <= log2(FFT_SIZE); ++stage) {
        size_t sub_arr_size = (size_t)1 << stage; // 2^stage
        size_t half_sub_arr = sub_arr_size >> 1;
        compf64 twiddle = c_from_real(1.0f);

        compf64 step = compf64_step(half_sub_arr, iota);
        for (size_t j = 0; j < half_sub_arr; j++) {
            for (size_t k = j; k < FFT_SIZE; k += sub_arr_size) {
                compf64 t = compf64_mult(twiddle, output[k + half_sub_arr]);
                compf64 u = output[k];

                output[k] = compf64_add(u, t);
                output[k + half_sub_arr] = compf64_subtract(u, t);
            }
            twiddle = compf64_mult(twiddle, step);
        }
    }
}

void transformer::compf_to_float(void)
{
    const size_t HALF_FFT_SIZE = FFT_SIZE / 2;
    for (size_t i = 0; i < HALF_FFT_SIZE; i++) {
        const compf64 &c = output[i];
        amplitudes[i] = sqrt(c.real * c.real + c.imag * c.imag);
    }
}

void transformer::hamming_window(vecf64 &fft_in)
{
    for (i32 i = 0; i < FFT_SIZE; i++) {
        fft_in[i] = fft_in[i] * hamming_values[i];
    }
}

void transformer::calculate_window(void)
{
    for (i32 i = 0; i < FFT_SIZE; ++i) {
        f64 t = (f64)i / (FFT_SIZE - 1);
        hamming_values[i] = 0.54 - 0.46 * cos(2.0 * PI * t);
    }
}


f64 transformer::mean_to_db(f64 mean) {
    return 10.0 * log10(mean + 1e-20);
}

f64 transformer::rms_to_db(f64 rms) {
    return 20.0 * log10(rms + 1e-20);
}

f64 transformer::normalize_db(f64 db){
  return std::clamp((db - DB_MIN) / (DB_MAX - DB_MIN), 0.0, 1.0);
}

f64 transformer::rms(f64 mean){
  return sqrt(mean);
}

f64 transformer::mean_sum_in_range(const i32& sample_rate)
{
    const u32 bin_max = (FREQ_RANGE_MAX * FFT_SIZE / sample_rate);
    const u32 bin_min = (FREQ_RANGE_MIN * FFT_SIZE / sample_rate);
    f64 sum = 0.0;
    u32 count = 0;
    
    for (u32 i = bin_min; i < bin_max && i < amplitudes.size(); i++) {
        sum += amplitudes[i] * amplitudes[i];
        count++;
    }

    if(count > 0)
      return sum / count;

    return 0.0;
}

