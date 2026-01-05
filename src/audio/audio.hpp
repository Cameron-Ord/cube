#pragma once
#include "adef.hpp"
#include <memory>
#include <SDL3/SDL_audio.h>

#define SAMPLES 128

void get_callback(void *userdata, SDL_AudioStream *stream, int add, int total);

typedef struct sf_private_tag SNDFILE;

struct audio_streambuffer
{
    audio_streambuffer() : stream(nullptr), output_spec({SDL_AUDIO_F32, 2, 44100}), device_id(0) {}
    ~audio_streambuffer() = default;

    u32 open_device(void);
    bool set_audio_callback(std::unique_ptr<audio_data> &data);

    bool audio_stream_bind(void);
    bool query_device_format(SDL_AudioSpec *dst);

    bool spec_compare(const SDL_AudioSpec *original, const SDL_AudioSpec *updated);
    SDL_AudioSpec spec_from_file(std::unique_ptr<audio_data> &data);
    SDL_AudioStream *create_stream(SDL_AudioSpec input_spec);

    void pause_audio(void);
    void resume_audio(void);
    void stream_destroy(void);

    void audio_device_close(void);
    void stream_unbind(void);
    bool set_stream_ptr(SDL_AudioStream *sptr)
    {
        if (!sptr) {
            return false;
        }
        stream = sptr;
        return true;
    }
    bool set_device_id(u32 id)
    {
        if (!id) {
            return false;
        }
        device_id = id;
        return true;
    }

    SDL_AudioStream *stream;
    SDL_AudioSpec output_spec;
    u32 device_id;
};
