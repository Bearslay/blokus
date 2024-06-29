#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#include "btils.hpp"
#include "bengine.hpp"

class mainLoop : public bengine::loop {
    private:
        bengine::basicTexture tile = bengine::basicTexture(this->window.loadTexture("dev/png/tile.png"), {0, 0, 64, 64});
        bengine::basicTexture cell = bengine::basicTexture(this->window.loadTexture("dev/png/empty_cell.png"), {0, 0, 64, 64});
        bengine::basicTexture texture;
        bengine::basicTexture texture2;

        unsigned char boardSize = 50;

        void handleEvent() {

        }
        void render() {
            this->window.renderBasicTexture(this->texture, {0, 0, 960, 960});
            // this->window.renderBasicTexture(this->texture2, {0, 0, 960, 960});
        }
        void compute() {
            this->visualsChanged = true;
        }

    public:
        mainLoop() : bengine::loop("window", 960, 960, SDL_WINDOW_SHOWN) {
            this->window.targetTexture(this->texture.getTexture(), boardSize * 64, boardSize * 64);
            
            this->window.clear();
            for (unsigned char i = 0; i < boardSize; i++) {
                for (unsigned char j = 0; j < boardSize; j++) {
                    this->window.renderBasicTexture(this->cell, {j * 64, i * 64, 64, 64});
                }
            }
            this->window.present();
            this->texture.setFrame({0, 0, boardSize * 64, boardSize * 64});
            
            this->window.targetTexture(this->texture2.getTexture(), boardSize * 64, boardSize * 64);
            
            this->window.clear();
            for (unsigned char i = 0; i < boardSize; i++) {
                for (unsigned char j = 0; j < boardSize; j++) {
                    if (i % 2 == 0 && j % 2 == 0) {   
                        this->window.renderBasicTexture(this->tile, {j * 64, i * 64, 64, 64});
                    }
                }
            }
            this->window.present();
            this->texture2.setFrame({0, 0, boardSize * 64, boardSize * 64});
           
            this->window.targetWindow();
            this->window.clear();
        }
};

int main() {
    mainLoop loop;
    loop.run();
}
