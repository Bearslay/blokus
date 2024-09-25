#ifndef BENGINE_MOUSE_hpp
#define BENGINE_MOUSE_hpp

#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>

#include "btils_main.hpp"
#include "bengine_window.hpp"

namespace bengine {
    typedef enum {
        MOUSE1 = 0x01,                   // Left mouse button
        MOUSE2 = 0x02,                   // Right mouse button
        MOUSE3 = 0x04,                   // Middle mouse button (scroll wheel)
        MOUSE4 = 0x08,                   // Mouse button usually used to 'go back' in a web page
        MOUSE5 = 0x10,                   // MOuse button usually used to 'go forwards' in a web page
        LEFT_MOUSE_BUTTON = 0x01,        // Left mouse button
        RIGHT_MOUSE_BUTTON = 0x02,       // Right mouse button
        MIDDLE_MOUSE_BUTTON = 0x04,      // Middle mouse button (scroll wheel)
        BACKWARD_MOUSE_BUTTON = 0x08,    // Mouse button usually used to 'go back' in a web page
        FORWARD_MOUSE_BUTTON = 0x10      // MOuse button usually used to 'go forwards' in a web page
    } mouseButtonTitles;

    /// @brief A virtual class that contains the basic aspects of a mouse's state that can be built off of for more specific uses
    class genericMouseState {
        protected:
            /// @brief x-position of the mouse
            Uint16 x;
            /// @brief y-position of the mouse
            Uint16 y;
            /// @brief Whether the mouse has moved or not
            bool motion = false;
            /// @brief A bitmask representing which of the 5 mouse buttons are pressed/released
            Uint8 buttons = 0;

        public:
            /// @brief bengine::genericMouseState constructor
            genericMouseState() {}
            /// @brief bengine::genericMouseState deconstructor
            ~genericMouseState() {}

            /** Update the mouse's position/motion; virtual function
             * @param event An SDL_Event used to update the mouse's position
             */
            virtual void updateMotion(const SDL_Event &event) = 0;

            /** Press a button (update bitmask) using an SDL_Event
             * @param event An SDL_Event used to determine the button to press
             */
            void pressButton(const SDL_Event &event) {
                this->buttons |= (Uint8)std::pow<Uint8, Uint8>(2, event.button.button - 1);
            }
            /** Press several buttons (update bitmask) using OR'd together elements from bengine::mouseButtonTitles
             * @param buttons A bitmask produced from bengine::mouseButtonTitles elements OR'd together
             */
            void pressButtons(const Uint8 &buttons) {
                this->buttons |= buttons;
            }
            /** Release a button (update bitmask) using an SDL_Event
             * @param event An SDL_Event used to determine the button to release
             */
            void releaseButton(const SDL_Event &event) {
                this->buttons &= ((Uint8)std::pow<Uint8, Uint8>(2, event.button.button - 1) ^ 255);
            }
            /** Release several buttons (update bitmask) using OR'd together elements from bengine::mouseButtonTitles
             * @param buttons A bitmask produced from bengine::mouseButtonTitles elements OR'd together
             */
            void releaseButtons(const Uint8 &buttons) {
                this->buttons &= (buttons ^ 255);
            }
            /** Do a general update based on SDL_Events
             * @param event The SDL_Event used to update the mouse's state
             */
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

            /** Get the x-position of the mouse
             * @returns The x-position of the mouse
             */
            Uint16 posx() const {
                return this->x;
            }
            /** Get the y-position of the mouse
             * @returns The y-position of the mouse
             */
            Uint16 posy() const {
                return this->y;
            }
            /** Determine whether the mouse has moved or not
             * @returns Whether the mouse has moved or not
             */
            bool moved() const {
                return this->motion;
            }
            /// @brief Set the mouse as no longer moving
            void stopMotion() {
                this->motion = false;
            }
            /** Determine if a set of buttons are pressed or not
             * @param buttons The set of buttons to check for (OR'd together from bengine::mouseButtonTitles)
             */
            bool pressed(const Uint8 &buttons) const {
                return (this->buttons & buttons) == buttons;
            }
    };

