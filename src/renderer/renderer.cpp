#include "renderer.hpp"
#include <iostream>
#include <SDL3/SDL.h>
#include <cmath>

SDL_Renderer *renderer::create(SDL_Window *w){
  SDL_Renderer *tmp = SDL_CreateRenderer(w, dname);
  if(!tmp){
    std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
    return nullptr;
  }
  return tmp;
}


void renderer::clear(void){
  SDL_RenderClear(r);
}

void renderer::colour(u8 r8, u8 g8, u8 b8, u8 a8){
  SDL_SetRenderDrawColor(r, r8, g8, b8, a8);
}

void renderer::present(void){
  SDL_RenderPresent(r);
}

void renderer::fill_rect(rect builder){
  SDL_RenderFillRect(r, &builder.box);
}

void renderer::draw_vertices(const std::vector<grid> *vertices){
  std::cout << "BEGIN" << std::endl;
  for(auto it = vertices->begin(); it != vertices->end(); it++){
    set_point(to_screen(project(*it)));
  }
  std::cout << "END" << std::endl;
}

std::vector<grid> renderer::rotate_vertices_xz(const std::vector<grid> *vertices, f32 angle){
  std::vector<grid> rotated = std::vector<grid>(*vertices);
  for(size_t i = 0; i < vertices->size(); i++){
    rotated[i] = rotate_xz((*vertices)[i], angle);
  }
  return rotated;
}

std::vector<grid> renderer::translate_vertices_z(const std::vector<grid> *vertices, f32 dz){
  std::vector<grid> translated = std::vector<grid>(*vertices);
  for(size_t i = 0; i < vertices->size(); i++){
    translated[i] = translate_z((*vertices)[i], dz);
  }
  return translated;
}

void renderer::set_point(position p){
  const f32 size = 8.0f;
  fill_rect(rect(p.x - size / 2, p.y - size / 2, size, size));
}

grid renderer::translate_z(grid gpos, f32 inc){
  return grid(gpos.x, gpos.y, gpos.z + inc);
}

grid renderer::translate_x(grid gpos, f32 inc){
  return grid(gpos.x + inc, gpos.y, gpos.z);
}

grid renderer::translate_y(grid gpos, f32 inc){
  return grid(gpos.x, gpos.y + inc, gpos.z);
}

position renderer::to_screen(position p){
  std::cout << "Px: " << p.x << " " << "Py: " << p.y << " ";
  
  const f32 xnorm = (p.x + 1) / 2 * window_width;
  const f32 ynorm = (1.0f - (p.y + 1.0f) / 2) * window_height;

  std::cout << "Pxnorm: " << xnorm << " " << "Pynorm: " << ynorm << " ";
  std::cout << std::endl;

  return position(xnorm, ynorm);
}

position renderer::project(grid gpos){
  const f32 dx = gpos.x / gpos.z;
  const f32 dy = gpos.y / gpos.z;
  return position(dx, dy);
}

grid renderer::rotate_yz(grid gpos, f32 angle){
  const f32 c = cos(angle);
  const f32 s = sin(angle);
  return grid(gpos.x, gpos.y * c - gpos.z * s, gpos.y * s + gpos.z * c);
}

grid renderer::rotate_xz(grid gpos, f32 angle){
  const f32 update_x = gpos.x * cosf(angle) - gpos.z * sinf(angle);
  const f32 update_y = gpos.y;
  const f32 update_z = gpos.x * sinf(angle) + gpos.z * cosf(angle);

  return grid(update_x, update_y, update_z);
}
