#pragma once

#include "../alias.hpp"

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <vector>
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Window SDL_Window;

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
  renderer(const char *driver_name, i32 ww, i32 wh)
      : dname(driver_name), window_width(ww), window_height(wh), r(nullptr) {}
  ~renderer() = default;

  void print_indice4(const std::vector<indice4> &indices);
  void print_indice3(const std::vector<indice3> &indices);
  void print_edges(const std::vector<edge> &edges);

  // These functions are meant to be called with a vertices vector that has had
  // all its transformations done on it and then piped straight into the
  // function, so they take rvalue references
  void render_triangles(const std::vector<grid_pos> &&vertices,
                        const std::vector<indice3> &indices,
                        const SDL_FColor &col);
  void render_wire_frame(const std::vector<grid_pos> &&vertices,
                         const std::vector<edge> &edges, const SDL_Color &col);
  // Same idea here but its just considering the pipeline of
  // scale->rotate->transform->render, making copies every time would be
  // wasteful, but i mean is this optimization really necessary in this case?
  // who cares.
  std::vector<grid_pos>
  rotate_vertices_xz(const std::vector<grid_pos> &&vertices, const f32 &angle);
  std::vector<grid_pos>
  rotate_vertices_yz(const std::vector<grid_pos> &&vertices, const f32 &angle);
  std::vector<grid_pos>
  translate_vertices_z(const std::vector<grid_pos> &&vertices, const f32 &dz);
  std::vector<grid_pos>
  translate_vertices_x(const std::vector<grid_pos> &&vertices, const f32 &dz);
  scr_pos to_screen(const scr_pos &&p);

  std::vector<grid_pos> scale_vertices(const std::vector<grid_pos> &gpos,
                                       f32 scale);
  std::vector<indice3> quad_to_triangle(const std::vector<indice4> &indices);
  std::vector<edge> make_edges(const std::vector<indice4> &indices);

  grid_pos translate_z(const grid_pos &gpos, const f32 &inc);
  grid_pos translate_x(const grid_pos &gpos, const f32 &inc);
  grid_pos translate_y(const grid_pos &gpos, const f32 &inc);
  grid_pos rotate_xz(const grid_pos &gpos, const f32 &angle);
  grid_pos rotate_yz(const grid_pos &gpos, const f32 &angle);

  scr_pos project(const grid_pos &gpos, const f32&& aspect_ratio);
  scr_pos project_ortho(const grid_pos &gpos, const f32&& aspect_ratio);
  scr_pos project_pers(const grid_pos &gpos, const f32&& aspect_ratio);

  SDL_Renderer *get_renderer(void) { return r; }
  SDL_Renderer *create(SDL_Window *w);

  std::vector<SDL_Vertex>
  vertices_convert_sdl_vertex(const std::vector<grid_pos> &vertices,
                              const SDL_FColor &col);
  veci32 indice3_flatten(const std::vector<indice3> &indices);

  void set_point(scr_pos p);
  void draw_points(const std::vector<grid_pos> &vertices);
  void set_renderer(SDL_Renderer *ptr) { r = ptr; }
  void fill_rect(rect builder);
  void clear(void);
  void colour(u8 r8, u8 g8, u8 b8, u8 a8);
  void present(void);
  void update_draw_plane(const i32&& width, const i32&& height);

private:
  const char *dname;
  i32 window_width, window_height;
  SDL_Renderer *r;
};