    /// @brief A mouse state that holds 2D data (most normal/common mouse state implementation)
    class normalMouseState : public genericMouseState {
        public:
            /// @brief bengine::normalMouseState constructor
            normalMouseState() {}
            /// @brief bengine::normalMouseState deconstructor
            ~normalMouseState() {}

            /** Update the mouse's position/motion, giving it a 2-D position within a window
             * @param event An SDL_Event used to update the mouse's position
             */
            void updateMotion(const SDL_Event &event) override {
                this->motion = true;
                this->x = event.motion.x;
                this->y = event.motion.y;
            }
    };

    /// @brief A mouse state that holds 3D data (applicable to implementations requiring a camera in a 3D world)
    class relativeMouseState : public genericMouseState {
        private:
            double sensitivity = 10;

            bool released = true;
            bool captured = false;

        public:
            relativeMouseState() {}
            ~relativeMouseState() {}

            void updateMotion(const SDL_Event &event) override {
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
            /// @brief x-position of the top-left corner of the rectangle
            Uint16 x1 = 0;
            /// @brief y-position of the top-left corner of the rectangle
            Uint16 y1 = 0;
            /// @brief x-position of the bottom-right corner of the rectangle
            Uint16 x2 = 0;
            /// @brief y-position of the bottom-left corner of the rectangle
            Uint16 y2 = 0;

        public:
            /** bengine::clickRectangle constructor
             * @param x1 x-position of the top-left corner of the rectangle
             * @param y1 y-position of the top-left corner of the rectangle
             * @param x2 x-position of the bottom-right corner of the rectangle
             * @param y2 y-position of the bottom-left corner of the rectangle
             */
            clickRectangle(const Uint16 x1 = 0, const Uint16 y1 = 0, const Uint16 x2 = 0, const Uint16 y2 = 0) {
                this->setAreaCorners(x1, y1, x2, y2);
            }
            /// @brief bengine::clickRectangle deconstructor
            ~clickRectangle() {}

            /** Get the x-position of the top-left corner of the rectangle
             * @returns The x-position of the top-left corner of the rectangle
             */
            Uint16 getX1() const {
                return this->x1;
            }
            /** Get the y-position of the top-left corner of the rectangle
             * @returns The y-position of the top-left corner of the rectangle
             */
            Uint16 getY1() const {
                return this->y1;
            }
            /** Get the x-position of the bottom-right corner of the rectangle
             * @returns The x-position of the bottom-right corner of the rectangle
             */
            Uint16 getX2() const {
                return this->x2;
            }
            /** Get the y-position of the bottom-right corner of the rectangle
             * @returns The y-position of the bottom-right corner of the rectangle
             */
            Uint16 getY2() const {
                return this->y2;
            }
            /** Set the x-position of the top-left corner of the rectangle
             * @param x The new x-position of the top-left corner of the rectangle
             * @returns The old x-position of the top-left corner of the rectangle
             */
            Uint16 setX1(const Uint16 &x) {
                const Uint16 output = this->x1;
                this->x1 = x;
                return output;
            }
            /** Set the y-position of the top-left corner of the rectangle
             * @param y The new y-position of the top-left corner of the rectangle
             * @returns The old y-position of the top-left corner of the rectangle
             */
            Uint16 setY1(const Uint16 &y) {
                const Uint16 output = this->y1;
                this->y1 = y;
                return output;
            }
            /** Set the x-position of the bottom-right corner of the rectangle
             * @param x The new x-position of the bottom-right corner of the rectangle
             * @returns The old x-position of the bottom-right corner of the rectangle
             */
            Uint16 setX2(const Uint16 &x) {
                const Uint16 output = this->x2;
                this->x2 = x;
                return output;
            }
            /** Set the y-position of the bottom-right corner of the rectangle
             * @param y The new y-position of the bottom-right corner of the rectangle
             * @returns The old y-position of the bottom-right corner of the rectangle
             */
            Uint16 setY2(const Uint16 &y) {
                const Uint16 output = this->y2;
                this->y2 = y;
                return output;
            }
            /** Change the position of all four corners of the rectangle
             * @param x1 The new x-position of the top-left corner of the rectangle
             * @param y1 The new y-position of the top-left corner of the rectangle
             * @param x2 The new x-position of the bottom-right corner of the rectangle
             * @param x2 The new y-position of the bottom-right corner of the rectangle
             */
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

            /** Get the x-position of the top-left corner of the rectangle
             * @returns The x-position of the top-left corner of the rectangle
             */
            Uint16 getX() const {
                return this->x1;
            }
            /** Get the y-position of the top-left corner of the rectangle
             * @returns The y-position of the top-left corner of the rectangle
             */
            Uint16 getY() const {
                return this->y1;
            }
            /** Get the width of the rectangle
             * @returns The width of the rectangle
             */
            Uint16 getW() const {
                return this->x2 - this->x1;
            }
            /** Get the height of the rectangle
             * @returns The height of the rectangle
             */
            Uint16 getH() const {
                return this->y2 - this->y1;
            }
            /** Set the x-position of the top-left corner of the rectangle while maintaining the rectangle's width
             * @param x The new x-position of the top-left corner of the rectangle
             * @returns The old x-position of the top-left corner of the rectangle
             */
            Uint16 setX(const Uint16 &x) {
                const Uint16 output = this->setX1(x);
                this->x2 = this->x1 + this->getW();
                return output;
            }
            /** Set the y-position of the top-left corner of the rectangle while maintaining the rectangle's height
             * @param y The new y-position of the top-left corner of the rectangle
             * @returns The old y-position of the top-left corner of the rectangle
             */
            Uint16 setY(const Uint16 &y) {
                const Uint16 output = this->setY1(y);
                this->y2 = this->y1 + this->getH();
                return output;
            }
            /** Set the width of the rectangle (top-left corner remains the same, bottom-right corner moves)
             * @param w The new width of the rectangle
             * @returns The old width of the rectangle
             */
            Uint16 setW(const Uint16 &w) {
                const Uint16 output = this->getW();
                this->x2 = this->x1 + w;
                return output;
            }
            /** Set the height of the rectangle (top-left corner remains the same, bottom-right corner moves)
             * @param h The new height of the rectangle
             * @returns The old height of the rectangle
             */
            Uint16 setH(const Uint16 &h) {
                const Uint16 output = this->getH();
                this->y2 = this->y1 + h;
                return output;
            }
            /** Set the position of the top-left corner of the rectangle as well as its width and height
             * @param x The new x-position of the top-left corner of the rectangle
             * @param y The new y-position of the top-left corner of the rectangle
             * @param w The new width of the rectangle
             * @param h The new height of the rectangle
             */
            void setAreaRect(const Uint16 x, const Uint16 y, const Uint16 &w, const Uint16 &h) {
                this->x1 = x;
                this->x2 = x + w;
                this->y1 = y;
                this->y2 = y + h;
            }

            /** Check to see if the mouse is within the rectangle
             * @param mouseState The mouse's state
             * @returns Whether the mouse is within the rectangle or not
             */
            bool checkPos(const normalMouseState &mouseState) const {
                if (this->getW() == 0 || this->getH() == 0) {
                    return false;
                }
                return mouseState.posx() >= this->x1 && mouseState.posy() >= this->y1 && mouseState.posx() <= this->x2 && mouseState.posy() <= this->y2;
            }
            /** Check to see if the mouse is within the rectangle and clicked the correct button
             * @param mouseState The mouse's state
             * @param button Which buttons to check for (OR'd together from bengine::mouseButtonTitles)
             * @returns Whether the mouse is both within the rectangle and clicked the right button or not
             */
            bool checkButton(const normalMouseState &mouseState, const Uint8 &button) const {
                if (!mouseState.pressed(button)) {
                    return false;
                }
                return this->checkPos(mouseState);
            }
    };

