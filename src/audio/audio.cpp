#include "audio.hpp"
#include <cstring>
#include <iostream>
//This is all untested and im doing my best to interpret the docs

void get_callback(void *userdata, SDL_AudioStream *stream, int add, int total_amount){
  audio_data *d = static_cast<audio_data*>(userdata);
  if(d->position >= d->samples){
    return;
  }

  i32 sample_amount = add / sizeof(f32);
  while(sample_amount > 0){
    f32 samples[128];
    memset(&samples, 0, 128 * sizeof(f32));
    const u32 total = SDL_min(sample_amount, SDL_arraysize(samples));
    for(u32 i = 0; i < total && i + d->position < d->samples; i++){
      samples[i] = (*d->buffer)[i + d->position];
    }
    SDL_PutAudioStreamData(stream, samples, total * sizeof(f32));
    d->position += total;
    sample_amount -= total;
  }
}

bool audio_streambuffer::set_audio_callback(audio_data *userdata){
  return SDL_SetAudioStreamGetCallback(stream, get_callback, userdata);
}

SDL_AudioSpec audio_streambuffer::spec_from_file(const audio_data *data){
  return {SDL_AUDIO_F32, data->channels, data->samplerate};
}

bool audio_streambuffer::spec_compare(const SDL_AudioSpec *original, const SDL_AudioSpec *updated){
  if(original->format != updated->format){
    return false;
  }

  if(original->channels != updated->channels){
    return false;
  }

  if(original->freq != updated->freq){
    return false;
  }
  return true;
}

bool audio_streambuffer::query_device_format(SDL_AudioSpec *dst){
  if(dst && device_id){
    return SDL_GetAudioDeviceFormat(device_id, dst, nullptr);
  }
  return false;
}

u32 audio_streambuffer::open_device(void){
  const u32 id = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &output_spec);
  if(id == 0){
    return 0;
  }
  return id;
}

//Every time the input spec changes the stream should be destroyed and remade
SDL_AudioStream *audio_streambuffer::create_stream(SDL_AudioSpec input_spec){
  SDL_AudioStream *stream = SDL_CreateAudioStream(&input_spec, &output_spec);
  if(!stream){
    return nullptr;
  }
  return stream;
}

void audio_streambuffer::stream_unbind(void){
  if(stream){
    SDL_UnbindAudioStream(stream);
  }
}

bool audio_streambuffer::audio_stream_bind(void){
  if(stream && device_id != 0){
    return SDL_BindAudioStream(device_id, stream);
  }
  return false;
}

void audio_streambuffer::audio_device_close(void){
  if(device_id)
    SDL_CloseAudioDevice(device_id);
}

void audio_streambuffer::pause_audio(void){
  if(device_id)
    SDL_PauseAudioDevice(device_id);
}

void audio_streambuffer::resume_audio(void){
  if(device_id)
    SDL_ResumeAudioDevice(device_id);
}

void audio_streambuffer::stream_destroy(void){
  if(stream)
    SDL_DestroyAudioStream(stream);
}
