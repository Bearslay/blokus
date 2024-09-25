#ifndef BENGINE_WINDOW_hpp
#define BENGINE_WINDOW_hpp

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#include "bengine_texture.hpp"
#include "btils_main.hpp"

#define THICKSHAPE_INNER 0
#define THICKSHAPE_OUTER 1
#define THICKSHAPE_MIDDLE 2

#define RENDERTARGET_WINDOW 0
#define RENDERTARGET_DUMMY 1

namespace bengine {
    /// @brief List of 16 basic colors
    const SDL_Color colors[16] = {
        {  0,   0,   0, 255},
        {255, 255, 255, 255},
        {170, 170, 170, 255},
        { 85,  85,  85, 255},
        {117,  60,  19, 255},
        {255,   0,   0, 255},
        {115,   0,   0, 255},
        {255, 115,   0, 255},
        {255, 255,   0, 255},
        {  0, 255,   0, 255},
        {  0, 115,   0, 255},
        {  0, 255, 255, 255},
        {  0, 115, 115, 255},
        {  0,   0, 255, 255},
        {255,   0, 255, 255},
        {115,   0, 115, 255}
    };
    typedef enum {
        COLOR_BLACK,         // Black (0, 0, 0)
        COLOR_WHITE,         // White (255, 255, 255)
        COLOR_LIGHT_GRAY,    // Light Gray (170, 170, 170)
        COLOR_DARK_GRAY,     // Dark Gray (85, 85, 85)
        COLOR_BROWN,         // Brown (117, 60, 19)
        COLOR_RED,           // Red (255, 0, 0)
        COLOR_MAROON,        // Maroon (115, 0, 0)
        COLOR_ORANGE,        // Orange (255, 115, 0)
        COLOR_YELLOW,        // Yellow (255, 255, 0)
        COLOR_LIME,          // Lime (0, 255, 0)
        COLOR_GREEN,         // Green (0, 115, 0)
        COLOR_CYAN,          // Cyan (0, 255, 255)
        COLOR_TEAL,          // Teal (0, 115, 115)
        COLOR_BLUE,          // Blue (0, 0, 255)
        COLOR_MAGENTA,       // Magenta (255, 0, 255)
        COLOR_PURPLE         // Purple (115, 0, 115)
    } colorTitles;

    /// @brief A wrapper class that combines SDL_Window and SDL_Renderer while also replacing many other SDL2 functions
    class window {
        private:
            /// @brief The SDL_Window that the whole class is based around
            SDL_Window *win = NULL;
            /// @brief The SDL_Renderer that the whole class is based around
            SDL_Renderer *renderer = NULL;

            /// @brief The width of the window (px)
            Uint16 width;
            /// @brief The height of the window (px)
            Uint16 height;
            /// @brief Half of the width of the window (px) (useful for referencing the center of the window)
            Uint16 halfWidth;
            /// @brief Half of the height of the window (px) (useful for referencing the center of the window)
            Uint16 halfHeight;
            /// @brief The title of the window
            const char* title;

            /// @brief Whether the window is fullscreen or not
            bool isFullscreen = false;

            /// @brief Whether to lock the aspect ratio of the window for resizing (WIP)
            bool lockRatio = true;
            /// @brief Smallest possible width for the window (px)
            Uint8 ratioX;
            /// @brief Smallest possible height for the window (px)
            Uint8 ratioY;

            /// @brief Whether to stretch graphics to fill the entire window whenever the baseWidth and baseHeight of the window do not match the current width and height of the window
            bool stretchGraphics = true;
            /// @brief The base width of the window that will be used to determine the amount of horizontal stretching that will happen
            Uint16 baseWidth;
            /// @brief The base height of the window that will be used to determine the amount of vertical stretching that will happen
            Uint16 baseHeight;

            /// @brief The SDL_Texture that is used whenever the window's dummy texture is initialized and drawn to
            SDL_Texture *dummyTexture = NULL;
            /// @brief The SDL_PixelFormat that the window's dummy texture will use
            SDL_PixelFormat pixelFormat;
            /// @brief Whether the renderer is targeting the window (false) or the dummy texture
            bool renderTarget = RENDERTARGET_WINDOW;

            /** Pretty much does the same thing as SDL_SetRenderDrawColor, but will also print an error if something goes wrong
             * @param color The SDL_Color to change the renderer's color to
             * @returns 0 on success or a negative error code on failure
             */
            int changeDrawColor(const SDL_Color &color) {
                const int output = SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
                if (output != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to change renderer's drawing color";
                    bengine::window::printError();
                }
                return output;
            }
            /// @brief Print the output of SDL_GetError with a timestamp and some extra formatting
            void printError() const {
                std::cout << "\nERROR [" << SDL_GetTicks() << "]: " << SDL_GetError() << "\n";
            }
 
            /** Scale an x-value (that is assumed to be) relative to the window's base width to one relative to the window's current width
             * @param x An x-value relative to the window's base width (px)
             * @returns The inputted x-value scaled to the window's current width (px)
             */
            int stretchX(const int &x) const {
                return (int)(this->width * x / this->baseWidth);
            }
            /** Scale a y-value (that is assumed to be) relative to the window's base height to one relative to the window's current height
             * @param y A y-value relative to the window's base height (px)
             * @returns The inputted y-value scaled to the window's current height (px)
             */
            int stretchY(const int &y) const {
                return (int)(this->height * y / this->baseHeight);
            }

        public:
            /** bengine::window constructor
             * @param title The title for the window
             * @param width The width for the window (px)
             * @param height The height for the window (px)
             * @param flags SDL_WINDOW flags that modify how the window will behave as a Uint32 mask
             */
            window(const char* title = "window", const Uint16 &width = 1920, const Uint16 &height = 1080, const Uint32 &flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE) {
                if ((this->win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags)) == NULL) {
                    std::cout << "Window \"" << title << "\" failed to initialize";
                    bengine::window::printError();
                }
                if ((this->renderer = SDL_CreateRenderer(this->win, -1, SDL_RENDERER_ACCELERATED)) == NULL) {
                    std::cout << "Window \"" << title << "\" failed to initialize renderer";
                    bengine::window::printError();
                }

