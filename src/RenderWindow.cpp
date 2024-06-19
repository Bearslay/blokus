#include "RenderWindow.hpp"

RenderWindow::RenderWindow(const char* title, const int &w, const int &h, Uint32 flags) : Window(NULL), Renderer(NULL), W(w), H(h), W_2(w / 2), H_2(h / 2) {
    if ((Window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags)) == NULL) {std::cout << "Window \"" << title << "\" failed to initialize\nERROR: " << SDL_GetError() << "\n";}
    if ((Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED)) == NULL) {std::cout << "Renderer for \"" << title << "\" failed to initialize\nERROR: " << SDL_GetError() << "\n";}
}
RenderWindow::RenderWindow() {RenderWindow("window", 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);}
RenderWindow::~RenderWindow() {
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
}

int RenderWindow::getRefreshRate() const {
    SDL_DisplayMode mode;
    SDL_GetDisplayMode(SDL_GetWindowDisplayIndex(Window), 0, &mode);
    return mode.refresh_rate;
}
Uint32 RenderWindow::getWindowFlags() {return SDL_GetWindowFlags(Window);}

int RenderWindow::getW() const {return W;}
int RenderWindow::setW(const int &w) {
    const int output = W;
    W = w;
    W_2 = W / 2;
    SDL_SetWindowSize(Window, W, H);
    return output;
}
int RenderWindow::adjW(const int &amount) {
    const int output = W;
    W += amount;
    W_2 = W / 2;
    SDL_SetWindowSize(Window, W, H);
    return output;
}
int RenderWindow::getH() const {return H;}
int RenderWindow::setH(const int &h) {
    const int output = H;
    H = h;
    H_2 = H / 2;
    SDL_SetWindowSize(Window, W, H);
    return output;
}
int RenderWindow::adjH(const int &amount) {
    const int output = W;
    H += amount;
    H_2 = H / 2;
    SDL_SetWindowSize(Window, W, H);
    return output;
}
SDL_Point RenderWindow::getDims() const {return {W, H};}
SDL_Point RenderWindow::setDims(const int &w, const int &h) {
    const SDL_Point output = {W, H};
    W = w;
    W_2 = W / 2;
    H = h;
    H_2 = H / 2;
    SDL_SetWindowSize(Window, W, H);
    return output;
}
SDL_Point RenderWindow::adjDims(const int &w, const int &h) {
    const SDL_Point output = {W, H};
    W += w;
    W_2 = W / 2;
    H += h;
    H_2 = H / 2;
    SDL_SetWindowSize(Window, W, H);
    return output;
}
SDL_Point RenderWindow::updateDims() {
    const SDL_Point output = {W, H};
    SDL_GetWindowSize(Window, &W, &H);
    W_2 = W / 2;
    H_2 = H / 2;
    return output;
}
int RenderWindow::getW_2() const {return W_2;}
int RenderWindow::getH_2() const {return H_2;}

bool RenderWindow::hasLockedRatio() const {return LockRatio;}
bool RenderWindow::setRatioLock(const bool &locked) {return btils::set<bool>(LockRatio, locked);}
bool RenderWindow::toggleRatioLock() {return btils::set<bool>(LockRatio, !LockRatio);}
std::pair<Uint8, Uint8> RenderWindow::getRatio() const {return Ratio;}
std::pair<Uint8, Uint8> RenderWindow::setRatio(const std::pair<Uint8, Uint8> &ratio) {return btils::set<std::pair<Uint8, Uint8>>(Ratio, ratio);}

const char* RenderWindow::getTitle() const {return SDL_GetWindowTitle(Window);}
const char* RenderWindow::setTitle(const char* title) {
    const char* output = RenderWindow::getTitle();
    SDL_SetWindowTitle(Window, title);
    return output;
}

void RenderWindow::clear(const SDL_Color &color) {
    SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(Renderer);
}
void RenderWindow::show() {SDL_RenderPresent(Renderer);}

