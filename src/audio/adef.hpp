#pragma once
#include "../alias.hpp"
#include <sndfile.h>

#define FFT_SIZE 4096
#define FREQUENCY_BINS 32

struct file_data
{
    file_data(SNDFILE *opened, i32 fmt, i32 sr, i32 ch, i32 fr)
        : open(opened), format(fmt), sample_rate(sr), channels(ch), frames(fr)
    {
    }
    SNDFILE *open;
    i32 format;
    i32 sample_rate;
    i32 channels;
    i32 frames;
};

struct meta_data
{
    meta_data(i32 chans, i32 sr, u32 file_samples, u32 file_bytes)
        : channels(chans), sample_rate(sr), samples(file_samples), bytes(file_bytes), position(0)
    {
    }
    i32 channels;
    i32 sample_rate;
    u64 samples;
    u64 bytes;
    u64 position;
};

struct audio_data
{
    audio_data(unique_vecf32 audio, vecf64 in, meta_data m, bool is_valid)
        : buffer(std::move(audio)), fft_in(in), meta(m), valid(is_valid) {};
    unique_vecf32 buffer;
    vecf64 fft_in;
    meta_data meta;
    bool valid;
};
