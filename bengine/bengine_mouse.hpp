#ifndef BENGINE_MOUSE_hpp
#define BENGINE_MOUSE_hpp

#include <SDL2/SDL_mouse.h>

namespace bengine {
    enum {
        MOUSE1 = 0,
        MOUSE2 = 1,
        MOUSE3 = 2,
        MOUSE4 = 4,
        MOUSE5 = 5,
        LEFT_MOUSE_BUTTON = 0,
        RIGHT_MOUSE_BUTTON = 1,
        MIDDLE_MOUSE_BUTTON = 2,
        BACKWARD_MOUSE_BUTTON = 4,
        FORWARD_MOUSE_BUTTON = 5
    } mouseButtonTitles;

    class genericMouseState {
        protected:
            SDL_Point pos = {0, 0};
            bool motion = false;
            bool buttons[5] = {false, false, false, false, false};

        public:
            genericMouseState() {

            }
            ~genericMouseState() {

            }

            virtual void update();

            Uint16 posx() const {
                return this->pos.x;
            }
            Uint16 posy() const {
                return this->pos.y;
            }
            bool moved() const {
                return this->motion;
            }
            bool pressed(const Uint8 &button) const {
                if (button > 4) {
                    return false;
                }
                return this->buttons[button];
            }
    };

    class normalMouseState {
        private:
            SDL_Point pos;
            bool motion = false;

            bool buttons[5] = {false, false, false, false, false};
        
        public:
            normalMouseState() {

            }

            void update() {
                this->motion = true;
                SDL_GetMouseState(&this->pos.x, &this->pos.y);
            }

            Uint16 posx() const {
                return this->pos.x;
            }
            Uint16 posy() const {
                return this->pos.y;
            }
            bool moved() const {
                return this->motion;
            }
            bool pressed(const Uint8 &button) const {
                if (button > 4) {
                    return false;
                }
                return this->buttons[button];
            }
    };

    class relativeMouseState {

    };
}

#endif /* BENGINE_MOUSE_hpp */