bool RenderWindow::toggleFullscreen(const bool &trueFullscreen) {
    const bool output = IsFullscreen;
    if (!IsFullscreen) {SDL_SetWindowFullscreen(Window, trueFullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_FULLSCREEN_DESKTOP);}
    else {SDL_SetWindowFullscreen(Window, SDL_FALSE);}
    IsFullscreen = !IsFullscreen;
    RenderWindow::updateDims();
    return output;
}
void RenderWindow::centerMouse() {SDL_WarpMouseInWindow(Window, W_2, H_2);}

void RenderWindow::handleEvent(const SDL_WindowEvent &event) {
    switch (event.event) {
        case SDL_WINDOWEVENT_RESIZED:
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            if (LockRatio) {
                int w, h;
                SDL_GetWindowSize(Window, &w, &h);
                
                if (std::abs(W - w) / Ratio.first > std::abs(H - h) / Ratio.second) {
                    SDL_SetWindowSize(Window, (int)(h * (double)(Ratio.first / Ratio.second)), h);
                } else {
                    SDL_SetWindowSize(Window, w, (int)(w * (double)(Ratio.second / Ratio.first)));
                }
            }
            RenderWindow::updateDims();
            break;
    }
}

void RenderWindow::drawPixel(const int &x, const int &y, const SDL_Color &color) {
    SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);

    if (!StretchGraphicsFromStandard) {
        SDL_RenderDrawPoint(Renderer, W_2 + x, H_2 - y);
        return;
    }
    SDL_RenderDrawPoint(Renderer, W_2 + W * x / StandardWidth, H_2 - H * y / StandardHeight);
}

void RenderWindow::drawLine(const int &x1, const int &y1, const int &x2, const int &y2, const SDL_Color &color) {
    SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);

    if (!StretchGraphicsFromStandard) {
        SDL_RenderDrawLine(Renderer, W_2 + x1, H_2 - y1, W_2 + x2, H_2 - y2);
        return;
    }
    SDL_RenderDrawLine(Renderer, W_2 + W * x1 / StandardWidth, H_2 - H * y1 / StandardHeight, W_2 + W * x2 / StandardWidth, H_2 - H * y2 / StandardHeight);
}

void RenderWindow::drawRectangle(const int &x, const int &y, const int &w, const int &h, const SDL_Color &color) {
    SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);
    
    if (!StretchGraphicsFromStandard) {
        const SDL_Rect dst = {W_2 + x, H_2 - y, w, h};
        SDL_RenderDrawRect(Renderer, &dst);
        return;
    }
    const SDL_Rect dst = {W_2 + W * x / StandardWidth, H_2 - H * y / StandardHeight, w * W / StandardWidth, h * H / StandardHeight};
    SDL_RenderDrawRect(Renderer, &dst);
}
void RenderWindow::drawThickRectangle(const int &x, const int &y, const int &w, const int &h, const int &thickness, const unsigned char mode, const SDL_Color &color) {
    switch (mode) {
        default:
        case THICKSHAPE_INNER:
            RenderWindow::fillRectangle(x, y, w, thickness, color);
            RenderWindow::fillRectangle(x, y - h + thickness, w, thickness, color);
            RenderWindow::fillRectangle(x, y - thickness, thickness, h - thickness * 2, color);
            RenderWindow::fillRectangle(x + w - thickness, y - thickness, thickness, h - thickness * 2, color);
            break;
        case THICKSHAPE_OUTER:
            RenderWindow::fillRectangle(x - thickness, y + thickness, w + thickness * 2, thickness, color);
            RenderWindow::fillRectangle(x - thickness, y - h, w + thickness * 2, thickness);
            RenderWindow::fillRectangle(x - thickness, y, thickness, h);
            RenderWindow::fillRectangle(x + w, y, thickness, h);
            break;
        case THICKSHAPE_MIDDLE:
            RenderWindow::fillRectangle(x - thickness / 2, y + thickness / 2, w + thickness, thickness, color);
            RenderWindow::fillRectangle(x - thickness / 2, y - h + thickness / 2, w + thickness, thickness, color);
            RenderWindow::fillRectangle(x - thickness / 2, y - thickness / 2, thickness, h - thickness, color);
            RenderWindow::fillRectangle(x + w - thickness / 2, y - thickness / 2, thickness, h - thickness, color);
            break;
    }
}
void RenderWindow::fillRectangle(const int &x, const int &y, const int &w, const int &h, const SDL_Color &color) {
    SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);
    
    if (!StretchGraphicsFromStandard) {
        const SDL_Rect dst = {W_2 + x, H_2 - y, w, h};
        SDL_RenderFillRect(Renderer, &dst);
        return;
    }
    const SDL_Rect dst = {W_2 + W * x / StandardWidth, H_2 - H * y / StandardHeight, w * W / StandardWidth, h * H / StandardHeight};
    SDL_RenderFillRect(Renderer, &dst);
}

