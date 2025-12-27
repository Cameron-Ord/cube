#pragma once

#include "../alias.hpp"

#include <vector>
#include <SDL3/SDL_rect.h>

typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Window SDL_Window;

struct grid {
  grid(f32 xval, f32 yval, f32 zval) : x(xval), y(yval), z(zval) {};
  f32 x, y, z;
};

struct position {
  position(f32 xval, f32 yval) : x(xval), y(yval) {}
  f32 x, y;
};

struct rect {
  rect(f32 x, f32 y, f32 w, f32 h) : box({x, y, w, h}) {}
  SDL_FRect box;
};

class renderer {
  public:
    renderer(const char *driver_name, i32 ww, i32 wh) : dname(driver_name), window_width(ww), window_height(wh), r(nullptr) {} 
    ~renderer() = default;

    void set_point(position p);
    position to_screen(position p);
    position project(grid gpos);
    grid rotate_xz(grid gpos, f32 angle);
    grid rotate_yz(grid gpos, f32 angle);
    void draw_vertices(const std::vector<grid> *vertices);
    std::vector<grid> translate_vertices_z(const std::vector<grid> *vertices, f32 dz);
    std::vector<grid> rotate_vertices_xz(const std::vector<grid> *vertices, f32 angle);
    grid translate_z(grid gpos, f32 inc);
    grid translate_x(grid gpos, f32 inc);
    grid translate_y(grid gpos, f32 inc);

    SDL_Renderer *create(SDL_Window *w);
    void set_renderer(SDL_Renderer *ptr) { r = ptr; }
    SDL_Renderer *get_renderer(void) { return r; }
    void fill_rect(rect builder);
    void clear(void);
    void colour(u8 r8, u8 g8, u8 b8, u8 a8);
    void present(void);

  private:
    const char *dname;
    i32 window_width, window_height;
    SDL_Renderer *r;
};
