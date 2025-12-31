#pragma once

#include "../alias.hpp"
typedef struct SDL_Window SDL_Window;

class window {
public:
  window(const char *wtitle, i32 wwidth, i32 wheight, u64 wflags)
      : title(wtitle), width(wwidth), height(wheight), flags(wflags) {}
  ~window() = default;
  SDL_Window *create(void);
  void set_window(SDL_Window *ptr) { w = ptr; }
  SDL_Window *get_window(void) { return w; }

  void win_size_update(void);

  i32 get_width(void) { return width; }
  i32 get_height(void) { return height; }

private:
  const char *title;
  i32 width, height;
  u64 flags;
  SDL_Window *w;
};
