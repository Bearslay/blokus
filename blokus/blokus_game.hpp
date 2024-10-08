#ifndef BLOKUS_GAME_hpp
#define BLOKUS_GAME_hpp

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>

#include "btils.hpp"
#include "bengine.hpp"

#include "blokus_polyominoes.hpp"
#include "blokus_piece.hpp"
#include "blokus_player.hpp"

namespace blokus {
    class game : public bengine::loop {
        private:
            bengine::normalMouseState mstate;

            std::vector<blokus::player> players = {};
            Uint8 pieceSets[6] = {1, 0, 0, 0, 0, 0};
            Uint8 turn = 0;

            /** Store the player and bitmask for each cell
             * 
             * A value of 0 indicates an empty tile, then the player on each tile can be found by checking the range of the tile's value; 1 = [1, 16], 2 = [17, 32], 3 = [33, 48], 4 = [49, 64]
             * 
             * The bitmask of each tile (used for autotiling) is determined by "bitmask = value - (playerid * 16) - 1"
             */
            std::vector<std::vector<Uint8>> board = {};

            TTF_Font* font_general = TTF_OpenFont("dev/fonts/GNU-Unifont.ttf", 35);
            TTF_Font* font_pageInfo = TTF_OpenFont("dev/fonts/GNU-Unifont.ttf", 32);

            bengine::basicTexture texture_background = bengine::basicTexture(this->window.loadTexture("dev/png/background.png"), {0, 0, 1920, 1080});
            bengine::basicTexture texture_boardframe = bengine::basicTexture(this->window.loadTexture("dev/png/boardframe.png"), {0, 0, 1064, 1064});
            bengine::basicTexture texture_emptyCell = bengine::basicTexture(this->window.loadTexture("dev/png/empty_cell.png"), {0, 0, 64, 64});
            bengine::basicTexture texture_grid;

            bengine::moddedTexture texture_piece_base = bengine::moddedTexture(this->window.loadTexture("dev/png/tilesets/piece_bases_sheet.png"), {0, 0, 256, 256}, {255, 0, 0, 255});
            bengine::basicTexture texture_piece_edge = bengine::basicTexture(this->window.loadTexture("dev/png/tilesets/piece_edges_sheet.png"), {0, 0, 256, 256});
            bengine::autotiler tiler;

            bengine::basicTexture texture_playerframe_small = bengine::basicTexture(this->window.loadTexture("dev/png/playerframe_small.png"), {0, 0, 800, 260});
            bengine::basicTexture texture_playerframe_large = bengine::basicTexture(this->window.loadTexture("dev/png/playerframe_large.png"), {0, 0, 800, 1064});
            bengine::moddedTexture texture_shaded_frame = bengine::moddedTexture(this->window.loadTexture("dev/png/shaded_frame.png"), {0, 0, 192, 192});

            bengine::clickMatrix gridClickArea;
            std::string textInput = "";

            bengine::paddedGrid piecesPreviewGrid = bengine::paddedGrid(766, 881, 5, 6, 4, 4, bengine::ALIGN_CTR_CTR);
            Uint8 piecesPreviewPage = 0;

            Uint16 maxPieces() const {
                Uint16 output = 0;
                for (blokus::polyType i = blokus::POLYTYPE_BASE; i <= blokus::POLYTYPE_DEC; i++) {
                    output += this->pieceSets[i] * blokus::polyominoAmounts[i];
                }
                return output;
            }
            Uint16 maxTiles() const {
                Uint16 output = 0;
                for (polyType i = blokus::POLYTYPE_BASE; i <= blokus::POLYTYPE_DEC; i++) {
                    output += this->pieceSets[i] * blokus::polyominoTileTotals[i];
                }
                return output;
            }

            void clearBoard() {
                for (Uint8 i = 0; i < this->board.size(); i++) {
                    for (Uint8 j = 0; j < this->board.size(); j++) {
                        this->board[i][j] = 0;
                    }
                }
            }

