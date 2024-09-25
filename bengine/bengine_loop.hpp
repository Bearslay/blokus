#ifndef BENGINE_MAINLOOP_hpp
#define BENGINE_MAINLOOP_hpp

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#include "bengine_window.hpp"

namespace bengine {
    /// @brief A virtual class used to contain the basic looping mechanism required to seperate rendering/computing while maintaining consistent computational behavior
    class loop {
        protected:
            /// @brief How long the loop has been active (seconds)
            long double time = 0.0;
            /// @brief How long each computation frame should take (in seconds)
            double deltaTime = 0.01;

            /// @brief Whether the loop is running or not
            bool loopRunning = true;
            /// @brief Whether the renderer needs to update the visuals or not (saves on performance when nothing visual is happening)
            bool visualsChanged = true;

            /// @brief The window that is interacted with and displays everything
            bengine::window window = bengine::window("window", 1280, 720, SDL_WINDOW_SHOWN);
            /// @brief The SDL_Event structure used to process events
            SDL_Event event;
            /// @brief The state of the keyboard; good for instantaneous feedback on which keys are pressed and which aren't
            const Uint8 *keystate = SDL_GetKeyboardState(NULL);

            /// @brief A virtual function that will be called whenever there is an event that needs to be addressed
            virtual void handleEvent() = 0;
            /// @brief A virtual function that will be called each computation frame to handle any non-rendering-related tasks
            virtual void compute() = 0;
            /// @brief A virtual function that will be called each rendering frame to handle all of the rendering-related tasks
            virtual void render() = 0;

        public:
            /** bengine::loop constructor; mainly creates the window that will be used
             * @param title The title of the window being created
             * @param width The width of the window being created
             * @param height The height of the window being created
             * @param flags SDL2 flags to create the window with
             * @param imageInitFlags SDL2 image flags to initialize SDL_image with (-1 to not initialize)
             * @param useTTF Whether to initialize SDL_ttf or not
             */
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

                SDL_StopTextInput();
            }
            /// @brief bengine::loop deconstructor; pretty much just handles some SDL cleanup
            ~loop() {
                TTF_Quit();
                IMG_Quit();
                SDL_Quit();
            }

            /** The main function that handles the looping behavior and virtual function calling
             * @returns 0 (anything additional hasn't been added yet)
             */
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
                                    this->visualsChanged = true;
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

#endif // BENGINE_MAINLOOP_hpp
