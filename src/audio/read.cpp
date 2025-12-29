#include "audio.hpp"
#include "fft.hpp"
#include "../util.hpp"
#include <sndfile.h>
#include <iostream>
#include <cstring>
#include <vector>

file_data open_file(const char *path){
  SF_INFO info;
  memset(&info, 0, sizeof(SF_INFO));
  
  SNDFILE *file = sf_open(path, SFM_READ, &info);
  if(!file){
    log_write_str("Failed to open file:", sf_strerror(nullptr));
    return file_data(nullptr, 0, 0, 0, 0);
  }

  const i32 frames = info.frames;
  const i32 samplerate = info.samplerate;
  const i32 channels = info.channels;
  const i32 format = info.format;

  return file_data(file, format, samplerate, channels, frames);
}

audio_data read_file(file_data file){
  std::cout << "Channels: " << file.channels << std::endl;
  std::cout << "Sample Rate: " << file.sample_rate << std::endl;
  
  const u32 samples = file.frames * file.channels;
  const u32 bytes = samples * sizeof(f32);
  
  unique_vecf32 buffer = std::make_unique<vecf32>(samples);
  if(sf_read_float(file.open, buffer->data(), samples) < 0){
    log_write_str("Error while reading audio chunks:", sf_strerror(file.open));
    sf_close(file.open);
    return audio_data(nullptr, vecf64(), meta_data(0, 0, 0, 0), false);
  }
  sf_close(file.open);
  return audio_data(std::move(buffer), vecf64(FFT_SIZE), meta_data(file.channels, file.sample_rate, samples, bytes), true);
}
