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
        COLOR_BLACK,
        COLOR_WHITE,
        COLOR_LIGHT_GRAY,
        COLOR_DARK_GRAY,
        COLOR_BROWN,
        COLOR_RED,
        COLOR_MAROON,
        COLOR_ORANGE,
        COLOR_YELLOW,
        COLOR_LIME,
        COLOR_GREEN,
        COLOR_CYAN,
        COLOR_TEAL,
        COLOR_BLUE,
        COLOR_MAGENTA,
        COLOR_PURPLE
    } colorTitles;

    class window {
        private:
            SDL_Window *win = NULL;
            SDL_Renderer *renderer = NULL;

            Uint16 width;
            Uint16 height;
            Uint16 halfWidth;
            Uint16 halfHeight;
            const char* title;

            bool isFullscreen = false;

            bool lockRatio = true;
            Uint8 ratioX;
            Uint8 ratioY;

            bool stretchGraphics = true;
            Uint16 baseWidth;
            Uint16 baseHeight;

            SDL_Texture *dummyTexture = NULL;
            SDL_PixelFormat pixelFormat;
            bool renderTarget = RENDERTARGET_WINDOW;

            int changeDrawColor(const SDL_Color &color) {
                const int output = SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
                if (output != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to change renderer's drawing color";
                    bengine::window::printError();
                }
                return output;
            }
            void printError() const {
                std::cout << "\nERROR [" << SDL_GetTicks() << "]: " << SDL_GetError() << "\n";
            }

            int stretchX(const int &x) const {
                return (int)(this->width * x / this->baseWidth);
            }
            int stretchY(const int &y) const {
                return (int)(this->height * y / this->baseHeight);
            }

        public:
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
            ~window() {
                SDL_DestroyRenderer(this->renderer);
                SDL_DestroyWindow(this->win);
                this->renderer = nullptr;
                this->win = nullptr;
            }

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
            Uint32 windowFlags() const {
                return SDL_GetWindowFlags(this->win);
            }
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
                    bengine::window::order();
                } else {
                    bengine::window::unorder();
                }
            }
            void lockSize() {
                SDL_SetWindowResizable(this->win, SDL_FALSE);
            }
            void unlockSize() {
                SDL_SetWindowResizable(this->win, SDL_TRUE);
            }
            void show() {
                SDL_ShowWindow(this->win);
            }
            void hide() {
                SDL_HideWindow(this->win);
            }
            void makeBorderless() {
                SDL_SetWindowBordered(this->win, SDL_FALSE);
            }
            void makeBordered() {
                SDL_SetWindowBordered(this->win, SDL_TRUE);
            }
            void minimize() {
                SDL_MinimizeWindow(this->win);
            }
            void maximize() {
                SDL_MaximizeWindow(this->win);
            }
            void grabMouse() {
                SDL_SetWindowGrab(this->win, SDL_TRUE);
            }
            void releaseMouse() {
                SDL_SetWindowGrab(this->win, SDL_FALSE);
            }
            void order() {
                SDL_SetWindowAlwaysOnTop(this->win, SDL_TRUE);
            }
            void unorder() {
                SDL_SetWindowAlwaysOnTop(this->win, SDL_FALSE);
            }
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
            bool makeWindowed() {
                const bool output = this->isFullscreen;
                SDL_SetWindowFullscreen(this->win, SDL_FALSE);
                this->isFullscreen = false;
                bengine::window::updateDims();
                return output;
            }
            bool toggleFullscreen(const bool &trueFullscreen = true) {
                const bool output = this->isFullscreen;
                if (this->isFullscreen) {
                    bengine::window::makeWindowed();
                } else {
                    bengine::window::makeFullscreen(trueFullscreen);
                }
                return output;
            }

            Uint16 getWidth() const {
                return this->width;
            }
            Uint16 setWidth(const Uint16 &width) {
                const Uint16 output = this->width;
                this->width = width;
                this->halfWidth = this->width / 2;
                SDL_SetWindowSize(this->win, this->width, this->height);
                return output;
            }
            Uint16 adjustWidth(const Uint16 &width) {
                const Uint16 output = this->width;
                this->width += width;
                this->halfWidth = this->width / 2;
                SDL_SetWindowSize(this->win, this->width, this->height);
                return output;
            }

            Uint16 getHeight() const {
                return this->height;
            }
            Uint16 setHeight(const Uint16 &height) {
                const Uint16 output = this->height;
                this->height = height;
                this->halfHeight = this->height / 2;
                SDL_SetWindowSize(this->win, this->width, this->height);
                return output;
            }
            Uint16 adjustHeight(const Uint16 &height) {
                const Uint16 output = this->height;
                this->height += height;
                this->halfHeight = this->height / 2;
                SDL_SetWindowSize(this->win, this->width, this->height);
                return output;
            }

            Uint16 getBaseWidth() const {
                return this->baseWidth;
            }
            Uint16 setBaseWidth(const Uint16 &width) {
                return btils::set<Uint16>(this->baseWidth, width);
            }
            Uint16 adjustBaseWidth(const Uint16 &width) {
                return btils::adj<Uint16>(this->baseWidth, width);
            }

            Uint16 getBaseHeight() const {
                return this->height;
            }
            Uint16 setBaseHeight(const Uint16 &height) {
                return btils::set<Uint16>(this->baseHeight, height);
            }
            Uint16 adjustBaseHeight(const Uint16 &height) {
                return btils::adj<Uint16>(this->baseHeight, height);
            }

            void setGraphicalStretching(const bool &stretchGraphics) {
                this->stretchGraphics = stretchGraphics;
            }
            void toggleGraphicalStretching() {
                this->stretchGraphics = !this->stretchGraphics;
            }

            Uint16 getHalfWidth() const {
                return this->halfWidth;
            }
            Uint16 getHalfHeight() const {
                return this->halfHeight;
            }

            const char* getTitle() const {
                return this->title;
            }
            const char* setTitle(const char* title) {
                const char* output = this->title;
                SDL_SetWindowTitle(this->win, title);
                this->title = title;
                return output;
            }

            void clear(const SDL_Color &color = bengine::colors[COLOR_BLACK]) {
                bengine::window::changeDrawColor(color);
                if (SDL_RenderClear(this->renderer) != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to clear renderer";
                    bengine::window::printError();
                }
            }
            void present() {
                SDL_RenderPresent(this->renderer);
            }

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
            void centerMouse() {
                SDL_WarpMouseInWindow(this->win, this->halfWidth, this->halfHeight);
            }
            void positionMouse(const Uint16 &x, const Uint16 &y) {
                SDL_WarpMouseInWindow(this->win, x, y);
            }

            void drawPixel(const int &x, const int &y, const SDL_Color &color = bengine::colors[bengine::COLOR_WHITE]) {
                bengine::window::changeDrawColor(color);

                if (this->stretchGraphics) {
                    if (SDL_RenderDrawPoint(this->renderer, bengine::window::stretchX(x), bengine::window::stretchY(y)) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to draw a pixel";
                        bengine::window::printError();
                    }
                } else {
                    if (SDL_RenderDrawPoint(this->renderer, x, y) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to draw a pixel";
                        bengine::window::printError();
                    }
                }
            }
            void drawLine(const int &x1, const int &y1, const int &x2, const int &y2, const SDL_Color &color = bengine::colors[bengine::COLOR_WHITE]) {
                bengine::window::changeDrawColor(color);

                if (this->stretchGraphics) {
                    if (SDL_RenderDrawLine(this->renderer, bengine::window::stretchX(x1), bengine::window::stretchX(y1), bengine::window::stretchX(x2), bengine::window::stretchX(y2)) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to draw a line";
                        bengine::window::printError();
                    }
                } else {
                    if (SDL_RenderDrawLine(this->renderer, x1, y1, x2, y2) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to draw a line";
                        bengine::window::printError();
                    }
                }
            }

            void drawRectangle(const int &x, const int &y, const int &w, const int &h, const SDL_Color &color = bengine::colors[bengine::COLOR_WHITE]) {
                bengine::window::changeDrawColor(color);
                
                if (this->stretchGraphics) {
                    const SDL_Rect dst = {x, y, w, h};
                    if (SDL_RenderDrawRect(this->renderer, &dst) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to draw a rectangle";
                        bengine::window::printError();
                    }
                } else {
                    const SDL_Rect dst = {bengine::window::stretchX(x), bengine::window::stretchY(y), bengine::window::stretchX(w), bengine::window::stretchY(h)};
                    if (SDL_RenderDrawRect(this->renderer, &dst) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to draw a rectangle";
                        bengine::window::printError();
                    }
                }
            }
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
            void fillRectangle(const int &x, const int &y, const int &w, const int &h, const SDL_Color &color = bengine::colors[bengine::COLOR_WHITE]) {
                bengine::window::changeDrawColor(color);

                if (this->stretchGraphics) {
                    const SDL_Rect dst = {x, y, w, h};
                    if (SDL_RenderFillRect(this->renderer, &dst) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to fill a rectangle";
                        bengine::window::printError();
                    }
                } else {
                    const SDL_Rect dst = {bengine::window::stretchX(x), bengine::window::stretchY(y), bengine::window::stretchX(w), bengine::window::stretchY(h)};
                    if (SDL_RenderFillRect(this->renderer, &dst) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to fill a rectangle";
                        bengine::window::printError();
                    }
                }
            }
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

            SDL_Texture *loadTexture(const char* filepath) {
                SDL_Texture *output = NULL;
                if ((output = IMG_LoadTexture(this->renderer, filepath)) == NULL) {
                    std::cout << "Window \"" << this->title << "\" failed to load texture";
                    bengine::window::printError();
                }
                return output;
            }

            SDL_PixelFormat getPixelFormat() const {
                return this->pixelFormat;
            }
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

            int initCanvas(const Uint16 &width, const Uint16 &height) {
                if (this->pixelFormat.format == SDL_PIXELFORMAT_UNKNOWN) {
                    bengine::window::setPixelFormat();
                }
                this->dummyTexture = SDL_CreateTexture(this->renderer, this->pixelFormat.format, SDL_TEXTUREACCESS_TARGET, width, height);
                if (this->dummyTexture == NULL) {
                    std::cout << "Window \"" << this->title << "\" failed to create texture canvas";
                    bengine::window::printError();
                    return -1;
                } else {
                    if (this->renderTarget == RENDERTARGET_DUMMY) {
                        bengine::window::targetDummy();
                    } else {
                        bengine::window::targetWindow();
                    }
                }
                return 0;
            }
            int targetDummy() {
                const int output = SDL_SetRenderTarget(this->renderer, this->dummyTexture);
                if (output != 0) {
                    std::cout << "Window \"" << this->title << "\" failed to switch the rendering target to the texture canvas";
                    bengine::window::printError();
                } else {
                    this->renderTarget = RENDERTARGET_DUMMY;
                }
                return output;
            }
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
            SDL_Texture* copyCanvas() {
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

            void renderSDLTexture(SDL_Texture *texture, const SDL_Rect &src, const SDL_Rect &dst) {
                if (this->stretchGraphics) {
                    const SDL_Rect destination = {bengine::window::stretchX(dst.x), bengine::window::stretchY(dst.y), bengine::window::stretchX(dst.w), bengine::window::stretchY(dst.h)};
                    if (SDL_RenderCopy(this->renderer, texture, &src, &destination) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render SDL_Texture";
                        bengine::window::printError();
                    }
                } else {
                    if (SDL_RenderCopy(this->renderer, texture, &src, &dst) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render SDL_Texture";
                        bengine::window::printError();
                    }
                }
            }
            void renderSDLTexture(SDL_Texture *texture, const SDL_Rect &src, const SDL_Rect &dst, const double &angle, const SDL_Point &center, const SDL_RendererFlip &flip) {
                if (this->stretchGraphics) {
                    const SDL_Rect destination = {bengine::window::stretchX(dst.x), bengine::window::stretchY(dst.y), bengine::window::stretchX(dst.w), bengine::window::stretchY(dst.h)};
                    if (SDL_RenderCopyEx(this->renderer, texture, &src, &destination, -angle, &center, flip) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render SDL_Texture";
                        bengine::window::printError();
                    }
                } else {
                    if (SDL_RenderCopyEx(this->renderer, texture, &src, &dst, -angle, &center, flip) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render SDL_Texture";
                        bengine::window::printError();
                    }
                }
            }

            void renderBasicTexture(const bengine::basicTexture &texture, const SDL_Rect &dst) {
                const SDL_Rect frame = texture.getFrame();
                if (this->stretchGraphics) {
                    const SDL_Rect destination = {bengine::window::stretchX(dst.x), bengine::window::stretchY(dst.y), bengine::window::stretchX(dst.w), bengine::window::stretchY(dst.h)};
                    if (SDL_RenderCopy(this->renderer, texture.getTexture(), &frame, &destination) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render bengine::basicTexture";
                        bengine::window::printError();
                    }
                } else {
                    if (SDL_RenderCopy(this->renderer, texture.getTexture(), &frame, &dst) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render bengine::basicTexture";
                        bengine::window::printError();
                    }
                }
            }
            void renderBasicTexture(const bengine::basicTexture &texture, const SDL_Rect &dst, const double &angle, const SDL_Point &pivot, const SDL_RendererFlip &flip) {
                const SDL_Rect frame = texture.getFrame();
                if (this->stretchGraphics) {
                    const SDL_Rect destination = {bengine::window::stretchX(dst.x), bengine::window::stretchY(dst.y), bengine::window::stretchX(dst.w), bengine::window::stretchY(dst.h)};
                    if (SDL_RenderCopyEx(this->renderer, texture.getTexture(), &frame, &destination, -angle, &pivot, flip) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render bengine::basicTexture";
                        bengine::window::printError();
                    }
                } else {
                    if (SDL_RenderCopyEx(this->renderer, texture.getTexture(), &frame, &dst, -angle, &pivot, flip) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render bengine::basicTexture";
                        bengine::window::printError();
                    }
                }
            }

            void renderModdedTexture(const bengine::moddedTexture &texture, const SDL_Rect &dst) {
                const SDL_Rect frame = texture.getFrame();
                if (this->stretchGraphics) {
                    const SDL_Rect destination = {bengine::window::stretchX(dst.x), bengine::window::stretchY(dst.y), bengine::window::stretchX(dst.w), bengine::window::stretchY(dst.h)};
                    if (SDL_RenderCopy(this->renderer, texture.getTexture(), &frame, &destination) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render bengine::moddedTexture";
                        bengine::window::printError();
                    }
                } else {
                    if (SDL_RenderCopy(this->renderer, texture.getTexture(), &frame, &dst) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render bengine::moddedTexture";
                        bengine::window::printError();
                    }
                }
            }
            void renderModdedTexture(const bengine::moddedTexture &texture, const SDL_Rect &dst, const double &angle, const SDL_Point &pivot, const SDL_RendererFlip &flip) {
                const SDL_Rect frame = texture.getFrame();
                if (this->stretchGraphics) {
                    const SDL_Rect destination = {bengine::window::stretchX(dst.x), bengine::window::stretchY(dst.y), bengine::window::stretchX(dst.w), bengine::window::stretchY(dst.h)};
                    if (SDL_RenderCopyEx(this->renderer, texture.getTexture(), &frame, &destination, -angle, &pivot, flip) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render bengine::moddedTexture";
                        bengine::window::printError();
                    }
                } else {
                    if (SDL_RenderCopyEx(this->renderer, texture.getTexture(), &frame, &dst, -angle, &pivot, flip) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render bengine::moddedTexture";
                        bengine::window::printError();
                    }
                }
            }

            void renderShiftingTexture(const bengine::shiftingTexture &texture, const SDL_Rect &dst) {
                const SDL_Rect frame = texture.getFrame();
                const SDL_Point center = texture.getPivot();
                if (this->stretchGraphics) {
                    const SDL_Rect destination = {bengine::window::stretchX(dst.x), bengine::window::stretchY(dst.y), bengine::window::stretchX(dst.w), bengine::window::stretchY(dst.h)};
                    if (SDL_RenderCopyEx(this->renderer, texture.getTexture(), &frame, &destination, -texture.getAngle(), &center, texture.getFlip()) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render bengine::shiftingTexture";
                        bengine::window::printError();
                    }
                } else {
                    if (SDL_RenderCopyEx(this->renderer, texture.getTexture(), &frame, &dst, -texture.getAngle(), &center, texture.getFlip()) != 0) {
                        std::cout << "Window \"" << this->title << "\" failed to render bengine::shiftingTexture";
                        bengine::window::printError();
                    }
                }
            }

            void renderText(TTF_Font *font, const char16_t *text, const SDL_Point &pos, const Uint32 &wrapWidth, const SDL_Color &color = bengine::colors[bengine::COLOR_WHITE]) {
                SDL_Surface *surface = TTF_RenderUNICODE_Blended_Wrapped(font, (Uint16*)text, color, wrapWidth);
                bengine::window::renderSDLTexture(SDL_CreateTextureFromSurface(this->renderer, surface), {0, 0, surface->w, surface->h}, {pos.x, pos.y, surface->w, surface->h});
                SDL_FreeSurface(surface);
                surface = nullptr;
            }
            void renderText(TTF_Font *font, const char16_t *text, const SDL_Rect &dst, const SDL_Color &color = bengine::colors[bengine::COLOR_WHITE]) {
                SDL_Surface *surface = TTF_RenderUNICODE_Blended_Wrapped(font, (Uint16*)text, color, dst.w);
                SDL_Texture *texture = SDL_CreateTextureFromSurface(this->renderer, surface);

                bengine::window::renderSDLTexture(texture, {0, 0, surface->w, surface->h}, dst);

                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
                surface = nullptr;
                texture = nullptr;
            }
    };
}

#endif /* BENGINE_WINDOW_hpp */