                this->width = width;
                this->height = height;
                bengine::window::updateDims();
                this->baseWidth = this->width;
                this->baseHeight = this->height;

                Uint16 gcd = btils::gcd<Uint16>(this->width, this->height);
                this->ratioX = this->width / gcd;
                this->ratioY = this->height / gcd;

                this->setPixelFormat();
            }
            /// @brief bengine::window deconstructor
            ~window() {
                SDL_DestroyRenderer(this->renderer);
                SDL_DestroyWindow(this->win);
                this->renderer = nullptr;
                this->win = nullptr;
            }

            /** Get the refresh rate of the monitor that the window was created on
             *@returns The refresh rate of the monitor that the window was created on
             */ 
            Uint16 refreshRate() const {
                SDL_DisplayMode mode;
                int displayIndex = SDL_GetWindowDisplayIndex(this->win);
                if (displayIndex < 0) {
                    std::cout << "Window \"" << this->title << "\" failed to fetch display index";
                    bengine::window::printError();
                }
                if (SDL_GetDisplayMode(displayIndex, 0, &mode) != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to fetch display mode information";
                    bengine::window::printError();
                }
                return (Uint16)mode.refresh_rate;
            }
            /** Get the SDL_WINDOW flags currently associated with the window
             * @returns The SDL_WINDOW flags currently associated with the window as a Uint32 mask
             */ 
            Uint32 windowFlags() const {
                return SDL_GetWindowFlags(this->win);
            }
            /** Set the SDL_WINDOW flags that the window should have; conflicting flags are dealt with here
             * @param flags A Uint32 mask that represents the flags the window should have
             */ 
            void setFlags(const Uint32 &flags) {
                if ((flags & SDL_WINDOW_FULLSCREEN) == SDL_WINDOW_FULLSCREEN && (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) != SDL_WINDOW_FULLSCREEN_DESKTOP) {
                    bengine::window::makeFullscreen(true);
                } else if ((flags & SDL_WINDOW_FULLSCREEN_DESKTOP) == SDL_WINDOW_FULLSCREEN_DESKTOP && (flags & SDL_WINDOW_FULLSCREEN) != SDL_WINDOW_FULLSCREEN) {
                    bengine::window::makeFullscreen(false);
                } else if ((flags & SDL_WINDOW_FULLSCREEN) != SDL_WINDOW_FULLSCREEN && (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) != SDL_WINDOW_FULLSCREEN_DESKTOP) {
                    bengine::window::makeWindowed();
                }
                if ((flags & SDL_WINDOW_SHOWN) == SDL_WINDOW_SHOWN && (flags & SDL_WINDOW_HIDDEN) != SDL_WINDOW_HIDDEN) {
                    bengine::window::show();
                } else if ((flags & SDL_WINDOW_HIDDEN) == SDL_WINDOW_HIDDEN && (flags & SDL_WINDOW_SHOWN) != SDL_WINDOW_SHOWN) {
                    bengine::window::hide();
                }
                if ((flags & SDL_WINDOW_BORDERLESS) == SDL_WINDOW_BORDERLESS) {
                    bengine::window::makeBorderless();
                } else {
                    bengine::window::makeBordered();
                }
                if ((flags & SDL_WINDOW_RESIZABLE) == SDL_WINDOW_RESIZABLE) {
                    bengine::window::unlockSize();
                } else {
                    bengine::window::lockSize();
                }
                if ((flags & SDL_WINDOW_MINIMIZED) == SDL_WINDOW_MINIMIZED && (flags & SDL_WINDOW_MAXIMIZED) != SDL_WINDOW_MAXIMIZED) {
                    bengine::window::minimize();
                } else if ((flags & SDL_WINDOW_MAXIMIZED) == SDL_WINDOW_MAXIMIZED && (flags & SDL_WINDOW_MINIMIZED) != SDL_WINDOW_MINIMIZED) {
                    bengine::window::maximize();
                }
                if ((flags & SDL_WINDOW_MOUSE_GRABBED) == SDL_WINDOW_MOUSE_GRABBED) {
                    bengine::window::grabMouse();
                } else {
                    bengine::window::releaseMouse();
                }
                if ((flags & SDL_WINDOW_ALWAYS_ON_TOP) == SDL_WINDOW_ALWAYS_ON_TOP) {
                    bengine::window::setTop();
                } else {
                    bengine::window::releaseTop();
                }
            }
            /// @brief Lock the window's current size (removes the SDL_WINDOW_RESIZABLE flag from the window)
            void lockSize() {
                SDL_SetWindowResizable(this->win, SDL_FALSE);
            }
            /// @brief Unlock the window's current size (adds the SDL_WINDOW_RESIZABLE flag to the window)
            void unlockSize() {
                SDL_SetWindowResizable(this->win, SDL_TRUE);
            }
            /// @brief Show the window (adds the SDL_WINDOW_SHOWN flag to the window)
            void show() {
                SDL_ShowWindow(this->win);
            }
            /// @brief Hide the window (adds the SDL_WINDOW_HIDDEN flag to the window)
            void hide() {
                SDL_HideWindow(this->win);
            }
            /// @brief Remove borders from the window (removes the SDL_WINDOW_BORDERLESS flag from the window)
            void makeBorderless() {
                SDL_SetWindowBordered(this->win, SDL_FALSE);
            }
            /// @brief Add borders to the window (adds the SDL_WINDOW_BORDERLESS flag to the window)
            void makeBordered() {
                SDL_SetWindowBordered(this->win, SDL_TRUE);
            }
            /// @brief Minimize the window to an iconic representation (adds the SDL_WINDOW_MINIMIZED flag to the window)
            void minimize() {
                SDL_MinimizeWindow(this->win);
            }
            /// @brief Maximize the window to as large of a size as possible (adds the SDL_WINDOW_MAXIMIZED flag to the window)
            void maximize() {
                SDL_MaximizeWindow(this->win);
            }
            /// @brief Confine the mouse to the window (adds the SDL_WINDOW_MOUSE_GRABBED flag to the window)
            void grabMouse() {
                SDL_SetWindowGrab(this->win, SDL_TRUE);
            }
            /// @brief Release the mouse from the confines of the window (removes the SDL_WINDOW_MOUSE_GRABBED flag from the window)
            void releaseMouse() {
                SDL_SetWindowGrab(this->win, SDL_FALSE);
            }
            /// @brief Set this window to always be visible/on top (adds the SDL_WINDOW_ALWAYS_ON_TOP flag to the window)
            void setTop() {
                SDL_SetWindowAlwaysOnTop(this->win, SDL_TRUE);
            }
            /// @brief Allow this window to be ordered in any configuration (removes the SDL_WINDOW_ALWAYS_ON_TOP flag from the window)
            void releaseTop() {
                SDL_SetWindowAlwaysOnTop(this->win, SDL_FALSE);
            }
            /** Make the window fullscreen in one of two ways
             * @param trueFullscreen Whether to make the window fullscreen while taking advantage of fullscreen-exclusive benefits (true) or not
             * @returns Whether the window used to be fullscreened or not
             */
            bool makeFullscreen(const bool &trueFullscreen = true) {
                const bool output = this->isFullscreen;
                if (trueFullscreen) {
                    SDL_SetWindowFullscreen(this->win, SDL_WINDOW_FULLSCREEN);
                } else {
                    SDL_SetWindowFullscreen(this->win, SDL_WINDOW_FULLSCREEN_DESKTOP);
                }
                this->isFullscreen = true;
                bengine::window::updateDims();
                return output;
            }
            /** Make the window windowed
             * @returns Whether the window used to be fullscreened or not
             */
            bool makeWindowed() {
                const bool output = this->isFullscreen;
                SDL_SetWindowFullscreen(this->win, SDL_FALSE);
                this->isFullscreen = false;
                bengine::window::updateDims();
                return output;
            }
            /** Toggle whether the window should be fullscreened or not
             * @param trueFullscreen Whether to make the window fullscreen while taking advantage of fullscreen-exclusive benefits (true) or not
             * @returns Whether the window used to be fullscreened or not
             */
            bool toggleFullscreen(const bool &trueFullscreen = true) {
                const bool output = this->isFullscreen;
                if (this->isFullscreen) {
                    bengine::window::makeWindowed();
                } else {
                    bengine::window::makeFullscreen(trueFullscreen);
                }
                return output;
            }

