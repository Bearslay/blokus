#ifndef CURSORBOX
#define CURSORBOX

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_events.h>

#include "RenderWindow.hpp"

struct MouseState {
    SDL_Point PosC = {0, 0};
    SDL_Point PosR = {0, 0};
    SDL_Point Rel = {0, 0};
    double Sensitivity = 10;

    bool Motion = false;
    bool Released = true;
    bool Captured = false;

    bool Pressed[6] = {false, false, false, false, false, false};
};

class MouseState1 {
    private:
        SDL_Point PosCentered = {0, 0};
        SDL_Point PosRegular = {0, 0};
        SDL_Point PosRelative = {0, 0};
        double CaptureSensitivity = 10;

        bool Motion = false;
        bool Released = true;
        bool Captured = false;

        bool PressedButtons[5] = {false, false, false, false, false};

    public:
        MouseState1() {}

        SDL_Point posCentered() const {return PosCentered;}
        SDL_Point posRegular() const {return PosRegular;}
        SDL_Point posRelative() const {return PosRelative;}
        bool isCaptured() const {return Captured;}

        bool PressButton(const Uint8 &button) {return btils::set<bool>(PressedButtons[button > 0 && button <= 5 ? button - 1 : 0], true);}
        bool PressButton(const SDL_Event &event) {return PressButton(event.button.button);}
        bool ReleaseButton(const Uint8 &button) {return btils::set<bool>(PressedButtons[button > 0 && button <= 5 ? button - 1 : 0], false);}
        bool ReleaseButton(const SDL_Event &event) {return ReleaseButton(event.button.button);}

        bool CaptureMouse() {
            SDL_SetRelativeMouseMode(SDL_FALSE);
            PosCentered = PosRegular = {0, 0};
            return btils::set<bool>(Captured, true);
        }
        bool ReleaseMouse(RenderWindow &window) {
            SDL_SetRelativeMouseMode(SDL_TRUE);
            Released = true;
            PosRelative = {0, 0};
            window.centerMouse();
            return btils::set<bool>(Captured, false);
        }
        bool ReleaseMouse() {
            SDL_SetRelativeMouseMode(SDL_TRUE);
            Released = true;
            PosRelative = {0, 0};
            return btils::set<bool>(Captured, false);
        }
};

class CursorBox {
    private:
        SDL_Rect Box = {0, 0, 0, 0};

    public:
        CursorBox(const SDL_Rect &box) : Box(box) {}

        bool check(const MouseState &mstate) {
            return mstate.PosR.x >= Box.x && mstate.PosR.y >= Box.y && mstate.PosR.x <= Box.x + Box.w && mstate.PosR.y <= Box.y + Box.h;
        }
};

#endif /* CURSORBOX */