void RenderWindow::drawCircle(const int &x, const int &y, const int &r, const SDL_Color &color) {
    SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);
    const int diameter = r * 2;
    int ox = r - 1;
    int oy = 0;
    int tx = 1;
    int ty = 1;
    int error = tx - diameter;
    while (ox >= oy) {
        SDL_RenderDrawPoint(Renderer, W_2 + x + ox, H_2 - y - oy);
        SDL_RenderDrawPoint(Renderer, W_2 + x + ox, H_2 - y + oy);
        SDL_RenderDrawPoint(Renderer, W_2 + x - ox, H_2 - y - oy);
        SDL_RenderDrawPoint(Renderer, W_2 + x - ox, H_2 - y + oy);
        SDL_RenderDrawPoint(Renderer, W_2 + x + oy, H_2 - y - ox);
        SDL_RenderDrawPoint(Renderer, W_2 + x + oy, H_2 - y + ox);
        SDL_RenderDrawPoint(Renderer, W_2 + x - oy, H_2 - y - ox);
        SDL_RenderDrawPoint(Renderer, W_2 + x - oy, H_2 - y + ox);
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
void RenderWindow::drawThickCircle(const int &x, const int &y, const int &r, const int &thickness, const unsigned char &mode, const SDL_Color &color) {
    switch (mode) {
        default:
        case THICKSHAPE_INNER:
            for (int i = 0; i < thickness; i++) {
                RenderWindow::drawCircle(x, y, r - i, color);
            }
            break;
        case THICKSHAPE_OUTER:
            for (int i = 0; i < thickness; i++) {
                RenderWindow::drawCircle(x, y, r + i, color);
            }
            break;
        case THICKSHAPE_MIDDLE:
            for (int i = 0; i < thickness; i++) {
                RenderWindow::drawCircle(x, y, r - thickness / 2 + i, color);
            }
            break;
    }
}
void RenderWindow::fillCircle(const int &x, const int &y, const int &r, const SDL_Color &color) {
    SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);
    int ox = 0;
    int oy = r;
    int error = r - 1;
    while (oy >= ox) {
        SDL_RenderDrawLine(Renderer, W_2 + x - oy, H_2 - y + ox, W_2 + x + oy, H_2 - y + ox);
        SDL_RenderDrawLine(Renderer, W_2 + x - ox, H_2 - y + oy, W_2 + x + ox, H_2 - y + oy);
        SDL_RenderDrawLine(Renderer, W_2 + x - ox, H_2 - y - oy, W_2 + x + ox, H_2 - y - oy);
        SDL_RenderDrawLine(Renderer, W_2 + x - oy, H_2 - y - ox, W_2 + x + oy, H_2 - y - ox);
        if (error >= ox * 2) {
            error -= ox * 2 + 1;
            ox++;
        } else if (error < 2 * (r - oy)) {
            error += oy * 2 - 1;
            oy--;;
        } else {
            error += 2 * (oy - ox - 1);
            oy--;
            ox++;
        }
    }
}