            /** Get the width of the window (px)
             * @returns The width of the window (px)
             */
            Uint16 getWidth() const {
                return this->width;
            }
            /** Set the width of the window (px)
             * @param width The new width of the window (px)
             * @returns The old width of the window (px)
             */
            Uint16 setWidth(const Uint16 &width) {
                const Uint16 output = this->width;
                this->width = width;
                this->halfWidth = this->width / 2;
                SDL_SetWindowSize(this->win, this->width, this->height);
                return output;
            }

            /** Get the height of the window (px)
             * @returns The height of the window (px)
             */
            Uint16 getHeight() const {
                return this->height;
            }
            /** Set the height of the window (px)
             * @param height The new height of the window (px)
             * @returns The old height of the window (px)
             */
            Uint16 setHeight(const Uint16 &height) {
                const Uint16 output = this->height;
                this->height = height;
                this->halfHeight = this->height / 2;
                SDL_SetWindowSize(this->win, this->width, this->height);
                return output;
            }

            /** Get half of the width of the window (px)
             * @returns Half of the width of the window (px)
             */
            Uint16 getHalfWidth() const {
                return this->halfWidth;
            }
            /** Get half of the height of the window (px)
             * @returns Half of the height of the window (px)
             */
            Uint16 getHalfHeight() const {
                return this->halfHeight;
            }

            /** Get the base width of the window (px) that will be used to determine the amount of horizontal stretching that will happen
             * @returns The base width of the window (px) that will be used to determine the amount of horizontal stretching that will happen
             */
            Uint16 getBaseWidth() const {
                return this->baseWidth;
            }
            /** Set the base width of the window (px) that will be used to determine the amount of horizontal stretching that will happen
             * @param width The new base width of the window (px) that will be used to determine the amount of horizontal stretching that will happen
             * @returns The old base width of the window (px) that was used to determine the amount of horizontal stretching that happened
             */
            Uint16 setBaseWidth(const Uint16 &width) {
                return btils::set<Uint16>(this->baseWidth, width);
            }

            /** Get the base height of the window (px) that will be used to determine the amount of vertical stretching that will happen
             * @returns The base height of the window (px) that will be used to determine the amount of vertical stretching that will happen
             */
            Uint16 getBaseHeight() const {
                return this->height;
            }
            /** Set the base height of the window (px) that will be used to determine the amount of vertical stretching that will happen
             * @param height The new base height of the window (px) that will be used to determine the amount of vertical stretching that will happen
             * @returns The old base height of the window (px) that was used to determine the amount of vertical stretching that happened
             */
            Uint16 setBaseHeight(const Uint16 &height) {
                return btils::set<Uint16>(this->baseHeight, height);
            }

            /** Get whether the window will stretch graphics based off of a base width/height or not
             * @returns Whether the window will stretch graphics based off of a base width/height or not
             */
            bool stretchesGraphics() const {
                return this->stretchGraphics;
            }
            /** Set whether the window will stretch graphics based off of a base width/height or not
             * @param stretchGraphics Whether the window will stretch graphics based off of a base width/height or not
             * @returns Whether the window used to stretch graphics based off of a base width/height or not
             */
            bool setGraphicalStretching(const bool &stretchGraphics) {
                const bool output = this->stretchGraphics;
                this->stretchGraphics = stretchGraphics;
                return output;
            }
            /** Toggle whether the window will stretch graphics based off of a base width/height or not
             * @returns Whether the window used to stretch graphics based off of a base width/height or not
             */
            bool toggleGraphicalStretching() {
                const bool output = this->stretchGraphics;
                this->stretchGraphics = !this->stretchGraphics;
                return output;
            }

