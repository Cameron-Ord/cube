#include "sys.hpp"
#include "audio/audio.hpp"
#include "window/window.hpp"
#include "renderer/renderer.hpp"
#include <SDL3/SDL.h>

constexpr double PI = 3.14159265358979323846;
static bool init_sdl(void);
static void quit_sdl(void);

#include <iostream>

int main(int argc, char **argv){
  if(!init_sdl()){
    log_write_str("Failed to initialize SDL3:", SDL_GetError());
    return 1;
  }

  contents entries = contents(strvec(), strvec(), false, true);
  if(argc > 1 && argc < 3){
    std::string dir = std::string(argv[1]);
    entries = get_directory_contents(dir);
  } else {
    std::cout << "Usage: sv relative/path/to/directory" << std::endl;
  }

  if(!entries.valid){
    log_write_str("Entries marked invalid", "");
    return 1;
  }

  audio_streambuffer stream;
  if(!stream.set_device_id(stream.open_device())){
    log_write_str("Failed to open default audio device:", SDL_GetError());
    return 1;
  }
  stream.pause_audio();

  audio_data data = audio_data(nullptr, 0, 0, 0, 0);
  {
    const char *path = entries.entry_paths[0].c_str();
    data = read_file(open_file(path));
  }

  if(!stream.set_stream_ptr(stream.create_stream(stream.spec_from_file(&data)))){
    log_write_str("Failed create audio stream:", SDL_GetError());
    return 1;
  }

  if(!stream.set_audio_callback(&data)){
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
    grid_pos(0.25f, 0.25f, 0.25f), 
    grid_pos(-0.25f, 0.25f, 0.25f),
    grid_pos(-0.25f, -0.25f, 0.25f),
    grid_pos(0.25f, -0.25f, 0.25f),

    grid_pos(0.25f, 0.25f, -0.25f), 
    grid_pos(-0.25f, 0.25f, -0.25),
    grid_pos(-0.25f, -0.25f, -0.25f),
    grid_pos(0.25f, -0.25f, -0.25f)
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

  SDL_ShowWindow(win.get_window());
  SDL_EnableScreenSaver();
  const u32 frame_gate = 1000 / 60;
  bool running = true;
  
  //f32 dz = 0.0f;
  f32 angle = 0.0f;
  while(running){
    u64 start = SDL_GetTicks();
    //dz += 1.0f * (1.0f / 60);
    angle += PI*(1.0f/60);

    rend.colour(0, 0, 0, 255);
    rend.clear();
  
    SDL_Event event;
    while(SDL_PollEvent(&event)){
      switch(event.type){
        default: break;

        case SDL_EVENT_QUIT:
            running = false;
            break;
      }
    }
  
    rend.colour(255, 255, 255, 255);
    std::vector<grid_pos> rotated = rend.rotate_vertices_xz(&vertices, angle);
    std::vector<grid_pos> translated = rend.translate_vertices_z(&rotated, 1.0f);
    rend.render_wire_frame(&edges, &translated);
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