            void handleEvent() override {
                Uint32 gridpos;
                switch (this->event.type) {
                    case SDL_MOUSEMOTION:
                        this->mstate.update(this->event);
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        this->mstate.pressButton(this->event);
                        if (false /* click on a text box or smth */ ) {
                            if (SDL_IsTextInputActive()) {
                                SDL_StopTextInput();
                            } else {
                                SDL_StartTextInput();
                            }
                            this->visualsChanged = true;
                        }
                        gridpos = this->gridClickArea.checkButton(this->mstate, bengine::MOUSE1);
                        if (gridpos != UINT32_MAX && this->board.at(gridpos % this->board.size()).at(gridpos / this->board.size()) == 0) {
                            std::vector<std::vector<char>> playerMask;
                            for (Uint8 i = 0; i < this->board.size(); i++) {
                                playerMask.emplace_back();
                                for (Uint8 j = 0; j < this->board.size(); j++) {
                                    if (this->board.at(i).at(j) < this->turn * 16 || this->board.at(i).at(j) > (this->turn + 1) * 16) {
                                        playerMask[i].emplace_back(-1);
                                        continue;
                                    }
                                    playerMask[i].emplace_back(this->board.at(i).at(j) - (this->turn * 16) - 1);
                                }
                            }
                            this->tiler.fourBit(playerMask, gridpos % this->board.size(), gridpos / this->board.size(), true, false);

                            for (Uint8 i = 0; i < this->board.size(); i++) {
                                for (Uint8 j = 0; j < this->board.size(); j++) {
                                    if (playerMask.at(i).at(j) < 0) {
                                        continue;
                                    }
                                    this->board[i][j] = (playerMask.at(i).at(j) + 1) + (16 * turn);
                                }
                            }

                            this->updateBoardTexture();
                            this->visualsChanged = true;
                        }
                        break;
                    case SDL_MOUSEBUTTONUP:
                        this->mstate.releaseButton(this->event);
                        break;
                    case SDL_TEXTINPUT:
                        textInput += this->event.text.text;
                        this->visualsChanged = true;
                        break;
                    case SDL_KEYDOWN:
                        if (SDL_IsTextInputActive()) {
                            switch (this->event.key.keysym.scancode) {
                                case SDL_SCANCODE_RETURN:
                                    textInput += "\n";
                                    this->visualsChanged = true;
                                    break;
                                case SDL_SCANCODE_BACKSPACE:
                                    if (textInput.size() > 0) {
                                        textInput.pop_back();
                                        this->visualsChanged = true;
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        if (keystate[SDL_SCANCODE_SPACE]) {
                            this->turn++;
                            this->turn %= this->players.size();
                            this->visualsChanged = true;
                        }
                        break;
                    default:
                        break;
                }
            }

            void compute() override {
                this->mstate.stopMotion();
            }

            void renderPlayer(const Uint8 &id) {
                const Uint16 xpos = 20;
                const Uint16 ypos = 8 + 268 * id;
                const char16_t* setTitles[6] = {u"Base:        ", u"Hexominoes:  ", u"Heptominoes: ", u"Octominoes:  ", u"Nonominoes:  ", u"Decominoes:  "};

                this->window.renderBasicTexture(this->texture_playerframe_small, {xpos, ypos, 800, 260});
                this->window.renderText(this->font_general, (u"Player " + btils::to_u16string<Uint8>(id + 1) + u" - " + this->players.at(id).getName()).c_str(), xpos + 12, ypos + 10, 0, bengine::colors[bengine::COLOR_WHITE]);
            
                this->window.renderText(this->font_general, (u"Tiles Left: " + btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->players.at(id).getRemainingTiles(blokus::POLYTYPE_SENTINAL), 4, 0) + "/" + btils::tstr_AddZeros<Uint16>(this->maxTiles(), 4, 0) + " (" + btils::tstr_AddZeros<Uint16>((this->maxTiles() - this->players.at(id).getRemainingTiles(blokus::POLYTYPE_SENTINAL)) / this->maxTiles() * 100, 3, 0) + "%)")).c_str(), xpos + 12, ypos + 50, 0, bengine::colors[bengine::COLOR_WHITE]);
                for (polyType i = blokus::POLYTYPE_BASE; i <= blokus::POLYTYPE_OCT; i++) {
                    if (this->pieceSets[i] > 0) {
                        this->window.renderText(this->font_general, (setTitles[i] + btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->players.at(id).getRemainingPieces(i), 3, 0) + "/" + btils::tstr_AddZeros<Uint16>(this->pieceSets[i] * blokus::polyominoAmounts[i], 3, 0) + "  (" + btils::tstr_AddZeros<Uint16>((this->pieceSets[i] * blokus::polyominoAmounts[i] - this->players.at(id).getRemainingPieces(i)) / (this->pieceSets[i] * blokus::polyominoAmounts[i]) * 100, 3, 0) + "%)")).c_str(), xpos + 12, ypos + 92 + 36 * (int)i, 0, bengine::colors[bengine::COLOR_WHITE]);
                    }
                }
                this->window.renderModdedTexture(this->texture_shaded_frame, {xpos + 559, ypos + 53, 192, 192});
            }
            void renderPiecePreview(const Uint8 &id) {
                this->window.renderBasicTexture(this->texture_playerframe_large, {20, 8, 800, 1064});
                this->window.renderText(this->font_general, (u"Player " + btils::to_u16string<Uint8>(id + 1) + u" - " + this->players.at(id).getName()).c_str(), 32, 18, 0, bengine::colors[bengine::COLOR_WHITE]);
                this->window.renderText(this->font_pageInfo, (u"Showing Pieces " + btils::to_u16string(btils::tstr_AddZeros<Uint8>(1, 4, 0)) + u"-" + btils::to_u16string(btils::tstr_AddZeros<Uint8>(21, 4, 0)) + u"/" + btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->maxPieces(), 4, 0)) + u" on Page " + btils::to_u16string(btils::tstr_AddZeros<Uint8>(this->piecesPreviewPage + 1, 3, 0)) + u"/" + btils::to_u16string(btils::tstr_AddZeros<Uint8>(this->maxPieces() / (this->piecesPreviewGrid.getRows() * this->piecesPreviewGrid.getCols()) + 1, 3, 0))).c_str(), 76, 1025, 0, bengine::colors[bengine::COLOR_WHITE]);

                // Uint16 pieceId = 0;
                const Uint8 previewCells = this->piecesPreviewGrid.getRows() * this->piecesPreviewGrid.getCols();
                Uint8 maxPages = 0;
                for (blokus::polyType i = blokus::POLYTYPE_BASE; i <= blokus::POLYTYPE_DEC; i++) {
                    if (this->pieceSets[i] < 1) {
                        continue;
                    }
                    maxPages += this->pieceSets[i] * blokus::polyominoAmounts[i] / previewCells + 1;
                }

                for (Uint8 i = 0; i < this->piecesPreviewGrid.getRows(); i++) {
                    for (Uint8 j = 0; j < this->piecesPreviewGrid.getCols(); j++) {
                        this->texture_shaded_frame.setColorMod({(Uint8)(std::rand() % 255), (Uint8)(std::rand() % 255), (Uint8)(std::rand() % 255), 255});
                        this->window.renderModdedTexture(this->texture_shaded_frame, {(int)(36 + this->piecesPreviewGrid.getXOffset() + j * (this->piecesPreviewGrid.getCellWidth() + this->piecesPreviewGrid.getGapWidth())), (int)(139 + this->piecesPreviewGrid.getYOffset() + i * (this->piecesPreviewGrid.getCellHeight() + this->piecesPreviewGrid.getGapHeight())), (int)this->piecesPreviewGrid.getCellWidth(), (int)this->piecesPreviewGrid.getCellHeight()});
                    }
                }
            }

            void updateBoardTexture() {
                this->window.targetDummy();
                this->window.initDummy(this->board.size() * 64, this->board.size() * 64);
                this->window.clear();

                this->window.renderBasicTexture(this->texture_background, {0, 0, (int)this->board.size() * 64, (int)this->board.size() * 64});
                for (Uint8 i = 0; i < this->board.size(); i++) {
                    for (Uint8 j = 0; j < this->board.size(); j++) {
                        // Render empty tile
                        if (this->board.at(i).at(j) == 0) {
                            this->window.renderBasicTexture(this->texture_emptyCell, {j * 64, i * 64, 64, 64});
                            continue;
                        }
                        // Determine the id of the player occupying the cell & change color of base texture
                        const Uint8 cellId = (this->board.at(i).at(j) - 1) / 16;
                        this->texture_piece_base.setColorMod(this->players.at(cellId).getColor());

                        // Determine the correct tile to use and then render it
                        const Uint8 cellMask = this->board.at(i).at(j) - (cellId * 16) - 1;
                        this->texture_piece_base.setFrame({cellMask % 4 * 64, cellMask / 4 * 64, 64, 64});
                        this->texture_piece_edge.setFrame({cellMask % 4 * 64, cellMask / 4 * 64, 64, 64});
                        this->window.renderModdedTexture(this->texture_piece_base, {j * 64, i * 64, 64, 64});
                        this->window.renderBasicTexture(this->texture_piece_edge, {j * 64, i * 64, 64, 64});
                    }
                }
                this->window.present();
                this->texture_grid.setTexture(this->window.copyDummy());
                this->window.targetWindow();
                this->window.clear();
            }
            void render() override {
                this->window.renderBasicTexture(this->texture_background, {0, 0, this->window.getWidth(), this->window.getHeight()});
                this->window.renderBasicTexture(this->texture_boardframe, {848, 8, 1064, 1064});
                this->window.renderBasicTexture(this->texture_grid, {867, 27, 1026, 1026});
                
                // for (Uint8 i = 0; i < this->players.size(); i++) {
                //     this->renderPlayer(i);
                // }
                this->renderPiecePreview(turn);
            }

        public:
            game(const Uint8 &boardSize = 20, const Uint8 &playerCount = 4, const Uint8 &baseSets = 1, const Uint8 &hexSets = 0, const Uint8 &heptSets = 0, const Uint8 &octSets = 0) : bengine::loop("Blokus", 1920, 1080, SDL_WINDOW_SHOWN | SDL_WINDOW_MOUSE_GRABBED | SDL_WINDOW_FULLSCREEN) {               
                // Basic random seed generation
                std::srand(std::time(NULL));
                
                // 30 fps; Blokus doesn't need to run fast at all
                this->deltaTime = 0.03333333;

                // bengine::loop has a default window size of 1280x1080, this can cause scaling issues if not dealt with
                this->window.setBaseWidth(this->window.getWidth());
                this->window.setBaseHeight(this->window.getHeight());

                // Amount of players for the game; processed to be valid
                const Uint8 players = playerCount < 2 ? 2 : (playerCount > 4 ? 4 : playerCount);
                // Size of the board for the game; processed to be valid
                const Uint8 size = boardSize < 20 ? 20 : (boardSize > 100 ? 100 : boardSize);

                this->pieceSets[blokus::POLYTYPE_BASE] = blokus::processPolyominoSet(blokus::POLYTYPE_BASE, baseSets);
                this->pieceSets[blokus::POLYTYPE_HEX] = blokus::processPolyominoSet(blokus::POLYTYPE_HEX, hexSets);
                this->pieceSets[blokus::POLYTYPE_HEPT] = blokus::processPolyominoSet(blokus::POLYTYPE_HEPT, heptSets);
                this->pieceSets[blokus::POLYTYPE_OCT] = blokus::processPolyominoSet(blokus::POLYTYPE_OCT, octSets);

                // Player setup, stuff after the for loop shall be removed later
                for (Uint8 i = 0; i < players; i++) {
                    this->players.emplace_back(blokus::player(this->pieceSets[blokus::POLYTYPE_BASE], this->pieceSets[blokus::POLYTYPE_HEX], this->pieceSets[blokus::POLYTYPE_HEPT], this->pieceSets[blokus::POLYTYPE_OCT]));
                }
                this->players[0].setColor({255, 0, 0, 255});
                this->players[0].setName(u"Bearslay");
                this->players[1].setColor({0, 255, 0, 255});
                this->players[1].setName(u"Barfunkel");
                if (this->players.size() > 2) {
                    this->players[2].setColor({0, 0, 255, 255});
                    this->players[2].setName(u"Belay");
                }
                if (this->players.size() > 3) {
                    this->players[3].setColor({255, 255, 0, 255});
                    this->players[3].setName(u"Charlie");
                }

                this->texture_grid.setFrame({0, 0, size * 64, size * 64});

                // Grid setup; includes initializing the board itself as well as the grid's initial texture of empty cells
                this->window.targetDummy();
                this->window.initDummy(size * 64, size * 64);

                this->window.clear();
                this->window.renderBasicTexture(this->texture_background, {0, 0, size * 64, size * 64});
                for (Uint8 i = 0; i < size; i++) {
                    this->board.emplace_back();
                    for (Uint8 j = 0; j < size; j++) {
                        this->board[i].emplace_back(0);
                        this->window.renderBasicTexture(this->texture_emptyCell, {j * 64, i * 64, 64, 64});
                    }
                }
                this->window.present();
                this->texture_grid.setTexture(this->window.copyDummy());
                this->window.targetWindow();
                this->window.clear();

                this->gridClickArea = bengine::clickMatrix(867, 27, 867 + 1026, 27 + 1026, this->board.size(), this->board.at(0).size());
                this->piecesPreviewGrid.setCellSquareness(true);
            }
            ~game() {
                TTF_CloseFont(this->font_general);
                TTF_CloseFont(this->font_pageInfo);
                this->font_general = nullptr;
                this->font_pageInfo = nullptr;
            }
    };
}

#endif // BLOKUS_GAME_hpp
