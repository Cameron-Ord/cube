#include "sys.hpp"
#include "window/window.hpp"
#include "renderer/renderer.hpp"
#include <SDL3/SDL.h>

constexpr double PI = 3.14159265358979323846;
static bool init_sdl(void);
static void quit_sdl(void);

#include <iostream>

int main(int argc, char **argv){
  if(!init_sdl()){
    return 1;
  }

  contents entries = contents(strvec(), strvec(), false, true);
  if(argc > 1 && argc < 3){
    std::string dir = std::string(argv[1]);
    entries = get_directory_contents(dir);
  } else {
    std::cout << "Usage: sv relative/path/to/directory" << std::endl;
    return 0;
  }

  if(!entries.valid){
    return 1;
  }

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
  quit_sdl();
	return 0;
}

bool init_sdl(void){
  return SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
}

void quit_sdl(void){
  SDL_Quit();
}

