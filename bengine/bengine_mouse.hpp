#ifndef BENGINE_MOUSE_hpp
#define BENGINE_MOUSE_hpp

#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>

#include "bengine_window.hpp"

namespace bengine {
    typedef enum {
        MOUSE1 = 1,
        MOUSE2 = 3,
        MOUSE3 = 2,
        MOUSE4 = 4,
        MOUSE5 = 5,
        LEFT_MOUSE_BUTTON = 1,
        RIGHT_MOUSE_BUTTON = 3,
        MIDDLE_MOUSE_BUTTON = 2,
        BACKWARD_MOUSE_BUTTON = 4,
        FORWARD_MOUSE_BUTTON = 5
    } mouseButtonTitles;

    class genericMouseState {
        protected:
            Uint16 x;
            Uint16 y;
            bool motion = false;
            bool buttons[5] = {false, false, false, false, false};

        public:
            genericMouseState() {}
            ~genericMouseState() {}

            virtual void updateMotion(const SDL_Event &event) = 0;

            void pressButton(const SDL_Event &event) {
                this->buttons[event.button.button - 1] = true;
            }
            void pressButton(const Uint8 &button) {
                if (button > 0 && button < 6) {
                    this->buttons[button] = true;
                }
            }
            void releaseButton(const SDL_Event &event) {
                this->buttons[event.button.button - 1] = false;
            }
            void releaseButton(const Uint8 &button) {
                if (button > 0 && button < 6) {
                    this->buttons[button] = false;
                }
            }
            void update(const SDL_Event &event) {
                switch (event.type) {
                    case SDL_MOUSEMOTION:
                        this->updateMotion(event);
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        this->pressButton(event);
                        break;
                    case SDL_MOUSEBUTTONUP:
                        this->releaseButton(event);
                        break;
                }
            }

            Uint16 posx() const {
                return this->x;
            }
            Uint16 posy() const {
                return this->y;
            }
            bool moved() const {
                return this->motion;
            }
            bool pressed(const Uint8 &button) const {
                if (button > 5 || button < 1) {
                    return false;
                }
                return this->buttons[button - 1];
            }
    };

    class normalMouseState : public genericMouseState {
        public:
            normalMouseState() {}
            ~normalMouseState() {}

            void updateMotion(const SDL_Event &event) {
                this->motion = true;
                this->x = event.motion.x;
                this->y = event.motion.y;
            }
    };

    class relativeMouseState : public genericMouseState {
        private:
            double sensitivity = 10;

            bool released = true;
            bool captured = false;

        public:
            relativeMouseState() {}
            ~relativeMouseState() {}

            void updateMotion(const SDL_Event &event) {
                this->motion = true;
                this->x = event.motion.xrel;
                this->y = event.motion.yrel;

                if (this->released) {
                    this->released = false;
                    this->x = this->y = 0;
                }
            }

            bool isCaptured() const {
                return this->captured;
            }
            bool capture() {
                const bool output = this->captured;
                if (SDL_SetRelativeMouseMode(SDL_TRUE) != 0) {
                    std::cout << "Could not enter relative mouse mode\nERROR [" << SDL_GetTicks() << "]: " << SDL_GetError() << "\n";
                }
                this->x = this->y = 0;
                this->captured = true;
                return output;
            }
            bool release() {
                const bool output = this->captured;
                if (SDL_SetRelativeMouseMode(SDL_FALSE) != 0) {
                    std::cout << "Could not exit relative mouse mode\nERROR [" << SDL_GetTicks() << "]: " << SDL_GetError() << "\n";
                }
                this->x = this->y = 0;
                this->captured = false;
                this->released = true;
                return output;
            }
            bool release(bengine::window &win) {
                const bool output = this->captured;
                if (SDL_SetRelativeMouseMode(SDL_FALSE) != 0) {
                    std::cout << "Could not exit relative mouse mode\nERROR [" << SDL_GetTicks() << "]: " << SDL_GetError() << "\n";
                }
                win.centerMouse();
                this->x = this->y = 0;
                this->captured = false;
                this->released = true;
                return output;
            }

            double getSensitivity() const {
                return this->sensitivity;
            }
            double setSensitivity(const double &sensitivity) {
                const double output = this->sensitivity;
                this->sensitivity = sensitivity;
                return output;
            }
    };

