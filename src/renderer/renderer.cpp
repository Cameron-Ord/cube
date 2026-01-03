#include "renderer.hpp"
#include "../util.hpp"
#include <SDL3/SDL.h>
#include <cmath>
#include <iostream>

const f32 LEFT = -1.0f;
const f32 RIGHT = 1.0f;
const f32 TOP = 1.0f;
const f32 BOTTOM = -1.0f;
const f32 ZNEAR = 0.1f;
const f32 ZFAR = 10.0f;

constexpr f32 FOV = 60.0f * DEG2RAD;

f64 interpolate(const f64 &target, const f64 &current, const f64 &alpha){
  return (target - current) * alpha;
}

renderer::renderer(const char *driver_name, i32 ww, i32 wh, SDL_Color bg, SDL_Color smear, SDL_Color box) 
  : dname(driver_name), window_width(ww), window_height(wh), bg_col(bg), smear_col(smear), box_col(box) {}

SDL_FColor renderer::icol_to_fcol(SDL_Color icol){
  const f32 rf32 = static_cast<f32>(icol.r) / UINT8_MAX;
  const f32 gf32 = static_cast<f32>(icol.g) / UINT8_MAX;
  const f32 bf32 = static_cast<f32>(icol.b) / UINT8_MAX;
  const f32 af32 = static_cast<f32>(icol.a) / UINT8_MAX;

  return { rf32, gf32, bf32, af32 };
}

SDL_Color renderer::fcol_to_icol(SDL_FColor fcol){
  const u8 r8 = static_cast<u8>(fcol.r) * UINT8_MAX;
  const u8 g8 = static_cast<u8>(fcol.g) * UINT8_MAX;
  const u8 b8 = static_cast<u8>(fcol.b) * UINT8_MAX;
  const u8 a8 = static_cast<u8>(fcol.a) * UINT8_MAX;

  return { r8, g8, b8, a8 };
}

void renderer::update_draw_plane(const i32 &&width, const i32 &&height)
{
    window_width = width, window_height = height;
}

SDL_Renderer *renderer::create(SDL_Window *w)
{
    SDL_Renderer *tmp = SDL_CreateRenderer(w, dname);
    if (!tmp) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    return tmp;
}

std::vector<grid_pos> renderer::scale_vertices(const std::vector<grid_pos> &gpos, f32 scale)
{
    std::vector<grid_pos> scaled = std::vector<grid_pos>(gpos);
    for (auto it = scaled.begin(); it != scaled.end(); it++) {
        it->x *= scale;
        it->y *= scale;
        it->z *= scale;
    }
    return scaled;
}

void renderer::clear(void) { SDL_RenderClear(r); }

void renderer::colour(u8 r8, u8 g8, u8 b8, u8 a8) { SDL_SetRenderDrawColor(r, r8, g8, b8, a8); }

void renderer::present(void) { SDL_RenderPresent(r); }

void renderer::fill_rect(rect builder) { SDL_RenderFillRect(r, &builder.box); }

void renderer::render_triangles(
    const std::vector<grid_pos> &&vertices, const std::vector<indice3> &indices, const SDL_FColor &col)
{
    const std::vector<SDL_Vertex> sdl_vertices = vertices_convert_sdl_vertex(vertices, col);
    const veci32 flat_indices = indice3_flatten(indices);

    const i32 nvertices = static_cast<i32>(sdl_vertices.size());
    const i32 nindices = static_cast<i32>(flat_indices.size());

    SDL_RenderGeometry(r, nullptr, sdl_vertices.data(), nvertices, flat_indices.data(), nindices);
}

std::vector<SDL_Vertex>
renderer::vertices_convert_sdl_vertex(const std::vector<grid_pos> &vertices, const SDL_FColor &col)
{
    std::vector<SDL_Vertex> conv;
    for (auto it = vertices.begin(); it != vertices.end(); it++) {
        scr_pos pos = to_screen(project_pers(*it, (f32)window_width / window_height));
        conv.push_back({{pos.x, pos.y}, col, {0, 0}});
    }
    return conv;
}

veci32 renderer::indice3_flatten(const std::vector<indice3> &indices)
{
    veci32 flat;
    for (auto it = indices.begin(); it != indices.end(); it++) {
        flat.push_back(it->x);
        flat.push_back(it->y);
        flat.push_back(it->z);
    }
    return flat;
}

// makes dupes idc atm
std::vector<edge> renderer::make_edges(const std::vector<indice4> &indices)
{
    std::vector<edge> edges;
    for (size_t i = 0; i < indices.size(); i++) {
        const indice4 *ind = &indices[i];
        edges.push_back({ind->x, ind->y});
        edges.push_back({ind->y, ind->z});
        edges.push_back({ind->z, ind->k});
        edges.push_back({ind->k, ind->x});
    }
    return edges;
}

std::vector<indice3> renderer::quad_to_triangle(const std::vector<indice4> &indices)
{
    std::vector<indice3> triangles;
    for (size_t i = 0; i < indices.size(); i++) {
        const indice4 *ind = &indices[i];
        triangles.push_back({ind->x, ind->y, ind->z});
        triangles.push_back({ind->x, ind->z, ind->k});
    }
    return triangles;
}

void renderer::print_indice4(const std::vector<indice4> &indices)
{
    for (size_t i = 0; i < indices.size(); i++) {
        const indice4 *ind = &indices[i];
        std::cout << "{ " << ind->x << "," << ind->y << "," << ind->z << "," << ind->k << " }";
    }
    std::cout << std::endl;
}

void renderer::print_indice3(const std::vector<indice3> &indices)
{
    for (size_t i = 0; i < indices.size(); i++) {
        const indice3 *ind = &indices[i];
        std::cout << "{ " << ind->x << "," << ind->y << "," << ind->z << " }";
    }
    std::cout << std::endl;
}

