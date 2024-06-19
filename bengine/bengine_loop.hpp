#ifndef BENGINE_MAINLOOP_hpp
#define BENGINE_MAINLOOP_hpp

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#include "bengine_window.hpp"

namespace bengine {
    class loop {
        protected:
            long double time = 0.0;
            double deltaTime = 0.01;

            bool loopRunning = true;
            bool visualsChanged = true;

            bengine::window window = bengine::window("window", 1280, 720, SDL_WINDOW_SHOWN);
            SDL_Event event;
            const Uint8 *keystate = SDL_GetKeyboardState(NULL);

            virtual void handleEvent() = 0;
            virtual void compute() = 0;
            virtual void render() = 0;

        public:
            loop(const char* title = "window", const Uint16 &width = 1920, const Uint16 &height = 1080, const Uint32 &flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE, const int &imageInitFlags = IMG_INIT_PNG, const bool &useTTF = true) {
                if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                    std::cout << "Error initializing SDL2\nERROR: " << SDL_GetError() << "\n";
                }
                if (imageInitFlags != -1 && IMG_Init(imageInitFlags) != imageInitFlags) {
                    std::cout << "Error initializing SDL2_image\nERROR: " << IMG_GetError() << "\n";
                }
                if (useTTF && TTF_Init() == -1) {
                    std::cout << "Error initializing SDL2_ttf\nERROR: " << TTF_GetError() << "\n";
                }

                this->window.setTitle(title);
                this->window.setWidth(width);
                this->window.setHeight(height);
                this->window.setFlags(flags);
                this->window.setBaseWidth(width);
                this->window.setBaseHeight(height);
            }
            ~loop() {
                TTF_Quit();
                IMG_Quit();
                SDL_Quit();
            }

            int run() {
                Uint32 startTicks = 0;
                Uint32 frameTicks = 0;
                double currentTime = SDL_GetTicks() * 0.01;
                long double newTime = 0.0;
                long double frameTime = 0.0;
                double accumulator = 0.0;

                while (this->loopRunning) {
                    startTicks = SDL_GetTicks();
                    newTime = SDL_GetTicks() * 0.01;
                    frameTime = newTime - currentTime;
                    currentTime = newTime;
                    accumulator += frameTime;

                    while (accumulator >= this->deltaTime) {
                        while (SDL_PollEvent(&this->event)) {
                            switch (this->event.type) {
                                case SDL_QUIT:
                                    this->loopRunning = false;
                                    break;
                                case SDL_WINDOWEVENT:
                                    this->window.handleEvent(this->event.window);
                                    break;
                            }
                            this->handleEvent();
                        }

                        this->compute();

                        this->time += this->deltaTime;
                        accumulator -= this->deltaTime;
                    }

                    if (this->visualsChanged) {
                        this->visualsChanged = false;
                        this->window.clear();
                        this->render();
                        this->window.present();
                    }

                    if ((frameTicks = SDL_GetTicks() - startTicks) < (Uint32)(1000 / this->window.refreshRate())) {
                        SDL_Delay(1000 / this->window.refreshRate() - frameTicks);
                    }
                }
                return 0;
            }
    };
}

#endif /* BENGINE_MAINLOOP_hpp */
