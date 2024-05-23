#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#include "RenderWindow.hpp"
#include "Utilities.hpp"
#include "CursorBox.hpp"

int main(int argc, char* args[]) {
    // Initializing SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {std::cout << "Error initializing SDL2\nERROR: " << SDL_GetError() << "\n";}
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {std::cout << "Error initializing SDL2_image\nERROR: " << IMG_GetError() << "\n";}
    if (TTF_Init() == -1) {std::cout << "Error initializing SDL2_ttf\nERROR: " << TTF_GetError() << "\n";}

    struct {
        Uint32 startTicks = 0;
        Uint32 frameTicks = 0;
        double currentTime = SDL_GetTicks() * 0.01;
        long double newTime = 0.0;
        long double frameTime = 0.0;
        double accumulator = 0.0;

        // Whether the program is running or not
        bool running = true;
        // Whether the program should re-render the window or not (based on whether visual changes were made during computation frames)
        bool madeChanges = true;
    }
    // Stuff related to the running of the base program, mainly in keeping consistent time and not overdoing rendering stuff
    MainLoop;

    struct {
        // Quit the program
        int Quit = SDL_SCANCODE_ESCAPE;
        // Either capture or release the mouse (mainly useful for 3D stuff tbh)
        int ToggleCapture = SDL_SCANCODE_F1;
    }
    // Names given to an SDL Scancode to make code easier to maintain/read
    Keybinds;

    RenderWindow Window("Blokus", 1280, 720);
    SDL_Event Event;
    MouseState mstate;
    const Uint8 *Keystate = SDL_GetKeyboardState(NULL);
    TTF_Font *font = TTF_OpenFont("dev/fonts/GNU-Unifont.ttf", 30);

    // Time since the program started
    long double t = 0.0;
    // Time each computation frame should take up when running optimally (0.01 = 100 fps, 0.01667 = 60 fps, etc)
    // Smaller values should produce more accurate simulations, but will require more processing power to keep the visual frames up-to-speed
    double dt = 0.01;
    while (MainLoop.running) {
        // Simulation stuff to (hopefully) keep time-sensitive things like physics deterministic between devices/sessions
        MainLoop.startTicks = SDL_GetTicks();
        MainLoop.newTime = SDL_GetTicks() * 0.01;
        MainLoop.frameTime = MainLoop.newTime - MainLoop.currentTime;
        MainLoop.currentTime = MainLoop.newTime;
        MainLoop.accumulator += MainLoop.frameTime;

        while (MainLoop.accumulator >= dt) {
            while (SDL_PollEvent(&Event)) {
                switch (Event.type) {
                    case SDL_QUIT:
                        MainLoop.running = false;
                        break;
                    case SDL_MOUSEMOTION:
                        // Update the mouse's position relative to the top-left corner and center of the window
                        mstate.Motion = true;
                        SDL_GetMouseState(&mstate.PosR.x, &mstate.PosR.y);
                        mstate.PosC = {mstate.PosR.x - Window.getW_2(), Window.getH() - mstate.PosR.y - Window.getH_2()};

                        // Quick updates for when the mouse is captured/released
                        if (!mstate.Released) {mstate.Rel = {Event.motion.xrel, -Event.motion.yrel};}
                        else {
                            mstate.Released = false;
                            mstate.Rel = {0, 0};
                        }

                        break;
                    case SDL_KEYDOWN:
                        if (!Event.key.repeat) {
                            // Capture/release mouse
                            if (Keystate[Keybinds.ToggleCapture]) {
                                mstate.Captured = !mstate.Captured;
                                if (mstate.Captured) {
                                    SDL_SetRelativeMouseMode(SDL_TRUE);
                                    mstate.Released = true;
                                    mstate.Rel = {0, 0};
                                } else {
                                    SDL_SetRelativeMouseMode(SDL_FALSE);
                                    Window.centerMouse();
                                    mstate.PosC = mstate.PosR = {0, 0};
                                }
                            }
                            // Quit program
                            if (Keystate[Keybinds.Quit]) {MainLoop.running = false;}
                        }
                        break;
                    case SDL_WINDOWEVENT:
                        Window.handleEvent(Event.window);
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        mstate.Pressed[Event.button.button] = true;
                        break;
                    case SDL_MOUSEBUTTONUP:
                        mstate.Pressed[Event.button.button] = false;
                        break;
                }
            }
            


            // End-of-computation-frame adjustments
            t += dt;
            MainLoop.accumulator -= dt;
            mstate.Motion = false;
        }

        // Only do a fresh render if changes were made to what would appear on-screen
        if (true) {
            MainLoop.madeChanges = false;
            Window.clear();

            Window.drawThickRectangle(-Window.getH_2() + 30, Window.getH_2() - 30, Window.getH() - 60, Window.getH() - 60, 15, THICKRECT_INNER, PresetColors[COLOR_WHITE]);

            Window.show();
        }

        // Limit the amount of renderings per second to the monitor's refresh rate
        if ((MainLoop.frameTicks = SDL_GetTicks() - MainLoop.startTicks) < (Uint32)(1000 / Window.getRefreshRate())) {SDL_Delay(1000 / Window.getRefreshRate() - MainLoop.frameTicks);}
    }

    // Cleanup
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0; 
}