void RenderWindow::drawEllipse(const int &x, const int &y, const int &rx, const int &ry, const SDL_Color &color) {
    SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);
    double dx, dy, d1, d2, ox, oy;
    ox = 0;
    oy = ry;

    d1 = (ry * ry) - (rx * rx * ry) + (0.25 * rx * rx);
    dx = 2 * ry * ry * ox;
    dy = 2 * rx * rx * oy;
 
    while (dx < dy) {
        SDL_RenderDrawPoint(Renderer, W_2 + x + ox, H_2 - y - oy);
        SDL_RenderDrawPoint(Renderer, W_2 + x - ox, H_2 - y - oy);
        SDL_RenderDrawPoint(Renderer, W_2 + x + ox, H_2 - y + oy);
        SDL_RenderDrawPoint(Renderer, W_2 + x - ox, H_2 - y + oy);

        if (d1 < 0) {
            ox++;
            dx = dx + (2 * ry * ry);
            d1 = d1 + dx + (ry * ry);
        } else {
            ox++;
            oy--;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d1 = d1 + dx - dy + (ry * ry);
        }
    }
    d2 = ((ry * ry) * ((ox + 0.5) * (ox + 0.5))) + ((rx * rx) * ((oy - 1) * (oy - 1))) - (rx * rx * ry * ry);

    while (oy >= 0) {
        SDL_RenderDrawPoint(Renderer, W_2 + x + ox, H_2 - y - oy);
        SDL_RenderDrawPoint(Renderer, W_2 + x - ox, H_2 - y - oy);
        SDL_RenderDrawPoint(Renderer, W_2 + x + ox, H_2 - y + oy);
        SDL_RenderDrawPoint(Renderer, W_2 + x - ox, H_2 - y + oy);

        if (d2 > 0) {
            oy--;
            dy = dy - (2 * rx * rx);
            d2 = d2 + (rx * rx) - dy;
        } else {
            oy--;
            ox++;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d2 = d2 + dx - dy + (rx * rx);
        }
    }
}
void RenderWindow::drawThickEllipse(const int &x, const int &y, const int &rx, const int &ry, const int &thickness, const unsigned char &mode, const SDL_Color &color) {
    switch (mode) {
        default:
        case THICKSHAPE_INNER:
            for (int i = 0; i < thickness; i++) {
                RenderWindow::drawEllipse(x, y, rx - i, ry - i, color);
            }
            break;
        case THICKSHAPE_OUTER:
            for (int i = 0; i < thickness; i++) {
                RenderWindow::drawEllipse(x, y, rx + i, ry + i, color);
            }
            break;
        case THICKSHAPE_MIDDLE:
            for (int i = 0; i < thickness; i++) {
                RenderWindow::drawEllipse(x, y, rx - thickness / 2 + i, ry - thickness / 2 + i, color);
            }
            break;
    }
}

SDL_Texture* RenderWindow::loadTexture(const std::string &path) {
    SDL_Texture* output = NULL;
    if ((output = IMG_LoadTexture(Renderer, path.c_str())) == NULL) {std::cout << "Failed to load texture\nERROR: " << SDL_GetError() << "\n";}
    return output;
}