    class clickCircle {
        protected:
            /// @brief x-position of the center of the circle
            Uint16 x = 0;
            /// @brief y-position of the center of the circle
            Uint16 y = 0;
            /// @brief Radius of the circle
            Uint16 radius = 0;

        public:
            /** @brief bengine::clickCircle constructor
             * @param x x-position of the center of the circle
             * @param y y-position of the center of the circle
             * @param radius Radius of the circle
             */
            clickCircle(const Uint16 &x = 0, const Uint16 &y = 0, const Uint16 &radius = 0) : x(x), y(y), radius(radius) {}
            /// @brief bengine::clickCircle deconstructor
            ~clickCircle() {}

            /** Get the x-position of the center of the circle
             * @returns The x-position of the center of the circle
             */
            Uint16 getX() const {
                return this->x;
            }
            /** Get the y-position of the center of the circle
             * @returns The y-position of the center of the circle
             */
            Uint16 getY() const {
                return this->y;
            }
            /** Get the radius of the circle
             * @returns The radius of the circle
             */
            Uint16 getRadius() const {
                return this->radius;
            }
            /** Set the x-position of the center of the circle
             * @param x The new x-position of the center of the circle
             * @returns The old x-position of the center of the circle
             */
            Uint16 setX(const Uint16 &x) {
                const Uint16 output = this->x;
                this->x = x;
                return output;
            }
            /** Set the y-position of the center of the circle
             * @param x The new y-position of the center of the circle
             * @returns The old y-position of the center of the circle
             */
            Uint16 setY(const Uint16 &y) {
                const Uint16 output = this->y;
                this->y = y;
                return output;
            }
            /** Set the radius of the circle
             * @param x The new radius of the circle
             * @returns The old radius of the circle
             */
            Uint16 setRadius(const Uint16 &radius) {
                const Uint16 output = this->radius;
                this->radius = radius;
                return output;
            }