    class clickRectangle {
        protected:
            Uint16 x1 = 0;
            Uint16 y1 = 0;
            Uint16 x2 = 0;
            Uint16 y2 = 0;

        public:
            clickRectangle(const Uint16 x1 = 0, const Uint16 y1 = 0, const Uint16 x2 = 0, const Uint16 y2 = 0){
                this->setAreaCorners(x1, y1, x2, y2);
            }
            ~clickRectangle() {}

            Uint16 getX1() const {
                return this->x1;
            }
            Uint16 getY1() const {
                return this->y1;
            }
            Uint16 getX2() const {
                return this->x2;
            }
            Uint16 getY2() const {
                return this->y2;
            }
            Uint16 setX1(const Uint16 &x) {
                const Uint16 output = this->x1;
                this->x1 = x;
                return output;
            }
            Uint16 setY1(const Uint16 &y) {
                const Uint16 output = this->y1;
                this->y1 = y;
                return output;
            }
            Uint16 setX2(const Uint16 &x) {
                const Uint16 output = this->x2;
                this->x2 = x;
                return output;
            }
            Uint16 setY2(const Uint16 &y) {
                const Uint16 output = this->y2;
                this->y2 = y;
                return output;
            }
            void setAreaCorners(const Uint16 x1, const Uint16 y1, const Uint16 x2, const Uint16 y2) {
                if (x1 < x2) {
                    this->x1 = x1;
                    this->x2 = x2;
                } else {
                    this->x1 = x2;
                    this->x2 = x1;
                }
                if (y1 < y2) {
                    this->y1 = y1;
                    this->y2 = y2;
                } else {
                    this->y1 = y2;
                    this->y2 = y1;
                }
            }

            Uint16 getX() const {
                return this->x1;
            }
            Uint16 getY() const {
                return this->y1;
            }
            Uint16 getW() const {
                return this->x2 - this->x1;
            }
            Uint16 getH() const {
                return this->y2 - this->y1;
            }
            Uint16 setX(const Uint16 &x) {
                return this->setX1(x);
            }
            Uint16 setY(const Uint16 &y) {
                return this->setY1(y);
            }
            Uint16 setW(const Uint16 &w) {
                const Uint16 output = this->getW();
                this->x2 = this->x1 + w;
                return output;
            }
            Uint16 setH(const Uint16 &h) {
                const Uint16 output = this->getH();
                this->y2 = this->y1 + h;
                return output;
            }
            void setAreaRect(const Uint16 x, const Uint16 y, const Uint16 &w, const Uint16 &h) {
                this->x1 = x;
                this->x2 = x + w;
                this->y1 = y;
                this->y2 = y + h;
            }

            bool checkPos(const normalMouseState &mouseState) const {
                if (this->getW() == 0 || this->getH() == 0) {
                    return false;
                }
                return mouseState.posx() >= this->x1 && mouseState.posy() >= this->y1 && mouseState.posx() <= this->x2 && mouseState.posy() <= this->y2;
            }
            bool checkButton(const normalMouseState &mouseState, const Uint8 &button) const {
                if (!mouseState.pressed(button)) {
                    return false;
                }
                return this->checkPos(mouseState);
            }
    };

    class clickCircle {
        protected:
            Uint16 x = 0;
            Uint16 y = 0;
            Uint16 radius = 0;

        public:
            clickCircle(const Uint16 &x = 0, const Uint16 &y = 0, const Uint16 &radius = 0) : x(x), y(y), radius(radius) {}
            ~clickCircle() {}

            Uint16 getX() const {
                return this->x;
            }
            Uint16 getY() const {
                return this->y;
            }
            Uint16 getRadius() const {
                return this->radius;
            }
            Uint16 setX(const Uint16 &x) {
                const Uint16 output = this->x;
                this->x = x;
                return output;
            }
            Uint16 setY(const Uint16 &y) {
                const Uint16 output = this->y;
                this->y = y;
                return output;
            }
            Uint16 setRadius(const Uint16 &radius) {
                const Uint16 output = this->radius;
                this->radius = radius;
                return output;
            }