void RenderWindow::renderTexture(SDL_Texture* texture, const SDL_Rect &src, const SDL_Rect &dst) {
    if (!StretchGraphicsFromStandard) {
        const SDL_Rect destination = {W_2 + dst.x, H_2 - dst.y, dst.w, dst.h};
        SDL_RenderCopy(Renderer, texture, &src, &destination);
        return;
    }
    const SDL_Rect destination = {W_2 + W * dst.x / StandardWidth, H_2 - H * dst.y / StandardHeight, W * dst.w / StandardWidth, H * dst.h / StandardHeight};
    SDL_RenderCopy(Renderer, texture, &src, &destination);
}
void RenderWindow::renderTexture(SDL_Texture* texture, const SDL_Rect &src, const SDL_Rect &dst, const double &angle, const SDL_Point &center, const SDL_RendererFlip &flip) {
    if (!StretchGraphicsFromStandard) {
        const SDL_Rect destination = {W_2 + dst.x, H_2 - dst.y, dst.w, dst.h};
        SDL_RenderCopyEx(Renderer, texture, &src, &destination, angle, &center, flip);
        return;
    }
    const SDL_Rect destination = {W_2 + W * dst.x / StandardWidth, H_2 - H * dst.y / StandardHeight, W * dst.w / StandardWidth, H * dst.h / StandardHeight};
    SDL_RenderCopyEx(Renderer, texture, &src, &destination, angle, &center, flip);
}
void RenderWindow::renderTexture(const Texture &texture, const SDL_Rect &dst) {
    const SDL_Rect frame = texture.getFrame();
    const SDL_Point center = texture.getCenter();
    if (!StretchGraphicsFromStandard) {
        const SDL_Rect destination = {W_2 + dst.x, H_2 - dst.y, dst.w, dst.h};
        SDL_RenderCopyEx(Renderer, texture.getTexture(), &frame, &destination, -texture.getAngle() * 180 / M_PI, &center, texture.getFlip());
        return;
    }
    const SDL_Rect destination = {W_2 + W * dst.x / StandardWidth, H_2 - H * dst.y / StandardHeight, W * dst.w / StandardWidth, H * dst.h / StandardHeight};
    SDL_RenderCopyEx(Renderer, texture.getTexture(), &frame, &destination, -texture.getAngle() * 180 / M_PI, &center, texture.getFlip());
}
void RenderWindow::renderTexture(const Texture &texture, const SDL_Point &pos) {
    const SDL_Rect frame = texture.getFrame();
    const SDL_Point center = texture.getCenter();
    if (!StretchGraphicsFromStandard) {
        const SDL_Rect dst = {W_2 + pos.x, H_2 - pos.y, texture.getFrame().w, texture.getFrame().h};
        SDL_RenderCopyEx(Renderer, texture.getTexture(), &frame, &dst, -texture.getAngle() * 180 / M_PI, &center, texture.getFlip());
        return;
    }
    const SDL_Rect dst = {W_2 + W * pos.x / StandardWidth, H_2 - H * pos.y / StandardHeight, W * texture.getFrame().w / StandardWidth, H * texture.getFrame().h / StandardHeight};
    SDL_RenderCopyEx(Renderer, texture.getTexture(), &frame, &dst, -texture.getAngle() * 180 / M_PI, &center, texture.getFlip());
}
void RenderWindow::renderTexture(const Texture &texture, const int &x, const int &y) {
    const SDL_Point pos = {x, y};
    RenderWindow::renderTexture(texture, pos);
}

