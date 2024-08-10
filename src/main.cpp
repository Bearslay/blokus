#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#include "btils.hpp"
#include "bengine.hpp"
#include "blokus.hpp"

int main(int argc, char* args[]) {
    blokus::game g(20, 4, 1, 1, 1, 0);
    // blokus::game g;
    g.run();

    return 0;
}
