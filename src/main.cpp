#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#include "btils.hpp"
#include "bengine.hpp"
#include "blokus.hpp"

int main(int argc, char* args[]) {
    // struct {
    //     int Quit = SDL_SCANCODE_ESCAPE;
    //     int ToggleCapture = SDL_SCANCODE_F1;
    // }
    // Keybinds;

    // while (MainLoop.running) {
    //     while (MainLoop.accumulator >= dt) {
    //         while (SDL_PollEvent(&Event)) {
    //             switch (Event.type) {
    //                 case SDL_QUIT:
    //                     MainLoop.running = false;
    //                     break;
    //                 case SDL_MOUSEMOTION:
    //                     // // Update the mouse's position relative to the top-left corner and center of the window
    //                     // mstate.Motion = true;
    //                     // SDL_GetMouseState(&mstate.PosR.x, &mstate.PosR.y);
    //                     // mstate.PosC = {mstate.PosR.x - Window.getW_2(), Window.getH() - mstate.PosR.y - Window.getH_2()};

    //                     // // Quick updates for when the mouse is captured/released
    //                     // if (!mstate.Released) {mstate.Rel = {Event.motion.xrel, -Event.motion.yrel};}
    //                     // else {
    //                     //     mstate.Released = false;
    //                     //     mstate.Rel = {0, 0};
    //                     // }

    //                     break;
    //                 case SDL_KEYDOWN:
    //                     if (!Event.key.repeat) {
    //                         // // Capture/release mouse
    //                         // if (Keystate[Keybinds.ToggleCapture]) {
    //                         //     mstate.Captured = !mstate.Captured;
    //                         //     if (mstate.Captured) {
    //                         //         SDL_SetRelativeMouseMode(SDL_TRUE);
    //                         //         mstate.Released = true;
    //                         //         mstate.Rel = {0, 0};
    //                         //     } else {
    //                         //         SDL_SetRelativeMouseMode(SDL_FALSE);
    //                         //         Window.centerMouse();
    //                         //         mstate.PosC = mstate.PosR = {0, 0};
    //                         //     }
    //                         // }
    //                         // Quit program
    //                         if (Keystate[Keybinds.Quit]) {MainLoop.running = false;}
    //                     }
    //                     break;
    //             }
    //         }
    //     }
    // }

    blokus::game g;
    g.run();

    return 0;
}
