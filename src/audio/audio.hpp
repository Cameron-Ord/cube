#pragma once

#include "../alias.hpp"
#include <SDL3/SDL_audio.h>
#include <vector>
#include <memory>

void get_callback(void *userdata, SDL_AudioStream *stream, int add, int total);

typedef struct sf_private_tag SNDFILE;

struct file_data {
  file_data(SNDFILE *opened, i32 fmt, i32 sr, i32 ch, i32 fr) 
    : open(opened), format(fmt), samplerate(sr), channels(ch), frames(fr) {}
  SNDFILE *open;
  i32 format;
  i32 samplerate;
  i32 channels;
  i32 frames;
};

struct audio_data {
  audio_data(std::shared_ptr<std::vector<f32>> audio, i32 file_channels, i32 file_sr, u32 file_samples, u32 file_bytes) 
    : buffer(audio), channels(file_channels), samplerate(file_sr), samples(file_samples), bytes(file_bytes), position(0)   {};
  std::shared_ptr<std::vector<f32>> buffer;
  i32 channels;
  i32 samplerate;
  u32 samples;
  u32 bytes;
  u32 position;
};

struct file_reader {
    static file_data open_file(const char *path);
    static audio_data read_file(file_data file);
};

class streambuffer{
  public:
    streambuffer() : stream(nullptr), output_spec({SDL_AUDIO_F32, 2, 44100}), device_id(0) {}
    ~streambuffer() = default;

    u32 open_device(void);
    u32 get_device_id(void) { return device_id; }
    bool set_audio_callback(audio_data *userdata);
    
    bool audio_stream_bind(void);
    bool query_device_format(SDL_AudioSpec *dst);

    bool spec_compare(const SDL_AudioSpec *original, const SDL_AudioSpec *updated);
    SDL_AudioSpec spec_from_file(const file_data *data);
    SDL_AudioStream *create_stream(SDL_AudioSpec input_spec);
    SDL_AudioStream *get_stream_ptr(void) { return stream; }

    void pause_audio(void);
    void resume_audio(void);
    void stream_destroy(void);

    void audio_device_close(void);
    void stream_unbind(void);
    void set_stream_ptr(SDL_AudioStream *sptr) { stream = sptr; }
    void set_device_id(u32 id) { device_id = id; }

  private:
    SDL_AudioStream *stream;
    SDL_AudioSpec output_spec;
    u32 device_id;
};

