#pragma once

#include "../alias.hpp"

#include <vector>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Window SDL_Window;

struct tri_spec {
  tri_spec(f32 sc, SDL_FColor col) : scale(sc), colour(col) {}
  const f32 scale;
  SDL_FColor colour;
};

struct grid_pos {
  grid_pos(f32 x0, f32 y0, f32 z0) : x(x0), y(y0), z(z0) {};
  f32 x, y, z;
};

struct scr_pos {
  scr_pos(f32 x0, f32 y0) : x(x0), y(y0) {}
  f32 x, y;
};

struct edge {
  edge(i32 x0, i32 y0) : x(x0), y(y0) {}
  i32 x, y;
};

struct indice4 {
  indice4(i32 x0, i32 y0, i32 z0, i32 k0) : x(x0), y(y0), z(z0), k(k0) {}
  i32 x, y, z, k;
};

struct indice3 {
  indice3(i32 x0, i32 y0, i32 z0) : x(x0), y(y0), z(z0) {}
  i32 x, y, z;
};

struct rect {
  rect(f32 x, f32 y, f32 w, f32 h) : box({x, y, w, h}) {}
  SDL_FRect box;
};

class renderer {
  public:
    renderer(const char *driver_name, i32 ww, i32 wh) : dname(driver_name), window_width(ww), window_height(wh), r(nullptr) {} 
    ~renderer() = default;

    void print_indice4(const std::vector<indice4> *indices);
    void print_indice3(const std::vector<indice3> *indices);
    void print_edges(const std::vector<edge> *edges);
   
    std::vector<indice3> quad_to_triangle(const std::vector<indice4> *indices);
    std::vector<edge> make_edges(const std::vector<indice4> *indices);
    std::vector<grid_pos> translate_vertices_z(const std::vector<grid_pos> *vertices, f32 dz);
    std::vector<grid_pos> rotate_vertices_xz(const std::vector<grid_pos> *vertices, f32 angle);
 

    grid_pos translate_z(grid_pos gpos, f32 inc);
    grid_pos translate_x(grid_pos gpos, f32 inc);
    grid_pos translate_y(grid_pos gpos, f32 inc);
    grid_pos rotate_xz(grid_pos gpos, f32 angle);
    grid_pos rotate_yz(grid_pos gpos, f32 angle);

    scr_pos to_screen(scr_pos p);
    scr_pos project(grid_pos gpos, f32 scale);

    SDL_Renderer *get_renderer(void) { return r; }
    SDL_Renderer *create(SDL_Window *w);
    

    std::vector<SDL_Vertex> vertices_convert_sdl_vertex(const std::vector<grid_pos>& vertices, const tri_spec& spec);
    veci32 indice3_flatten(const std::vector<indice3>& indices);

    void render_triangles(const std::vector<grid_pos>& vertices, const std::vector<indice3>& indices, const tri_spec spec);
    void render_wire_frame(const std::vector<edge> *edges, const std::vector<grid_pos> *vertices, const tri_spec spec);
    void set_point(scr_pos p);
    void draw_points(const std::vector<grid_pos> *vertices);
    void set_renderer(SDL_Renderer *ptr) { r = ptr; }
    void fill_rect(rect builder);
    void clear(void);
    void colour(u8 r8, u8 g8, u8 b8, u8 a8);
    void present(void);

  private:
    const char *dname;
    i32 window_width, window_height;
    SDL_Renderer *r;
};