            /** Get the window's title as a C-style string
             * @returns The window's title as a C-style string
             */
            const char* getTitle() const {
                return this->title;
            }
            /** Set the window's title as a C-style string
             * @param title The window's new title as a C-style string
             * @returns The window's old title as a C-style string
             */
            const char* setTitle(const char* title) {
                const char* output = this->title;
                SDL_SetWindowTitle(this->win, title);
                this->title = title;
                return output;
            }

            /** Clear the renderer
             * @param color The color to make the newly blank screen as an SDL_Color
             */
            void clear(const SDL_Color &color = bengine::colors[COLOR_BLACK]) {
                bengine::window::changeDrawColor(color);
                if (SDL_RenderClear(this->renderer) != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to clear renderer";
                    bengine::window::printError();
                }
            }
            /// @brief Present the renderer's buffer to the window to see
            void present() {
                SDL_RenderPresent(this->renderer);
            }

            /** Syncronize the window's dimensions held in the class with what is actually present in SDL2
             * @returns The window's width and height prior to updating (px) (these values will not necessarily be different after this function call)
             */
            SDL_Point updateDims() {
                const SDL_Point output = {this->width, this->height};
                
                int w, h;
                SDL_GetWindowSize(this->win, &w, &h);

                this->width = (Uint16)w;
                this->height = (Uint16)h;
                this->halfWidth = this->width / 2;
                this->halfHeight = this->height / 2;

                return output;
            }
            /** Handles the general behavior that windows should have when certain events trigger (for now, just window resizing)
             * @param event The SDL_WindowEvent to handle
             */
            void handleEvent(const SDL_WindowEvent &event) {
                switch (event.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        if (this->lockRatio) {
                            int w, h;
                            SDL_GetWindowSize(this->win, &w, &h);
                            
                            if (std::abs(this->width - w) / this->ratioX > std::abs(this->height - h) / this->ratioY) {
                                SDL_SetWindowSize(this->win, (int)(h * (double)(this->ratioX / this->ratioY)), h);
                            } else {
                                SDL_SetWindowSize(this->win, w, (int)(w * (double)(this->ratioY / this->ratioX)));
                            }
                        }
                        bengine::window::updateDims();
                        break;
                }
            }
            /// @brief Center the mouse within the window
            void centerMouse() {
                SDL_WarpMouseInWindow(this->win, this->halfWidth, this->halfHeight);
            }
            /** Set the current mouse's position to wherever specified and relative to the window
             * @param x The x-position for the mouse's new position relative to the window (px)
             * @param y The y-position for the mouse's new position relative to the window (px)
             */
            void positionMouse(const Uint16 &x, const Uint16 &y) {
                SDL_WarpMouseInWindow(this->win, x, y);
            }

            /** Draw a singular pixel
             * @param x x-position of the pixel to change relative to the window
             * @param y y-position of the pixel to change relative to the window
             * @param color The color to change the pixel to as an SDL_Color
             */
            void drawPixel(const int &x, const int &y, const SDL_Color &color = bengine::colors[bengine::COLOR_WHITE]) {
                bengine::window::changeDrawColor(color);

                if (this->stretchGraphics) {
                    if (SDL_RenderDrawPoint(this->renderer, bengine::window::stretchX(x), bengine::window::stretchY(y)) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to draw a pixel";
                        bengine::window::printError();
                    }
                    return;
                }
                if (SDL_RenderDrawPoint(this->renderer, x, y) != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to draw a pixel";
                    bengine::window::printError();
                }
            }
            /** Draw a line; includes minor optimizations for totally horizontal/vertical lines and single points
             * @param x1 x-position of the starting point relative to the window (px)
             * @param y1 y-position of the starting point relative to the window (px)
             * @param x2 x-position of the ending point relative to the window (px)
             * @param y2 y-position of the ending point relative to the window (px)
             * @param color The color to draw the line with as an SDL_Color
             */
            void drawLine(const int &x1, const int &y1, const int &x2, const int &y2, const SDL_Color &color = bengine::colors[bengine::COLOR_WHITE]) {
                if (x1 == x2 && y1 == y2) {
                    bengine::window::drawPixel(x1, y1, color);
                    return;
                }

                if (this->stretchGraphics) {
                    if (x1 == x2) {
                        bengine::window::drawRectangle(bengine::window::stretchX(x1), bengine::window::stretchY(y1), 1, bengine::window::stretchY(y2) - bengine::window::stretchY(y1), color);
                        return;
                    }
                    if (y1 == y2) {
                        bengine::window::drawRectangle(bengine::window::stretchX(x1), bengine::window::stretchY(y1), bengine::window::stretchX(x2) - bengine::window::stretchX(x1), 1, color);
                        return;
                    }

                    bengine::window::changeDrawColor(color);
                    if (SDL_RenderDrawLine(this->renderer, bengine::window::stretchX(x1), bengine::window::stretchY(y1), bengine::window::stretchX(x2), bengine::window::stretchY(y2)) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to draw a line";
                        bengine::window::printError();
                    }
                    return;
                }
                if (x1 == x2) {
                    bengine::window::drawRectangle(x1, y1, 1, y2 - y1, color);
                    return;
                }
                if (y1 == y2) {
                    bengine::window::drawRectangle(x1, y1, x2 - x1, 1, color);
                    return;
                }

                bengine::window::changeDrawColor(color);
                if (SDL_RenderDrawLine(this->renderer, x1, y1, x2, y2) != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to draw a line";
                    bengine::window::printError();
                }
            }

