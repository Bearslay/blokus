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

        /** Print the grid (cut to content and with formatting)
         * 
         * 0's are blank space and 1's are tiles
         * Colons mark the end of a row for a piece and semi-colons mark the end of the last row for a piece
         * 
         * If the grid is either blank or has a size of zero, then "1;" is printed as a fall-back
         * 
         * Example:
         * ░░░░░
         * ░░█░░
         * ░███░ -> 010:111:001;
         * ░░░█░
         * ░░░░░
         */
        void printGrid() {
            std::vector<std::vector<char>> output = this->grid;

            if (output.size() < 1) {
                std::cout << "1;\n";
                return;
            }

            // Catch empty grids; convert it into a single tile
            for (Uint8 i = 0; i < output.size(); i++) {
                bool halt = false;
                for (Uint8 j = 0; j < output.at(i).size(); j++) {
                    if (output.at(i).at(j) >= 0) {
                        halt = true;
                        break;
                    }
                }
                if (halt) {
                    break;
                }
                if (i == output.size() - 1) {
                    std::cout << "1;\n";
                    return;
                }
            }

            // Remove empty rows
            for (std::size_t i = output.size(); i--;) {
                Uint8 rowSum = 0;
                for (std::size_t j = 0; j < output.at(i).size(); j++) {
                    if (output.at(i).at(j) >= 0) {
                        rowSum++;
                    }
                }
                if (rowSum == 0) {
                    output.erase(output.begin() + i);
                }
            }
            // Remove empty columns
            for (std::size_t i = output.at(0).size(); i--;) {
                Uint8 colSum = 0;
                for (std::size_t j = 0; j < output.size(); j++) {
                    if (output.at(j).at(i) >= 0) {
                        colSum++;
                    }
                }
                if (colSum > 0) {
                    continue;
                }
                for (std::size_t j = 0; j < output.size(); j++) {
                    output[j].erase(output.at(j).begin() + i);
                }
            }

            // Print the grid
            for (Uint8 i = 0; i < output.size(); i++) {
                for (Uint8 j = 0; j < output.at(i).size(); j++) {
                    std::cout << (output.at(i).at(j) >= 0);
                }
                std::cout << (i < output.size() - 1 ? ":" : ";");
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
            this->window.renderText(this->font, u"PRINT", 60, 536, 0);
            this->window.renderText(this->font, u"CLEAR", 280, 536, 0);

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
