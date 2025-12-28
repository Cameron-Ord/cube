#include "audio.hpp"
#include <iostream>
//This is all untested and im doing my best to interpret the docs

void get_callback(void *userdata, SDL_AudioStream *stream, int add, int total){
  return;
}

bool streambuffer::set_audio_callback(audio_data *userdata){
  return SDL_SetAudioStreamGetCallback(stream, get_callback, userdata);
}

SDL_AudioSpec streambuffer::spec_from_file(const file_data *data){
  return {SDL_AUDIO_F32, data->channels, data->samplerate};
}

bool streambuffer::spec_compare(const SDL_AudioSpec *original, const SDL_AudioSpec *updated){
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

bool streambuffer::query_device_format(SDL_AudioSpec *dst){
  if(dst && device_id){
    return SDL_GetAudioDeviceFormat(device_id, dst, nullptr);
  }
  return false;
}

u32 streambuffer::open_device(void){
  const u32 id = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &output_spec);
  if(id == 0){
    std::cerr << "Failed to open audio device->" << SDL_GetError() << std::endl;
    return 0;
  }
  return id;
}

//Every time the input spec changes the stream should be destroyed and remade
SDL_AudioStream *streambuffer::create_stream(SDL_AudioSpec input_spec){
  SDL_AudioStream *stream = SDL_CreateAudioStream(&input_spec, &output_spec);
  if(!stream){
    std::cerr << "Failed to create audio stream->" << SDL_GetError() << std::endl;
    return nullptr;
  }
  return stream;
}

void streambuffer::stream_unbind(void){
  if(stream){
    SDL_UnbindAudioStream(stream);
  }
}

bool streambuffer::audio_stream_bind(void){
  if(stream && device_id != 0){
    return SDL_BindAudioStream(device_id, stream);
  }
  return false;
}

void streambuffer::audio_device_close(void){
  if(device_id)
    SDL_CloseAudioDevice(device_id);
}

void streambuffer::pause_audio(void){
  if(device_id)
    SDL_PauseAudioDevice(device_id);
}

void streambuffer::resume_audio(void){
  if(device_id)
    SDL_ResumeAudioDevice(device_id);
}

void streambuffer::stream_destroy(void){
  if(stream)
    SDL_DestroyAudioStream(stream);
}