            /** Draw a rectangle (not filled, will only draw the perimeter)
             * @param x x-position of the top-left corner relative to the window (px) (assuming positive width and height)
             * @param y y-position of the top-left corner relative to the window (px) (assuming positive width and height)
             * @param w Width of the rectangle (px) (can be negative, thereby making the "x" parameter reference the right side of the rectangle)
             * @param h Height of the rectangle (px) (can be negative, thereby making the "y" parameter reference the bottom side of the rectangle)
             * @param color The color to draw the rectangle with as an SDL_Color
             */
            void drawRectangle(const int &x, const int &y, const int &w, const int &h, const SDL_Color &color = bengine::colors[bengine::COLOR_WHITE]) {
                bengine::window::changeDrawColor(color);
                
                if (this->stretchGraphics) {
                    const SDL_Rect dst = {x, y, w, h};
                    if (SDL_RenderDrawRect(this->renderer, &dst) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to draw a rectangle";
                        bengine::window::printError();
                    }
                    return;
                }
                const SDL_Rect dst = {bengine::window::stretchX(x), bengine::window::stretchY(y), bengine::window::stretchX(w), bengine::window::stretchY(h)};
                if (SDL_RenderDrawRect(this->renderer, &dst) != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to draw a rectangle";
                    bengine::window::printError();
                }
            }
            /** Draw a rectangle with a specified edge thickness (not filled, will only draw the perimeter at the set thickness)
             * @param x x-position of the top-left corner relative to the window (px) (assuming positive width and height)
             * @param y y-position of the top-left corner relative to the window (px) (assuming positive width and height)
             * @param w Width of the rectangle (px) (can be negative, thereby making the "x" parameter reference the right side of the rectangle)
             * @param h Height of the rectangle (px) (can be negative, thereby making the "y" parameter reference the bottom side of the rectangle)
             * @param thickness The thickness of the rectangle's sides (px)
             * @param mode The mode to use when thickening the lines; THICKSHAPE_INNER keeps the same width/height, THICKSHAPE_OUTER adds 2*thickness to the width/height, and THICKSHAPE_MIDDLE adds 1*thickness to the width/height
             * @param color The color to draw the rectangle with as an SDL_Color
             */
            void drawThickRectangle(const int &x, const int &y, const int &w, const int &h, const int &thickness, const unsigned char &mode = THICKSHAPE_INNER, const SDL_Color &color = bengine::colors[bengine::COLOR_WHITE]) {
                bengine::window::changeDrawColor(color);

                SDL_Rect rect[4];
                switch (mode) {
                    default:
                    case THICKSHAPE_INNER:
                        rect[0] = {x, y, w, thickness};
                        rect[1] = {x, y + h - thickness, w, thickness};
                        rect[2] = {x, y + thickness, thickness, h - thickness * 2};
                        rect[3] = {x + w - thickness, y + thickness, thickness, h - thickness * 2};
                        break;
                    case THICKSHAPE_OUTER:
                        rect[0] = {x - thickness, y - thickness, w + thickness * 2, thickness};
                        rect[1] = {x - thickness, y + h, w + thickness * 2, thickness};
                        rect[2] = {x - thickness, y, thickness, h};
                        rect[3] = {x + w, y, thickness, h};
                        break;
                    case THICKSHAPE_MIDDLE:
                        rect[0] = {x - thickness / 2, y - thickness / 2, w + thickness, thickness};
                        rect[1] = {x - thickness / 2, y + h - thickness / 2, w + thickness, thickness};
                        rect[2] = {x - thickness / 2, y + thickness / 2, thickness, h - thickness};
                        rect[3] = {x + w - thickness / 2, y + thickness / 2, thickness, h - thickness};
                        break;
                }

                if (this->stretchGraphics) {
                    for (Uint8 i = 0; i < 4; i++) {
                        rect[i].x = bengine::window::stretchX(rect[i].x);
                        rect[i].y = bengine::window::stretchY(rect[i].y);
                        rect[i].w = bengine::window::stretchX(rect[i].w);
                        rect[i].h = bengine::window::stretchY(rect[i].h);
                    }
                }

                if (SDL_RenderFillRect(this->renderer, &rect[0]) != 0 || SDL_RenderFillRect(this->renderer, &rect[1]) != 0 || SDL_RenderFillRect(this->renderer, &rect[2]) != 0 || SDL_RenderFillRect(this->renderer, &rect[3]) != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to draw a thick rectangle";
                    bengine::window::printError();
                }
            }
            /** Fill a rectangle
             * @param x x-position of the top-left corner relative to the window (px) (assuming positive width and height)
             * @param y y-position of the top-left corner relative to the window (px) (assuming positive width and height)
             * @param w Width of the rectangle (px) (can be negative, thereby making the "x" parameter reference the right side of the rectangle)
             * @param h Height of the rectangle (px) (can be negative, thereby making the "y" parameter reference the bottom side of the rectangle)
             * @param color The color to fill the rectangle with as an SDL_Color
             */
            void fillRectangle(const int &x, const int &y, const int &w, const int &h, const SDL_Color &color = bengine::colors[bengine::COLOR_WHITE]) {
                bengine::window::changeDrawColor(color);

                if (this->stretchGraphics) {
                    const SDL_Rect dst = {x, y, w, h};
                    if (SDL_RenderFillRect(this->renderer, &dst) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to fill a rectangle";
                        bengine::window::printError();
                    }
                    return;
                }
                const SDL_Rect dst = {bengine::window::stretchX(x), bengine::window::stretchY(y), bengine::window::stretchX(w), bengine::window::stretchY(h)};
                if (SDL_RenderFillRect(this->renderer, &dst) != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to fill a rectangle";
                    bengine::window::printError();
                }
            }
            /** Draw a circle (not filled, will only draw the perimeter at the set thickness; DOES NOT GET STRETCHED)
             * @param x x-position of the center of the circle relative to the window (px)
             * @param y y-position of the center of the circle relative to the window (px)
             * @param r Radius of the circle (px)
             * @param color The color to draw the circle with as an SDL_Color
             */
            void drawCircle(const int &x, const int &y, const int &r, const SDL_Color &color = bengine::colors[bengine::COLOR_WHITE]) {
                SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
                const int diameter = r * 2;
                int ox = r - 1;
                int oy = 0;
                int tx = 1;
                int ty = 1;
                int error = tx - diameter;
                while (ox >= oy) {
                    SDL_RenderDrawPoint(this->renderer, x + ox, y - oy);
                    SDL_RenderDrawPoint(this->renderer, x + ox, y + oy);
                    SDL_RenderDrawPoint(this->renderer, x - ox, y - oy);
                    SDL_RenderDrawPoint(this->renderer, x - ox, y + oy);
                    SDL_RenderDrawPoint(this->renderer, x + oy, y - ox);
                    SDL_RenderDrawPoint(this->renderer, x + oy, y + ox);
                    SDL_RenderDrawPoint(this->renderer, x - oy, y - ox);
                    SDL_RenderDrawPoint(this->renderer, x - oy, y + ox);
                    if (error <= 0) {
                        oy++;
                        error += ty;
                        ty += 2;
                    } else if (error > 0) {
                        ox--;
                        tx += 2;
                        error += tx - diameter;
                    }
                }
            }
            /** Fill a circle (DOES NOT GET STRETCHED)
             * @param x x-position of the center of the circle relative to the window (px)
             * @param y y-position of the center of the circle relative to the window (px)
             * @param r Radius of the circle (px)
             * @param color The color to fill the circle with as an SDL_Color
             */
            void fillCircle(const int &x, const int &y, const int &r, const SDL_Color &color = bengine::colors[bengine::COLOR_WHITE]) {
                SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
                int ox = 0;
                int oy = r;
                int error = r - 1;
                while (oy >= ox) {
                    SDL_RenderDrawLine(this->renderer, x - oy, y + ox, x + oy, y + ox);
                    SDL_RenderDrawLine(this->renderer, x - ox, y + oy, x + ox, y + oy);
                    SDL_RenderDrawLine(this->renderer, x - ox, y - oy, x + ox, y - oy);
                    SDL_RenderDrawLine(this->renderer, x - oy, y - ox, x + oy, y - ox);
                    if (error >= ox * 2) {
                        error -= ox * 2 + 1;
                        ox++;
                    } else if (error < 2 * (r - oy)) {
                        error += oy * 2 - 1;
                        oy--;
                    } else {
                        error += 2 * (oy - ox - 1);
                        oy--;
                        ox++;
                    }
                }
            }