            /** Check to see if the mouse is within the circle
             * @param mouseState The mouse's state
             * @returns Whether the mouse is within the circle or not
             */
            bool checkPos(const normalMouseState &mouseState) const {
                if (this->radius == 0) {
                    // A radius of zero will still let you click on the center of the circle
                    return mouseState.posx() == this->x && mouseState.posy() == this->y;
                }
                // First a square is checked to save on some math in the most common cases, then the circle is checked if the mouse is within the matching square
                if (mouseState.posx() < this->x - radius || mouseState.posx() > this->x + radius || mouseState.posy() < this->y - radius || mouseState.posy() > this->y + radius) {
                    return false;
                }
                const double root = std::round(std::sqrt(this->radius * this->radius - (mouseState.posx() - this->x) * (mouseState.posx() - this->x)));
                return mouseState.posy() < this->y + root && mouseState.posy() > this->y - root;
            }
            /** Check to see if the mouse is within the circle and clicked the correct button
             * @param mouseState The mouse's state
             * @param button Which buttons to check for (OR'd together from bengine::mouseButtonTitles)
             * @returns Whether the mouse is both within the circle and clicked the right button or not
             */
            bool checkButton(const normalMouseState &mouseState, const Uint8 &button) const {
                if (!mouseState.pressed(button)) {
                    return false;
                }
                return this->checkPos(mouseState);
            }
    };

    class clickMatrix : public bengine::clickRectangle {
        protected:
            /// @brief The amount of rows in the matrix
            Uint16 rows = 1;
            /// @brief The amount of columns in the matrix
            Uint16 cols = 1;
            /// @brief A width for the matrix that allows for width / cols without a remainder; greater than or equal to the value of the regular width of the matrix
            Uint16 evenWidth = 1;
            /// @brief A height for the matrix that allows for height / rows without a remainder; greater than or equal to the value of the regular height of the matrix
            Uint16 evenHeight = 1;

            /// @brief Update the matrix's evenWidth
            void updateEvenWidth() {
                this->evenWidth = btils::lcm<Uint16>(this->getW(), this->cols);
            }
            /// @brief Update the matrix's evenHeight
            void updateEvenHeight() {
                this->evenHeight = btils::lcm<Uint16>(this->getH(), this->rows);
            }

