#include "entries.hpp"
#include "audio/fft.hpp"
#include "util.hpp"
#include "audio/audio.hpp"
#include "window/window.hpp"
#include "renderer/renderer.hpp"
#include <SDL3/SDL.h>
#include <cmath>

const SDL_Color background = { 76 , 86 , 106 , 255  };

static bool init_sdl(void);
static void quit_sdl(void);

#include <iostream>

int main(int argc, char **argv){
  if(!init_sdl()){
    log_write_str("Failed to initialize SDL3:", SDL_GetError());
    return 1;
  }

  contents entries = contents(strvec(0), strvec(0), false, true);
  if(argc > 1 && argc < 3){
    std::string dir = std::string(argv[1]);
    entries = get_directory_contents(dir);
  } else {
    std::cout << "Usage: sv relative/path/to/directory" << std::endl;
    return 0;
  }

  if(!entries.valid || entries.empty){
    log_write_str("Entries marked invalid or empty", "");
    return 1;
  }

  audio_streambuffer stream;
  if(!stream.set_device_id(stream.open_device())){
    log_write_str("Failed to open default audio device:", SDL_GetError());
    return 1;
  }
  stream.pause_audio();
  
  strvec::iterator entry_iterator = entries.entry_paths.begin();
  std::unique_ptr<audio_data> data = std::make_unique<audio_data>(nullptr, vecf64(), meta_data(0, 0, 0, 0), false);
  const char *path = (*entry_iterator).c_str();
  *data = read_file(open_file(path));

  if(!stream.set_stream_ptr(stream.create_stream(stream.spec_from_file(data)))){
    log_write_str("Failed create audio stream:", SDL_GetError());
    return 1;
  }

  if(!stream.set_audio_callback(data)){
    log_write_str("Failed to assign audio get callback:", SDL_GetError()); 
    return 1;
  }

  if(!stream.audio_stream_bind()){
    log_write_str("Failed to bind stream to device:", SDL_GetError());
    return 1;
  }
  stream.resume_audio();

  window win = window("sv", 400, 300, SDL_WINDOW_HIDDEN);
  win.set_window(win.create());
  renderer rend = renderer(nullptr, win.get_width(), win.get_height());
  rend.set_renderer(rend.create(win.get_window()));

  std::vector<grid_pos> vertices = {
    grid_pos(0.5f, 0.5f, 0.5f), 
    grid_pos(-0.5f, 0.5f, 0.5f),
    grid_pos(-0.5f, -0.5f, 0.5f),
    grid_pos(0.5f, -0.5f, 0.5f),

    grid_pos(0.5f, 0.5f, -0.5f), 
    grid_pos(-0.5f, 0.5f, -0.5f),
    grid_pos(-0.5f, -0.5f, -0.5f),
    grid_pos(0.5f, -0.5f, -0.5f)
  };

  std::vector<indice4> indices = {
    indice4(0, 1, 2, 3),
    indice4(4, 5, 6, 7),
    indice4(1, 5, 6, 2),
    indice4(0, 3, 7, 4),
    indice4(0, 4, 5, 1), 
    indice4(3, 2, 6, 7)
  };
  std::vector<edge> edges = rend.make_edges(&indices);
  std::vector<indice3> triangle_indices = rend.quad_to_triangle(&indices);
  transformer fft;
  range_holder ranges;

  SDL_ShowWindow(win.get_window());
  SDL_EnableScreenSaver();

  const f32 scale_default = 1.0f;
  const f32 scale_high = 1.5f;
  const f32 scale_low = 0.5;

  const u32 FPS = 240;
  // a = 1 - e(-t / time_constant)
  const f32 ema_alpha = 1.0 - exp(-1.0 / (FPS * 0.10));
  const f32 frame_alpha = 1.0 - exp(-1.0 / (FPS * 0.15));

  const u32 frame_gate = 1000 / FPS;
  bool running = true;

  //f32 dz = 0.0f;
  f32 angle = 0.0f;
  while(running){
    u64 start = SDL_GetTicks();
    rend.colour(background.r, background.g, background.b, background.a);
    rend.clear();

    if(data->valid && data->meta.position >= data->meta.samples){
      stream.pause_audio();
      entry_iterator = get_next_entry(strvec_view(entries.entry_paths, entry_iterator));
      *data = read_file(open_file(entry_iterator->c_str()));
      stream.resume_audio();
    }

    if(data->valid && data->meta.position < data->meta.samples){
      ranges.sums = fft.fft_exec(data->fft_in, data->meta.sample_rate);
      for(size_t i = 0; i < 1; i++){
        rythm_interpreter& ri = ranges.intrps[i];

        ri.ema_update(ri.ema_calculate(ranges.sums[i], ema_alpha));
        if(ri.is_more(ranges.sums[i])){
          ri.interpolate_apply(ri.smoothed_scale, ri.scale_interpolate(scale_high, ri.smoothed_scale, frame_alpha));
          ri.interpolate_apply(ri.smeared_scale, ri.scale_interpolate(ri.smoothed_scale, ri.smeared_scale, frame_alpha));
        } else if (ri.is_less(ranges.sums[i])){
          ri.interpolate_apply(ri.smoothed_scale, ri.scale_interpolate(scale_low, ri.smoothed_scale, frame_alpha));
          ri.interpolate_apply(ri.smeared_scale, ri.scale_interpolate(ri.smoothed_scale, ri.smeared_scale, frame_alpha));
        } else {
          ri.interpolate_apply(ri.smoothed_scale, ri.scale_interpolate(scale_default, ri.smoothed_scale, frame_alpha));
          ri.interpolate_apply(ri.smeared_scale, ri.scale_interpolate(ri.smoothed_scale, ri.smeared_scale, frame_alpha));
        }

      }
    }

    //dz += 1.0f * (1.0f / 60);
    angle += PI*(1.0f/FPS);
  
    SDL_Event event;
    while(SDL_PollEvent(&event)){
      switch(event.type){
        default: break;

        case SDL_EVENT_QUIT:
            running = false;
            break;
      }
    }


    std::vector<grid_pos> smear = rend.scale_vertices(vertices, ranges.intrps[0].smeared_scale);
    std::vector<grid_pos> smooth = rend.scale_vertices(vertices, ranges.intrps[0].smoothed_scale);
    
    std::vector<grid_pos> smearz = rend.translate_vertices_z(&smear, 2.0f);
    std::vector<grid_pos> smoothz = rend.translate_vertices_z(&smooth, 2.0f);

    rend.render_triangles(smearz, triangle_indices, tri_spec(ranges.intrps[0].smeared_scale, {163.0f / 255.0f, 190 / 255.0f , 140 / 255.0f, 255/ 255.0f}));
    rend.render_triangles(smoothz, triangle_indices, tri_spec(ranges.intrps[0].smoothed_scale, {94.0f / 255.0f, 129 / 255.0f , 172 / 255.0f, 255/ 255.0f}));
    //rend.draw_points(&translated);
    rend.present();


    u64 frame_time = SDL_GetTicks() - start;
    if(frame_time < frame_gate){
      u32 delay = static_cast<u32>(frame_gate - frame_time);
      SDL_Delay(delay);
    }

  }
  
  stream.audio_device_close();
  stream.stream_destroy();
  quit_sdl();
	return 0;
}

bool init_sdl(void){
  return SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
}

void quit_sdl(void){
  SDL_Quit();
}

