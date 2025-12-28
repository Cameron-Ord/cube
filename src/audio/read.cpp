#include "audio.hpp"
#include <sndfile.h>
#include <iostream>
#include <cstring>
#include <vector>

file_data file_reader::open_file(const char *path){
  SF_INFO info;
  memset(&info, 0, sizeof(SF_INFO));
  
  SNDFILE *file = sf_open(path, SFM_READ, &info);
  if(!file){
    std::cerr << "Could not open file->" << sf_strerror(nullptr) << std::endl;
    return file_data(nullptr, 0, 0, 0, 0);
  }

  const i32 frames = info.frames;
  const i32 samplerate = info.samplerate;
  const i32 channels = info.channels;
  const i32 format = info.format;

  return file_data(file, format, samplerate, channels, frames);
}

audio_data file_reader::read_file(file_data file){
  std::cout << "Channels: " << file.channels << std::endl;
  std::cout << "Sample Rate: " << file.samplerate << std::endl;
  
  const u32 samples = file.frames * file.channels;
  const u32 bytes = samples * sizeof(f32);

  std::shared_ptr<std::vector<f32>> buffer = std::make_shared<std::vector<f32>>(samples);
  if(sf_read_float(file.open, buffer->data(), samples) < 0){
    std::cerr << "Error reading audio data->" << sf_strerror(file.open) << std::endl;
    sf_close(file.open);
    return audio_data(nullptr, 0, 0, 0, 0);
  }
  sf_close(file.open);
  return audio_data(buffer, file.channels, file.samplerate, samples, bytes);
}