            /** Load an SDL_Texture using the window's renderer
             * @param filepath The path to the file to load in as an SDL_Texture
             * @returns An SDL_Texture of the image file located at filepath
             */
            SDL_Texture *loadTexture(const char* filepath) {
                SDL_Texture *output = NULL;
                if ((output = IMG_LoadTexture(this->renderer, filepath)) == NULL) {
                    std::cout << "Window \"" << this->title << "\" failed to load texture";
                    bengine::window::printError();
                }
                return output;
            }

            /** Get the pixelformat that the window's dummy texture uses
             * @returns The pixelformat that the window's dummy texture uses
             */
            SDL_PixelFormat getPixelFormat() const {
                return this->pixelFormat;
            }
            /** Automatically set the pixelformat that the window's dummy texture uses
             * @returns The old pixelformat that the window's dummy texture uses
             */
            SDL_PixelFormat setPixelFormat() {
                const SDL_PixelFormat output = this->pixelFormat;
                SDL_RendererInfo info;
                if (SDL_GetRendererInfo(this->renderer, &info) != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to get a valid pixel format";
                    bengine::window::printError();
                    this->pixelFormat.format = SDL_PIXELFORMAT_UNKNOWN;
                } else {
                    this->pixelFormat.format = info.texture_formats[0];
                }
                return output;
            }

            /** Initialize the dummy texture for the ability to render to textures rather than just the window
             * @param width The width of the dummy texture (px)
             * @param height The height of the dummy texture (px)
             */
            int initDummy(const Uint16 &width, const Uint16 &height) {
                if (this->pixelFormat.format == SDL_PIXELFORMAT_UNKNOWN) {
                    bengine::window::setPixelFormat();
                }
                this->dummyTexture = SDL_CreateTexture(this->renderer, this->pixelFormat.format, SDL_TEXTUREACCESS_TARGET, width, height);
                if (this->dummyTexture == NULL) {
                    std::cout << "Window \"" << this->title << "\" failed to create dummy texture";
                    bengine::window::printError();
                    return -1;
                }
                if (this->renderTarget == RENDERTARGET_DUMMY) {
                    bengine::window::targetDummy();
                } else {
                    bengine::window::targetWindow();
                }
                return 0;
            }
            /** Target the renderer at the dummy texture
             * @returns 0 on success or a negative error code on failure
             */
            int targetDummy() {
                const int output = SDL_SetRenderTarget(this->renderer, this->dummyTexture);
                if (output != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to switch the rendering target to the dummy texture";
                    bengine::window::printError();
                } else {
                    this->renderTarget = RENDERTARGET_DUMMY;
                }
                return output;
            }
            /** Target the renderer at the window
             * @returns 0 on success or a negative error code on failure
             */
            int targetWindow() {
                const int output = SDL_SetRenderTarget(this->renderer, NULL);
                if (output != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to switch the rendering target to the window";
                    bengine::window::printError();
                } else {
                    this->renderTarget = RENDERTARGET_WINDOW;
                }
                return output;
            }
            /** Copy the dummy texture onto another texture (has a few ramifications but should be fine overall)
             * @returns An SDL_Texture that reflects the dummy texture
             */
            SDL_Texture* copyDummy() {
                int w, h;
                SDL_BlendMode blendmode;

                SDL_QueryTexture(this->dummyTexture, NULL, NULL, &w, &h);
                SDL_GetTextureBlendMode(this->dummyTexture, &blendmode);

                SDL_Texture* output = SDL_CreateTexture(this->renderer, this->pixelFormat.format, SDL_TEXTUREACCESS_TARGET, w, h);
                SDL_SetTextureBlendMode(output, SDL_BLENDMODE_NONE);
                
                SDL_SetRenderTarget(this->renderer, output);
                bengine::window::clear();
                SDL_RenderCopy(this->renderer, this->dummyTexture, NULL, NULL);
                bengine::window::present();
                SDL_SetTextureBlendMode(output, blendmode);

                if (this->renderTarget == RENDERTARGET_WINDOW) {
                    bengine::window::targetWindow();
                } else {
                    bengine::window::targetDummy();
                }
                return output;
            }