            bool checkPos(const normalMouseState &mouseState) const {
                if (this->radius == 0) {
                    return false;
                }
                // First a square is checked to save on some math in the most common cases, then the circle is checked if the mouse is within the matching square
                if (mouseState.posx() >= this->x - radius && mouseState.posx() <= this->x + radius && mouseState.posy() >= this->y - radius && mouseState.posy() <= this->y + radius) {
                    const double root = std::round(std::sqrt(this->radius * this->radius - (mouseState.posx() - this->x) * (mouseState.posx() - this->x)));
                    return mouseState.posy() < this->y + root && mouseState.posy() > this->y - root;
                }
                return false;
            }
            bool checkButton(const normalMouseState &mouseState, const Uint8 &button) const {
                if (!mouseState.pressed(button)) {
                    return false;
                }
                return this->checkPos(mouseState);
            }
    };

    class clickMatrix : public bengine::clickRectangle {
        protected:
            Uint16 rows = 1;
            Uint16 cols = 1;
            double rowHeight = 1;
            double colWidth = 1;
        
        public:
            clickMatrix(const Uint16 x1 = 0, const Uint16 y1 = 0, const Uint16 x2 = 0, const Uint16 y2 = 0, const Uint16 &rows = 1, const Uint16 &cols = 1) : clickRectangle(x1, y1, x2, y2) {
                this->setRows(rows);
                this->setCols(cols);
            }
            ~clickMatrix() {}

            Uint16 getRows() const {
                return this->rows;
            }
            Uint16 getCols() const {
                return this->cols;
            }
            Uint16 setRows(const Uint16 &rows) {
                const Uint16 output = this->rows;
                if (rows > this->getH()) {
                    this->rows = this->getH();
                } else {
                    this->rows = rows;
                }
                this->rowHeight = this->rows == 0 ? this->getH() : this->getH() / this->rows;
                return output;
            }
            Uint16 setCols(const Uint16 &cols) {
                const Uint16 output = this->cols;
                if (cols > this->getW()) {
                    this->cols = this->getW();
                } else {
                    this->cols = cols;
                }
                this->colWidth = this->cols == 0 ? this->getW() : this->getW() / this->cols;
                return output;
            }

            Uint32 checkPos(const normalMouseState &mouseState) const {
                if (this->getW() == 0 || this->getH() == 0) {
                    return UINT32_MAX;
                }
                if (mouseState.posx() < this->x1 || mouseState.posx() > this->x2 || mouseState.posy() < this->y1 || mouseState.posy() > this->y2) {
                    return UINT32_MAX;
                }
                Uint16 posx = mouseState.posx() - this->x1;
                Uint16 posy = mouseState.posy() - this->y1;

                // Uint16 col = (mouseState.posx() == this->x2 ? this->cols - 1 : std::round(posx / this->colWidth));
                // Uint16 row = (mouseState.posy() == this->y2 ? this->rows - 1 : std::round(posy / this->rowHeight));

                Uint16 col = posx / this->colWidth;
                Uint16 row = posy / this->rowHeight;

                Uint16 tlc = std::abs(posx - this->colWidth * col - 1);
                Uint16 trc = std::abs(posx - this->colWidth * col);
                Uint16 blc = std::abs(posx - this->colWidth * col - 1);
                Uint16 brc = std::abs(posx - this->colWidth * col);
                
                Uint16 tlr = std::abs(posy - this->rowHeight * row - 1);
                Uint16 trr = std::abs(posy - this->rowHeight * row - 1);
                Uint16 blr = std::abs(posy - this->rowHeight * row);
                Uint16 brr = std::abs(posy - this->rowHeight * row);

                if ((tlc < trc && tlc < blc && tlc < brc) || (blc < tlc && blc < trc && blc < brc)) {
                    col--;
                }
                if ((tlr < trr && tlr < blr && tlr < brr) || (blr < tlr && blr < trr && blr < brr)) {
                    row--;
                }

                if (col >= this->cols) {
                    col = this->cols - 1;
                }
                
                const Uint32 pos = row * this->cols + col;
                return pos < this->cols * this->rows ? pos : UINT32_MAX;
            }
            Uint32 checkButton(const normalMouseState &mouseState, const Uint8 &button) const {
                const Uint32 pos = this->checkPos(mouseState);
                return mouseState.pressed(button) ? pos : UINT32_MAX;
            }
    };
}

#endif /* BENGINE_MOUSE_hpp */