        public:
            /** bengine::clickMatrix constructor
             * @param x1 x-position of the top-left corner of the rectangle
             * @param y1 y-position of the top-left corner of the rectangle
             * @param x2 x-position of the bottom-right corner of the rectangle
             * @param y2 y-position of the bottom-left corner of the rectangle
             * @param rows The amount of rows that the matrix will have
             * @param cols The amount of columns that the matrix will have
             */
            clickMatrix(const Uint16 x1 = 0, const Uint16 y1 = 0, const Uint16 x2 = 0, const Uint16 y2 = 0, const Uint16 &rows = 1, const Uint16 &cols = 1) : clickRectangle(x1, y1, x2, y2) {
                this->setRows(rows);
                this->setCols(cols);
                this->updateEvenWidth();
                this->updateEvenHeight();
            }
            /// @brief bengine::clickMatrix deconstructor
            ~clickMatrix() {}

            /** Get the amount of rows in the matrix
             * @returns The amount of rows in the matrix
             */
            Uint16 getRows() const {
                return this->rows;
            }
            /** Get the amount of columns in the matrix
             * @returns The amount of columns in the matrix
             */
            Uint16 getCols() const {
                return this->cols;
            }
            /** Get the amount of rows in the matrix
             * @param rows The new amount of rows for the matrix
             * @returns The old amount of rows in the matrix
             */
            Uint16 setRows(const Uint16 &rows) {
                const Uint16 output = this->rows;
                if (rows > this->getH()) {
                    this->rows = this->getH();
                } else {
                    this->rows = rows;
                }
                return output;
            }
            /** Get the amount of columns in the matrix
             * @param cols The new amount of columns for the matrix
             * @returns The old amount of columns in the matrix
             */
            Uint16 setCols(const Uint16 &cols) {
                const Uint16 output = this->cols;
                if (cols > this->getW()) {
                    this->cols = this->getW();
                } else {
                    this->cols = cols;
                }
                return output;
            }
            /** Get the width for the matrix that allows for width / cols without a remainder; greater than or equal to the value of the regular width of the matrix
             * @returns The even width of the matrix
             */
            Uint16 getEvenWidth() const {
                return this->evenWidth;
            }
            /** Get the height for the matrix that allows for height / rows without a remainder; greater than or equal to the value of the regular height of the matrix
             * @returns The even height of the matrix
             */
            Uint16 setEvenHeight() const {
                return this->evenHeight;
            }

            /** Convert a positional integer to one representing its matching row within the matrix
             * @param pos The positional integer to convert into a row index
             * @returns A row index matching the inputted position and this matrix's dimensions
             */
            Uint16 toRow(const Uint32 &pos) const {
                return pos / this->cols;
            }
            /** Convert a positional integer to one representing its matching column within the matrix
             * @param pos The positional integer to convert into a column index
             * @returns A column index matching the inputted position and this matrix's dimensions
             */
            Uint16 toCol(const Uint32 &pos) const {
                return pos % this->cols;
            }
            /** Convert a positional integer to one representing its matching row within a matrix
             * @param pos The positional integer to convert into a row index
             * @param cols The amount of columns in the matrix
             * @returns A row index matching the inputted position and the matrix's dimensions
             */
            static Uint16 toRow(const Uint32 &pos, const Uint16 &cols) {
                return pos / cols;
            }
            /** Convert a positional integer to one representing its matching column within a matrix
             * @param pos The positional integer to convert into a column index
             * @param cols The amount of columns in the matrix
             * @returns A column index matching the inputted position and the matrix's dimensions
             */
            static Uint16 toCol(const Uint32 &pos, const Uint16 &cols) {
                return pos % cols;
            }