            /** Render an SDL_Texture
             * @param texture The SDL_Texture to render
             * @param src The portion of the SDL_Texture to copy and render (px for all 4 metrics)
             * @param dst The portion of the window/dummy texture to copy to (px for all 4 metrics) (will stretch the texture to fill the given rectangle)
             */
            void renderSDLTexture(SDL_Texture *texture, const SDL_Rect &src, const SDL_Rect &dst) {
                if (this->stretchGraphics) {
                    const SDL_Rect destination = {bengine::window::stretchX(dst.x), bengine::window::stretchY(dst.y), bengine::window::stretchX(dst.w), bengine::window::stretchY(dst.h)};
                    if (SDL_RenderCopy(this->renderer, texture, &src, &destination) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render SDL_Texture";
                        bengine::window::printError();
                    }
                    return;
                }
                if (SDL_RenderCopy(this->renderer, texture, &src, &dst) != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to render SDL_Texture";
                    bengine::window::printError();
                }
            }
            /** Render an SDL_Texture while also applying rotations/reflections
             * @param texture The SDL_Texture to render
             * @param src The portion of the SDL_Texture to copy and render (px for all 4 metrics) 
             * @param dst The portion of the window/dummy texture to copy to (px for all 4 metrics)  (will stretch the texture to fill the given rectangle)
             * @param angle The angle to rotate the texture (degrees)
             * @param center The point to rotate around (px for both metrics)  relative to the top-left corner of the destination rectangle
             * @param flip How to flip the rectangle (SDL_FLIP_NONE, SDL_FLIP_HORIZONTAL, SDL_FLIP_VERTICAL can be OR'd together)
             */
            void renderSDLTexture(SDL_Texture *texture, const SDL_Rect &src, const SDL_Rect &dst, const double &angle, const SDL_Point &center, const SDL_RendererFlip &flip) {
                if (this->stretchGraphics) {
                    const SDL_Rect destination = {bengine::window::stretchX(dst.x), bengine::window::stretchY(dst.y), bengine::window::stretchX(dst.w), bengine::window::stretchY(dst.h)};
                    if (SDL_RenderCopyEx(this->renderer, texture, &src, &destination, -angle, &center, flip) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render SDL_Texture";
                        bengine::window::printError();
                    }
                    return;
                }
                if (SDL_RenderCopyEx(this->renderer, texture, &src, &dst, -angle, &center, flip) != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to render SDL_Texture";
                    bengine::window::printError();
                }
            }

            /** Render a bengine::basicTexture
             * @param texture The bengine::basicTexture to render
             * @param dst The portion of the window/dummy texture to copy to (px for all 4 metrics)  (will stretch the texture to fill the given rectangle)
             */
            void renderBasicTexture(const bengine::basicTexture &texture, const SDL_Rect &dst) {
                const SDL_Rect frame = texture.getFrame();
                if (this->stretchGraphics) {
                    const SDL_Rect destination = {bengine::window::stretchX(dst.x), bengine::window::stretchY(dst.y), bengine::window::stretchX(dst.w), bengine::window::stretchY(dst.h)};
                    if (SDL_RenderCopy(this->renderer, texture.getTexture(), &frame, &destination) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render bengine::basicTexture";
                        bengine::window::printError();
                    }
                    return;
                }
                if (SDL_RenderCopy(this->renderer, texture.getTexture(), &frame, &dst) != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to render bengine::basicTexture";
                    bengine::window::printError();
                }
            }
            /** Render a bengine::basicTexture while also applying rotations/reflections
             * @param texture The bengine::basicTexture to render
             * @param dst The portion of the window/dummy texture to copy to (px for all 4 metrics)  (will stretch the texture to fill the given rectangle)
             * @param angle The angle to rotate the texture (degrees)
             * @param center The point to rotate around (px for both metrics)  relative to the top-left corner of the destination rectangle
             * @param flip How to flip the rectangle (SDL_FLIP_NONE, SDL_FLIP_HORIZONTAL, SDL_FLIP_VERTICAL can be OR'd together)
             */
            void renderBasicTexture(const bengine::basicTexture &texture, const SDL_Rect &dst, const double &angle, const SDL_Point &pivot, const SDL_RendererFlip &flip) {
                const SDL_Rect frame = texture.getFrame();
                if (this->stretchGraphics) {
                    const SDL_Rect destination = {bengine::window::stretchX(dst.x), bengine::window::stretchY(dst.y), bengine::window::stretchX(dst.w), bengine::window::stretchY(dst.h)};
                    if (SDL_RenderCopyEx(this->renderer, texture.getTexture(), &frame, &destination, -angle, &pivot, flip) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render bengine::basicTexture";
                        bengine::window::printError();
                    }
                    return;
                }
                if (SDL_RenderCopyEx(this->renderer, texture.getTexture(), &frame, &dst, -angle, &pivot, flip) != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to render bengine::basicTexture";
                    bengine::window::printError();
                }
            }

