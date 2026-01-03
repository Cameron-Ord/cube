#include "fft.hpp"
#include "../util.hpp"
#include <algorithm> 
#include <iostream>
#include <cmath>


const f64 EPS = 1e-20;
const f64 GAIN = 20.0;
const f64 FREQ_RANGE_MIN = 100.0;
const f64 FREQ_RANGE_MAX = 16000.0;

transformer::transformer(void) : output(FFT_SIZE), magnitudes(FFT_SIZE / 2), hamming_values(FFT_SIZE)
{
    calculate_window();

    const f64 RATIO = FREQ_RANGE_MAX / FREQ_RANGE_MIN;
    ranges.resize(FREQUENCY_BINS, freq_range(0, 0));

    for(i32 i = 0; i < FREQUENCY_BINS; i++){
      const f64 first = (f64)i / FREQUENCY_BINS;
      const f64 last = (f64)(i+1) / FREQUENCY_BINS;

      const f64 low = FREQ_RANGE_MIN * pow(RATIO, first);
      const f64 high = FREQ_RANGE_MIN * pow(RATIO, last);

      ranges[i] = freq_range(low, high);
    }

    std::cout << "BIN COUNT: " << ranges.size() << std::endl;

    for(u32 i = 0; i < ranges.size(); i++){
      std::cout << i + 1 << "{ " << ranges[i].low << ", " << ranges[i].high << " }" << std::endl;
    }
}


f64 transformer::msum_compress_positive(f64 msum){
  const f64 f = (msum * GAIN);
  if(f > 0.0){
    return log(1.0 + f);
  } else {
    return 0.0;
  }
}

std::vector<f64> transformer::fft_exec(const vecf64 &fft_in, const i32& sample_rate)
{
    vecf64 samples = vecf64(fft_in);
    hamming_window(samples);
    iterative_fft(samples);
    compf_to_float();
    return nsum_in_ranges(sample_rate);
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
        magnitudes[i] = sqrt(c.real * c.real + c.imag * c.imag);
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


std::vector<f64> transformer::nsum_in_ranges(const i32& sample_rate){
  std::vector<f64> nsums(FREQUENCY_BINS);
  f64 max = 0.0;
  for(u32 i = 0; i < ranges.size(); i++){
    const freq_range& range = ranges[i];
    const f64 sum = msum_compress_positive(sum_in_range(range.low, range.high, sample_rate));
    if(sum > max){
      max = sum;
    }
    nsums[i] = sum;
  }

  //normalize 0 .. 1
  for(u32 i = 0; i < nsums.size() && max > 0.0; i++){
    nsums[i] /= max;
  }
  return nsums;
}

f64 transformer::sum_in_range(const f64& min, const f64& max, const i32& sample_rate)
{
    const u32 bin_max = (max * FFT_SIZE / sample_rate);
    const u32 bin_min = (min * FFT_SIZE / sample_rate);
    f64 sum = 0.0;
    for (u32 i = bin_min; i < bin_max && i < magnitudes.size(); i++) {
        sum += magnitudes[i] * magnitudes[i];
    }

    return sum;
}

