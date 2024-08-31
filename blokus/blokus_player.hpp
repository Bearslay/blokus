#ifndef BLOKUS_PLAYER_hpp
#define BLOKUS_PLAYER_hpp

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>

#include "btils.hpp"
#include "bengine.hpp"

#include "blokus_polyominoes.hpp"
#include "blokus_piece.hpp"

namespace blokus {
    /// @brief The minimum amount of sets of a given polyomino type allowed in a game of Blokus
    const Uint8 polySetMins[6] = {1, 0, 0, 0, 0, 0};
    /// @brief The maximum amount of sets of a given polyomino type allowed in a game of Blokus
    const Uint8 polySetMaxes[6] = {12, 4, 2, 1, 0, 0};

    /** Take a desired amount of polyomino sets to use and confine that value to the mins/maxes allowed for that type
     * @param type The type of polyomino being analyzed
     * @param value The input for the amount of the given polyomino sets to uses
     * @returns The amount of polyomino sets for the given polyType as bounded
     */
    Uint8 processPolyominoSet(const blokus::polyType &type, const Uint8 &value) {
        if (type > blokus::POLYTYPE_DEC) {
            return 0;
        }
        return value > blokus::polySetMaxes[type] ? blokus::polySetMaxes[type] : (value < blokus::polySetMins[type] ? blokus::polySetMins[type] : value);
    }

    class player {
        private:
            std::u16string name = u"Red";
            SDL_Color color = {255, 0, 0, 255};

            std::vector<std::vector<blokus::piece>> pieces;

        public:
            player(const Uint8 &baseSets = polySetMins[blokus::POLYTYPE_BASE], const Uint8 &hexSets = polySetMins[blokus::POLYTYPE_HEX], const Uint8 &heptSets = polySetMins[blokus::POLYTYPE_HEPT], const Uint8 &octSets = polySetMins[blokus::POLYTYPE_OCT]) {
                const Uint8 setValues[4] = {baseSets, hexSets, heptSets, octSets};

                Uint16 idStart = 0;
                for (blokus::polyType i = blokus::POLYTYPE_BASE; i <= blokus::POLYTYPE_OCT; i++) {
                    this->pieces.emplace_back();
                    for (Uint8 j = 0; j < setValues[i]; j++) {
                        for (Uint8 k = 0; k < blokus::polyominoAmounts[i]; k++) {
                            this->pieces[i].emplace_back(blokus::piece(idStart + k));
                        }
                    }
                    idStart += blokus::polyominoAmounts[i];
                }
            }

            void printPieces(const blokus::polyominoType &type) const {
                if (type < blokus::POLYTYPE_BASE || type > blokus::POLYTYPE_OCT) {
                    return;
                }
                for (Uint16 i = 0; i < this->pieces.at(type).size(); i++) {
                    this->pieces.at(type).at(i).print();
                }
            }

            Uint16 getRemainingPieces(const blokus::polyominoType &type) const {
                if (type >= blokus::POLYTYPE_BASE && type <= blokus::POLYTYPE_OCT) {
                    return (Uint16)this->pieces.at(type).size();
                }
                return this->getRemainingPieces(blokus::POLYTYPE_BASE) + this->getRemainingPieces(blokus::POLYTYPE_HEX) + this->getRemainingPieces(blokus::POLYTYPE_HEPT) + this->getRemainingPieces(blokus::POLYTYPE_OCT);
            }

            Uint16 getRemainingTiles(const blokus::polyominoType &type) const {
                if (type == blokus::POLYTYPE_BASE) {
                    Uint16 output = 0;
                    for (Uint8 i = 0; i < this->pieces.at(blokus::POLYTYPE_BASE).size(); i++) {
                        output += this->pieces.at(blokus::POLYTYPE_BASE).at(i).getTiles();
                    }
                    return output;
                } else if (type >= blokus::POLYTYPE_HEX && type <= blokus::POLYTYPE_OCT) {
                    return (Uint16)this->pieces.at(type).size() * blokus::polyominoTiles[type];
                }
                return this->getRemainingTiles(blokus::POLYTYPE_BASE) + this->getRemainingTiles(blokus::POLYTYPE_HEX) + this->getRemainingTiles(blokus::POLYTYPE_HEPT) + this->getRemainingTiles(blokus::POLYTYPE_OCT);
            }

            std::u16string getName() const {
                return this->name;
            }
            std::u16string setName(const std::u16string &name) {
                return btils::set<std::u16string>(this->name, name);
            }
            SDL_Color getColor() const {
                return this->color;
            }
            SDL_Color setColor(const SDL_Color &color) {
                const SDL_Color output = this->color;
                this->color.r = color.r;
                this->color.g = color.g;
                this->color.b = color.b;
                this->color.a = color.a;
                return output;
            }
    };
}

#endif /* BLOKUS_PLAYER_hpp */
