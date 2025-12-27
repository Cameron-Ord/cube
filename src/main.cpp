#include <SDL3/SDL.h>
#include "window/window.hpp"
#include "renderer/renderer.hpp"

constexpr double PI = 3.14159265358979323846;
static bool init_sdl(void);
static void quit_sdl(void);

#include <iostream>

int main(int argc, char **argv){
  if(!init_sdl()){
    return 1;
  }

  window win = window("sv", 400, 300, SDL_WINDOW_HIDDEN);
  win.set_window(win.create());
  renderer rend = renderer(nullptr, win.get_width(), win.get_height());
  rend.set_renderer(rend.create(win.get_window()));

  std::vector<grid> vertices = {
    grid(0.25f, 0.25f, 0.25f), 
    grid(-0.25f, 0.25f, 0.25f),
    grid(0.25f, -0.25f, 0.25f),
    grid(-0.25f, -0.25f, 0.25f),

    grid(0.25f, 0.25f, -0.25f), 
    grid(-0.25f, 0.25f, -0.25),
    grid(0.25f, -0.25f, -0.25f),
    grid(-0.25f, -0.25f, -0.25f)

  };

  SDL_ShowWindow(win.get_window());
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
    std::vector<grid> rotated = rend.rotate_vertices_xz(&vertices, angle);
    std::vector<grid> translated = rend.translate_vertices_z(&rotated, 1.0f);
    rend.draw_vertices(&translated);
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