            /** Render a bengine::moddedTexture
             * @param texture The bengine::moddedTexture to render
             * @param dst The portion of the window/dummy texture to copy to (px for all 4 metrics) (will stretch the texture to fill the given rectangle)
             */
            void renderModdedTexture(const bengine::moddedTexture &texture, const SDL_Rect &dst) {
                const SDL_Rect frame = texture.getFrame();
                if (this->stretchGraphics) {
                    const SDL_Rect destination = {bengine::window::stretchX(dst.x), bengine::window::stretchY(dst.y), bengine::window::stretchX(dst.w), bengine::window::stretchY(dst.h)};
                    if (SDL_RenderCopy(this->renderer, texture.getTexture(), &frame, &destination) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render bengine::moddedTexture";
                        bengine::window::printError();
                    }
                    return;
                }
                if (SDL_RenderCopy(this->renderer, texture.getTexture(), &frame, &dst) != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to render bengine::moddedTexture";
                    bengine::window::printError();
                }
            }
            /** Render a bengine::moddedTexture while also applying rotations/reflections
             * @param texture The bengine::moddedTexture to render
             * @param dst The portion of the window/dummy texture to copy to (px for all 4 metrics) (will stretch the texture to fill the given rectangle)
             * @param angle The angle to rotate the texture (degrees)
             * @param center The point to rotate around (px for both metrics)  relative to the top-left corner of the destination rectangle
             * @param flip How to flip the rectangle (SDL_FLIP_NONE, SDL_FLIP_HORIZONTAL, SDL_FLIP_VERTICAL can be OR'd together)
             */
            void renderModdedTexture(const bengine::moddedTexture &texture, const SDL_Rect &dst, const double &angle, const SDL_Point &pivot, const SDL_RendererFlip &flip) {
                const SDL_Rect frame = texture.getFrame();
                if (this->stretchGraphics) {
                    const SDL_Rect destination = {bengine::window::stretchX(dst.x), bengine::window::stretchY(dst.y), bengine::window::stretchX(dst.w), bengine::window::stretchY(dst.h)};
                    if (SDL_RenderCopyEx(this->renderer, texture.getTexture(), &frame, &destination, -angle, &pivot, flip) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render bengine::moddedTexture";
                        bengine::window::printError();
                    }
                    return;
                }
                if (SDL_RenderCopyEx(this->renderer, texture.getTexture(), &frame, &dst, -angle, &pivot, flip) != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to render bengine::moddedTexture";
                    bengine::window::printError();
                }
            }

            /** Render a bengine::shiftingTexture
             * @param texture The bengine::shiftingTexture to render
             * @param dst The portion of the window/dummy texture to copy to (px for all 4 metrics) (will stretch the texture to fill the given rectangle)
             */
            void renderShiftingTexture(const bengine::shiftingTexture &texture, const SDL_Rect &dst) {
                const SDL_Rect frame = texture.getFrame();
                const SDL_Point pivot = texture.getPivot();
                if (this->stretchGraphics) {
                    const SDL_Rect destination = {bengine::window::stretchX(dst.x), bengine::window::stretchY(dst.y), bengine::window::stretchX(dst.w), bengine::window::stretchY(dst.h)};
                    if (SDL_RenderCopyEx(this->renderer, texture.getTexture(), &frame, &destination, -texture.getAngle(), &pivot, texture.getFlip()) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render bengine::shiftingTexture";
                        bengine::window::printError();
                    }
                    return;
                }
                if (SDL_RenderCopyEx(this->renderer, texture.getTexture(), &frame, &dst, -texture.getAngle(), &pivot, texture.getFlip()) != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to render bengine::shiftingTexture";
                    bengine::window::printError();
                }
            }

            /** Render text using a TTF_Font based off of a point (supports most unicode characters)
             * @param font The TTF_Font to use (represents both the font and size of the font)
             * @param text The text to display (literals are written as u"[text]", std::u16_string is useful too)
             * @param x x-position of the top-left corner of the text (px)
             * @param y y-position of the top-left corner of the text (px)
             * @param wrapWidth The maximum width for the text to render (px) (a width of zero prevents any wrapping)
             * @param color The color to fill the circle with as an SDL_Color
             */
            void renderText(TTF_Font *font, const char16_t *text, const int &x, const int &y, const Uint32 &wrapWidth, const SDL_Color &color = bengine::colors[bengine::COLOR_WHITE]) {
                SDL_Surface *surface = TTF_RenderUNICODE_Blended_Wrapped(font, (Uint16*)text, color, wrapWidth);

                const SDL_Rect src = {0, 0, surface->w, surface->h};
                const SDL_Rect dst = {x, y, surface->w, surface->h};
                bengine::window::renderSDLTexture(SDL_CreateTextureFromSurface(this->renderer, surface), src, dst);
                
                SDL_FreeSurface(surface);
                surface = nullptr;
            }
            /** Render text using a TTF_Font based off of a point (supports most unicode characters)
             * @param font The TTF_Font to use (represents both the font and size of the font)
             * @param text The text to display (literals are written as u"[text]", std::u16_string is useful too)
             * @param dst The portion of the window/dummy texture to copy to (px for all 4 metrics) (will stretch the text to fill the given rectangle)
             * @param color The color to fill the circle with as an SDL_Color
             */
            void renderText(TTF_Font *font, const char16_t *text, const SDL_Rect &dst, const SDL_Color &color = bengine::colors[bengine::COLOR_WHITE]) {
                SDL_Surface *surface = TTF_RenderUNICODE_Blended_Wrapped(font, (Uint16*)text, color, dst.w);
                SDL_Texture *texture = SDL_CreateTextureFromSurface(this->renderer, surface);
                
                const SDL_Rect src = {0, 0, surface->w, surface->h};
                bengine::window::renderSDLTexture(texture, src, dst);

                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
                surface = nullptr;
                texture = nullptr;
            }
    };
}

#endif // BENGINE_WINDOW_hpp