void renderer::print_edges(const std::vector<edge> &edges)
{
    for (size_t i = 0; i < edges.size(); i++) {
        const edge *e = &edges[i];
        std::cout << "{ " << e->x << "," << e->y << " }";
    }
    std::cout << std::endl;
}

void renderer::draw_points(const std::vector<grid_pos> &vertices)
{
    for (auto it = vertices.begin(); it != vertices.end(); it++) {
        set_point(to_screen(project_pers(*it, (f32)window_width / window_height)));
    }
}

std::vector<grid_pos> renderer::rotate_vertices_yz(const std::vector<grid_pos> &&vertices, const f32 &angle)
{
    std::vector<grid_pos> rotated = std::vector<grid_pos>(vertices);
    for (size_t i = 0; i < vertices.size(); i++) {
        rotated[i] = rotate_yz(vertices[i], angle);
    }
    return rotated;
}

std::vector<grid_pos> renderer::rotate_vertices_xz(const std::vector<grid_pos> &&vertices, const f32 &angle)
{
    std::vector<grid_pos> rotated = std::vector<grid_pos>(vertices);
    for (size_t i = 0; i < vertices.size(); i++) {
        rotated[i] = rotate_xz(vertices[i], angle);
    }
    return rotated;
}


std::vector<grid_pos> renderer::translate_vertices_y(const std::vector<grid_pos> &&vertices, const f32 &dy){
    std::vector<grid_pos> translated = std::vector<grid_pos>(vertices);
    for (size_t i = 0; i < vertices.size(); i++) {
        translated[i] = translate_y(vertices[i], dy);
    }
    return translated;
}

std::vector<grid_pos> renderer::translate_vertices_x(const std::vector<grid_pos> &&vertices, const f32 &dx)
{
    std::vector<grid_pos> translated = std::vector<grid_pos>(vertices);
    for (size_t i = 0; i < vertices.size(); i++) {
        translated[i] = translate_x(vertices[i], dx);
    }
    return translated;
}

std::vector<grid_pos> renderer::translate_vertices_z(const std::vector<grid_pos> &&vertices, const f32 &dz)
{
    std::vector<grid_pos> translated = std::vector<grid_pos>(vertices);
    for (size_t i = 0; i < vertices.size(); i++) {
        translated[i] = translate_z(vertices[i], dz);
    }
    return translated;
}

void renderer::render_wire_frame(
    const std::vector<grid_pos> &&vertices, const std::vector<edge> &edges, const SDL_Color &col)
{
    colour(col.r, col.g, col.b, col.a);
    for (size_t i = 0; i < edges.size(); i++) {
        const edge *e = &edges[i];
        grid_pos v0 = vertices[e->x];
        grid_pos v1 = vertices[e->y];

        scr_pos p0 = to_screen(project_pers(v0, (f32)window_width / window_height));
        scr_pos p1 = to_screen(project_pers(v1, (f32)window_width / window_height));

        SDL_RenderLine(r, p0.x, p0.y, p1.x, p1.y);
    }
}

void renderer::set_point(scr_pos p)
{
    const f32 size = 8.0f;
    fill_rect(rect(p.x - size / 2, p.y - size / 2, size, size));
}

grid_pos renderer::translate_z(const grid_pos &gpos, const f32 &inc) { return grid_pos(gpos.x, gpos.y, gpos.z + inc); }

grid_pos renderer::translate_x(const grid_pos &gpos, const f32 &inc) { return grid_pos(gpos.x + inc, gpos.y, gpos.z); }

grid_pos renderer::translate_y(const grid_pos &gpos, const f32 &inc) { return grid_pos(gpos.x, gpos.y + inc, gpos.z); }

scr_pos renderer::to_screen(const scr_pos &&p)
{
    const f32 xnorm = (p.x + 1) / 2 * window_width;
    const f32 ynorm = (1.0f - (p.y + 1.0f) / 2) * window_height;
    return scr_pos(xnorm, ynorm);
}

scr_pos renderer::project_ortho(const grid_pos &gpos, const f32 &&aspect_ratio)
{
    const f32 dx = 2 * (gpos.x - LEFT) / (RIGHT - LEFT) - 1;
    const f32 dy = 2 * (gpos.y - BOTTOM) / (TOP - BOTTOM) - 1;
    return scr_pos(dx / aspect_ratio, dy);
}

scr_pos renderer::project_pers(const grid_pos &gpos, const f32 &&aspect_ratio)
{
    f32 f = 1.0f / tanf(FOV * 0.5f);

    const f32 dx = (gpos.x * f) / gpos.z;
    const f32 dy = (gpos.y * f) / gpos.z;

    return scr_pos(dx / aspect_ratio, dy);
}

scr_pos renderer::project(const grid_pos &gpos, const f32 &&aspect_ratio)
{
    const f32 dx = (gpos.x / gpos.z);
    const f32 dy = (gpos.y / gpos.z);
    return scr_pos(dx / aspect_ratio, dy);
}

grid_pos renderer::rotate_yz(const grid_pos &gpos, const f32 &angle)
{
    const f32 c = cos(angle);
    const f32 s = sin(angle);
    return grid_pos(gpos.x, gpos.y * c - gpos.z * s, gpos.y * s + gpos.z * c);
}

grid_pos renderer::rotate_xz(const grid_pos &gpos, const f32 &angle)
{
    const f32 update_x = gpos.x * cosf(angle) - gpos.z * sinf(angle);
    const f32 update_y = gpos.y;
    const f32 update_z = gpos.x * sinf(angle) + gpos.z * cosf(angle);

    return grid_pos(update_x, update_y, update_z);
}
