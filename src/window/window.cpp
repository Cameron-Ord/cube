#include "window.hpp"

#include <iostream>
#include <SDL3/SDL.h>

SDL_Window *window::create(){
  SDL_Window *tmp = SDL_CreateWindow(title, width, height, flags);
  if(!tmp){
    std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
    return nullptr;
  }
  return tmp;
}