            /** Set the x-position of the top-left corner of the matrix
             * @param x The new x-position of the top-left corner of the matrix
             * @returns The old x-position of the top-left corner of the matrix
             */
            Uint16 setX1(const Uint16 &x) {
                const Uint16 output = bengine::clickRectangle::setX1(x);
                this->updateEvenWidth();
                return output;
            }
            /** Set the y-position of the top-left corner of the matrix
             * @param y The new y-position of the top-left corner of the matrix
             * @returns The old y-position of the top-left corner of the matrix
             */
            Uint16 setY1(const Uint16 &y) {
                const Uint16 output = bengine::clickRectangle::setY1(y);
                this->updateEvenHeight();
                return output;
            }
            /** Set the x-position of the bottom-right corner of the matrix
             * @param x The new x-position of the bottom-right corner of the matrix
             * @returns The old x-position of the bottom-right corner of the matrix
             */
            Uint16 setX2(const Uint16 &x) {
                const Uint16 output = bengine::clickRectangle::setX2(x);
                this->updateEvenWidth();
                return output;
            }
            /** Set the y-position of the bottom-right corner of the matrix
             * @param y The new y-position of the bottom-right corner of the matrix
             * @returns The old y-position of the bottom-right corner of the matrix
             */
            Uint16 setY2(const Uint16 &y) {
                const Uint16 output = bengine::clickRectangle::setY2(y);
                this->updateEvenHeight();
                return output;
            }
            /** Change the position of all four corners of the matrix
             * @param x1 The new x-position of the top-left corner of the matrix
             * @param y1 The new y-position of the top-left corner of the matrix
             * @param x2 The new x-position of the bottom-right corner of the matrix
             * @param x2 The new y-position of the bottom-right corner of the matrix
             */
            void setAreaCorners(const Uint16 x1, const Uint16 y1, const Uint16 x2, const Uint16 y2) {
                bengine::clickRectangle::setAreaCorners(x1, y1, x2, y2);
                this->updateEvenWidth();
                this->updateEvenHeight();
            }

            /** Set the width of the matrix (top-left corner remains the same, bottom-right corner moves)
             * @param w The new width of the matrix
             * @returns The old width of the matrix
             */
            Uint16 setW(const Uint16 &w) {
                const Uint16 output = bengine::clickRectangle::setW(w);
                this->updateEvenWidth();
                return output;
            }
            /** Set the height of the matrix (top-left corner remains the same, bottom-right corner moves)
             * @param h The new height of the matrix
             * @returns The old height of the matrix
             */
            Uint16 setH(const Uint16 &h) {
                const Uint16 output = bengine::clickRectangle::setH(h);
                this->updateEvenHeight();
                return output;
            }
            /** Set the position of the top-left corner of the matrix as well as its width and height
             * @param x The new x-position of the top-left corner of the matrix
             * @param y The new y-position of the top-left corner of the matrix
             * @param w The new width of the matrix
             * @param h The new height of the matrix
             */
            void setAreaRect(const Uint16 x, const Uint16 y, const Uint16 &w, const Uint16 &h) {
                bengine::clickRectangle::setAreaRect(x, y, w, h);
                this->updateEvenWidth();
                this->updateEvenHeight();
            }

            /** Find the cell within the matrix that the mouse is located in
             * @param mouseState The mouse's state
             * @returns The cell within the matrix that the mouse is located in
             */
            Uint32 checkPos(const normalMouseState &mouseState) const {
                if (this->getW() == 0 || this->getH() == 0) {
                    return UINT32_MAX;
                }
                if (mouseState.posx() < this->x1 || mouseState.posx() > this->x2 || mouseState.posy() < this->y1 || mouseState.posy() > this->y2) {
                    return UINT32_MAX;
                }
                const Uint16 col = ((mouseState.posx() - this->x1) * (double)(evenWidth / this->getW())) / (evenWidth / this->cols);
                const Uint16 row = ((mouseState.posy() - this->y1) * (double)(evenHeight / this->getH())) / (evenHeight / this->rows);

                return (row >= this->rows ? this->rows - 1 : row) * this->cols + (col >= this->cols ? this->cols - 1 : col);
            }
            /** Find the cell within the matrix that the mouse is located in and confirm that the mouse has clicked the correct button
             * @param mouseState The mouse's state
             * @param button Which buttons to check for (OR'd together from bengine::mouseButtonTitles)
             * @returns The cell within the matrix that the mouse is located in if the mouse has clicked the correct button; UINT32_MAX as a sentinal value otherwise
             */
            Uint32 checkButton(const normalMouseState &mouseState, const Uint8 &button) const {
                const Uint32 pos = this->checkPos(mouseState);
                return mouseState.pressed(button) ? pos : UINT32_MAX;
            }
    };
}

#endif // BENGINE_MOUSE_hpp
