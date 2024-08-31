#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#include "btils.hpp"
#include "bengine.hpp"

class polymaker : public bengine::loop {
    private:
        bengine::normalMouseState mstate;
        std::vector<std::vector<char>> grid;
        Uint32 gridPos = UINT32_MAX;
        Uint32 oldGridPos = UINT32_MAX;

        bengine::basicTexture piecesOutline = bengine::basicTexture(this->window.loadTexture("dev/png/tilesets/piece_edges_sheet.png"), {0, 0, 256, 256});
        bengine::moddedTexture piecesBase = bengine::moddedTexture(this->window.loadTexture("dev/png/tilesets/piece_bases_sheet.png"), {0, 0, 256, 256}, {255, 0, 0, 255});

        bengine::clickRectangle printButton = bengine::clickRectangle(20, 520, 220, 600);
        bengine::clickRectangle clearButton = bengine::clickRectangle(240, 520, 440, 600);
        bengine::clickMatrix gridButton;

        bengine::autotiler tiler;

        TTF_Font *font = TTF_OpenFont("dev/fonts/GNU-Unifont.ttf", 48);

        void printGrid() {
            for (Uint8 i = 0; i < this->grid.size(); i++) {
                for (Uint8 j = 0; j < this->grid.size(); j++) {
                    std::cout << (this->grid.at(i).at(j) >= 0);
                }
                std::cout << "\n";
            }
            std::cout << "\n";
        }

        void clearGrid() {
            for (Uint8 i = 0; i < this->grid.size(); i++) {
                for (Uint8 j = 0; j < this->grid.size(); j++) {
                    this->grid[i][j] = -1;
                }
            }
        }

        void handleEvent() {
            switch (this->event.type) {
                case SDL_MOUSEMOTION:
                    this->mstate.updateMotion(this->event);
                    this->oldGridPos = this->gridPos;
                    this->gridPos = this->gridButton.checkPos(this->mstate);
                    if (this->gridPos != this->oldGridPos && this->mstate.pressed(bengine::MOUSE1)) {
                        if (this->gridPos < this->grid.size() * this->grid.size()) {
                            tiler.fourBit(this->grid, this->gridPos % this->grid.size(), this->gridPos / this->grid.size(), this->grid[this->gridPos / this->grid.size()][this->gridPos % this->grid.size()] < 0);
                            this->visualsChanged = true;
                        }
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    this->mstate.pressButton(this->event);
                    if (this->printButton.checkButton(this->mstate, bengine::MOUSE1)) {
                        this->printGrid();
                    } else if (this->clearButton.checkButton(this->mstate, bengine::MOUSE1)) {
                        this->clearGrid();
                        this->visualsChanged = true;
                    } else {
                        if (this->gridPos != UINT32_MAX && this->gridPos < this->grid.size() * this->grid.size()) {
                            tiler.fourBit(this->grid, this->gridPos % this->grid.size(), this->gridPos / this->grid.size(), this->grid[this->gridPos / this->grid.size()][this->gridPos % this->grid.size()] < 0);
                            this->visualsChanged = true;
                        }
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    this->mstate.releaseButton(this->event);
                    break;
                case SDL_KEYDOWN:
                    if (!this->event.key.repeat) {
                        switch (this->event.key.keysym.scancode) {
                            case SDL_SCANCODE_SPACE:
                                this->printGrid();
                                break;
                            default:
                                break;
                        }
                    }
            }
        }

        void compute() {}

        void render() {
            this->window.drawThickRectangle(20, 520, 200, 80, 10, THICKSHAPE_INNER);
            this->window.drawThickRectangle(240, 520, 200, 80, 10, THICKSHAPE_INNER);
            this->window.renderText(this->font, u"PRINT", {60, 536}, 0);
            this->window.renderText(this->font, u"CLEAR", {280, 536}, 0);

            const Uint16 cellSize = 480 / this->grid.size();
            for (Uint8 i = 0; i < this->grid.size(); i++) {
                for (Uint8 j = 0; j < this->grid.size(); j++) {
                    this->window.drawRectangle(j * cellSize, i * cellSize, cellSize, cellSize, bengine::colors[bengine::COLOR_WHITE]);
                    if (this->grid.at(i).at(j) < 0) {
                        continue;
                    }
                    this->piecesBase.setFrame({this->grid.at(i).at(j) % 4 * 64, this->grid.at(i).at(j) / 4 * 64, 64, 64});
                    this->piecesOutline.setFrame({this->grid.at(i).at(j) % 4 * 64, this->grid.at(i).at(j) / 4 * 64, 64, 64});
                    this->window.renderModdedTexture(this->piecesBase, {j * cellSize, i * cellSize, cellSize, cellSize});
                    this->window.renderBasicTexture(this->piecesOutline, {j * cellSize, i * cellSize, cellSize, cellSize});
                }
            }
        }

    public:
        polymaker(const Uint8 &dimensions) : bengine::loop("Polyomino Maker", 480, 640, SDL_WINDOW_SHOWN) {
            // 30 fps
            this->deltaTime = 0.03333333;

            // No need to waste resources on stretching things
            this->window.setGraphicalStretching(false);

            for (Uint8 i = 0; i < dimensions; i++) {
                this->grid.emplace_back();
                for (Uint8 j = 0; j < dimensions; j++) {
                    this->grid[i].emplace_back(-1);
                }
            }

            this->gridButton = bengine::clickMatrix(0, 0, 480, 480, dimensions, dimensions);
        }
        ~polymaker() {
            TTF_CloseFont(this->font);
            this->font = nullptr;
        }
};

int main(int argc, char* args[]) {
    polymaker p(8);
    p.run();
}