void RenderWindow::renderText(TTF_Font *font, const char16_t* text, const SDL_Point &pos, const Uint32 wrapWidth, const SDL_Color &color) {
    SDL_Surface *surface = TTF_RenderUNICODE_Blended_Wrapped(font, (Uint16*)text, color, wrapWidth);
    renderTexture(SDL_CreateTextureFromSurface(Renderer, surface), {0, 0, surface->w, surface->h}, {pos.x - surface->w / 2, pos.y + surface->h / 2, surface->w, surface->h});
    SDL_FreeSurface(surface);
}
void RenderWindow::renderText(TTF_Font *font, const char16_t *text, const SDL_Rect &dst, const SDL_Color &color) {
    SDL_Surface *surface = TTF_RenderUNICODE_Blended_Wrapped(font, (Uint16*)text, color, dst.w);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(Renderer, surface);

    RenderWindow::renderTexture(texture, {0, 0, surface->w, surface->h}, dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void RenderWindow::drawLineOverlap(const int &x1, const int &y1, const int &x2, const int &y2, const unsigned char overlapType, const SDL_Color &color) {
    if (x1 == x2 || y1 == y2) {
        RenderWindow::fillRectangle(x1, x2, std::abs(x2 - x1), std::abs(y2 - y1), color);
        return;
    }
    if (overlapType == LINE_OVERLAP_NONE) {
        RenderWindow::drawLine(x1, y1, x2, y2, color);
        return;
    }
    int dx = x2 - x1, dy = y2 - y1;
    int sx = 1, sy = 1;

    if (dx < 0) {
        dx = -dx;
        sx = -1;
    }
    if (dy < 0) {
        dy = -dy;
        sy = -1;
    }
    int x = x1, y = y1, error = 0;
    const int dx2 = dx * 2;
    const int dy2 = dy * 2;

    RenderWindow::drawPixel(x, y, color);
    if (dx > dy) {
        error = dy2 - dx;
        while (x != x2) {
            x += sx;
            if (error >= 0) {
                if (overlapType == LINE_OVERLAP_MAJOR || overlapType == LINE_OVERLAP_BOTH) {RenderWindow::drawPixel(x, y, color);}
                y += sy;
                if (overlapType == LINE_OVERLAP_MINOR || overlapType == LINE_OVERLAP_BOTH) {RenderWindow::drawPixel(x - sx, y, color);}
                error -= dx2;
            }
            error += dy2;
            RenderWindow::drawPixel(x, y, color);
        }
        return;
    }
    error = dx2 - dy;
    while (y != y2) {
        y += sy;
        if (error >= 0) {
            if (overlapType == LINE_OVERLAP_MAJOR || overlapType == LINE_OVERLAP_BOTH) {RenderWindow::drawPixel(x, y, color);}
            x += sx;
            if (overlapType == LINE_OVERLAP_MINOR || overlapType == LINE_OVERLAP_BOTH) {RenderWindow::drawPixel(x, y - sy, color);}
            error -= dy2;
        }
        error += dx2;
        RenderWindow::drawPixel(x, y, color);
    }
}

/**
 * Bresenham with thickness
 * No pixel missed and every pixel only drawn once!
 * The code is bigger and more complicated than drawThickLineSimple() but it tends to be faster, since drawing a pixel is often a slow operation.
 * aThicknessMode can be one of LINE_THICKNESS_MIDDLE, LINE_THICKNESS_DRAW_CLOCKWISE, LINE_THICKNESS_DRAW_COUNTERCLOCKWISE
 */
void RenderWindow::drawThickLine(const int x1, const int y1, const int x2, const int y2, const int thickness, const unsigned char thicknessMode, const SDL_Color &color) {
    if (thickness <= 1) {
        RenderWindow::drawLine(x1, y1, x2, y2, color);
        return;
    }
    int dx = x2 - x1, dy = y2 - y1;
    int sx = 1, sy = 1;
    bool swap = true;

    if (dx < 0) {
        dx = -dx;
        sx = -1;
        swap = !swap;
    }
    if (dy < 0) {
        dy = -dy;
        sy = -1;
        swap = !swap;
    }

    int xs = x1, xe = x2, ys = y1, ye = y2, error = 0;
    const int dx2 = dx * 2;
    const int dy2 = dy * 2;
    unsigned char overlap;

    int drawAdjust = thickness / 2;
    if (thicknessMode == LINE_THICKNESS_DRAW_COUNTERCLOCKWISE) {
        drawAdjust = thickness - 1;
    } else if (thicknessMode == LINE_THICKNESS_DRAW_CLOCKWISE) {
        drawAdjust = 0;
    }

    if (dx >= dy) {
        if (swap) {
            drawAdjust = thickness - drawAdjust - 1;
            sy = -sy;
        } else {
            sx = -sx;
        }
        error = dy2 - dx;
        for (int i = drawAdjust; i > 0; i--) {
            xs -= sx;
            xe -= sx;
            if (error >= 0) {
                ys -= sy;
                ye -= sy;
                error -= dx2;
            }
            error += dy2;
        }
        RenderWindow::drawLine(xs, ys, xe, ye, color);

        error = dy2 - dx;
        for (int i = thickness; i > 1; i--) {
            xs += sx;
            xe += sx;
            overlap = LINE_OVERLAP_NONE;
            if (error >= 0) {
                ys += sy;
                ye += sy;
                error -= dx2;
                overlap = LINE_OVERLAP_MAJOR;
            }
            error += dy2;
            RenderWindow::drawLineOverlap(xs, ys, xe, ye, overlap, color);
        }
        return;
    }

    if (swap) {
        sx = -sx;
    } else {
        drawAdjust = thickness - drawAdjust - 1;
        sy = -sy;
    }
    error = dx2 - dy;
    for (int i = drawAdjust; i > 0; i--) {
        ys -= sy;
        ye -= sy;
        if (error >= 0) {
            xs -= sx;
            xe -= sx;
            error -= dy2;
        }
        error += dx2;
    }
    RenderWindow::drawLine(xs, ys, xe, ye, color);

    error = dx2 - dy;
    for (int i = thickness; i > 1; i--) {
        ys += sy;
        ye += sy;
        overlap = LINE_OVERLAP_NONE;
        if (error >= 0) {
            xs += sx;
            xe += sx;
            error -= dy2;
            overlap = LINE_OVERLAP_MAJOR;
        }
        error += dx2;
        RenderWindow::drawLineOverlap(xs, ys, xe, ye, overlap, color);
    }
}
