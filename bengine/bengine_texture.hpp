#ifndef BENGINE_TEXTURE_hpp
#define BENGINE_TEXTURE_hpp

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "btils_main.hpp"

namespace bengine {
    class basicTexture {
        protected:
            SDL_Texture *source = NULL;
            SDL_Rect frame = {};

        public:
            basicTexture(SDL_Texture *texture = NULL, const SDL_Rect &frame = {}) : source(texture), frame(frame) {

            }
            ~basicTexture() {
                SDL_DestroyTexture(this->source);
            }

            SDL_Texture *getTexture() const {
                return this->source;
            }
            SDL_Texture *setTexture(SDL_Texture *texture) {
                return btils::set<SDL_Texture*>(this->source, texture);
            }
            SDL_Rect getFrame() const {
                return this->frame;
            }
            SDL_Rect setFrame(const SDL_Rect &frame) {
                return btils::set<SDL_Rect>(this->frame, frame);
            }
    };

    class moddedTexture : public basicTexture {
        protected:
            SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
            SDL_Color colorMod = {255, 255, 255, 255};

        public:
            moddedTexture(SDL_Texture *texture = NULL, const SDL_Rect &frame = {}, const SDL_Color &colorMod = {255, 255, 255, 255}) : colorMod(colorMod) {
                setTexture(texture);
                setFrame(frame);
            }
            ~moddedTexture() {
                SDL_DestroyTexture(this->source);
            }

            SDL_BlendMode getBlendMode() const {
                return this->blendMode;
            }
            SDL_BlendMode setBlendMode(const SDL_BlendMode &blendMode) {
                return btils::set<SDL_BlendMode>(this->blendMode, blendMode);
            }

            SDL_Color getColorMod() const {
                return this->colorMod;
            }
            SDL_Color setColorMod(const SDL_Color &colorMod) {
                return btils::set<SDL_Color>(this->colorMod, colorMod);
            }

            Uint8 getRedMod() const {
                return this->colorMod.r;
            }
            Uint8 setRedMod(const Uint8 &redMod) {
                return btils::set<Uint8>(this->colorMod.r, redMod);
            }

            Uint8 getGreenMod() const {
                return this->colorMod.g;
            }
            Uint8 setGreenMod(const Uint8 &greenMod) {
                return btils::set<Uint8>(this->colorMod.g, greenMod);
            }

            Uint8 getBlueMod() const {
                return this->colorMod.b;
            }
            Uint8 setBlueMod(const Uint8 &blueMod) {
                return btils::set<Uint8>(this->colorMod.b, blueMod);
            }

            Uint8 getAlphaMod() const {
                return this->colorMod.a;
            }
            Uint8 setAlphaMod(const Uint8 &alphaMod) {
                return btils::set<Uint8>(this->colorMod.a, alphaMod);
            }
    };

    class shiftingTexture : public moddedTexture {
        protected:
            SDL_Point pivot = {};
            double angle = 0;
            SDL_RendererFlip flip = SDL_FLIP_NONE;

        public:
            shiftingTexture(SDL_Texture *texture = NULL, const SDL_Rect &frame = {}, const SDL_Point &pivot = {}, const double &angle = 0, const SDL_Color &colorMod = {255, 255, 255, 255}) : pivot(pivot), angle(angle) {
                setTexture(texture);
                setFrame(frame);
                setColorMod(colorMod);
            }
            ~shiftingTexture() {
                SDL_DestroyTexture(this->source);
            }

            SDL_Point getPivot() const {
                return this->pivot;
            }
            SDL_Point setPivot(const SDL_Point &pivot) {
                const SDL_Point output = this->pivot;
                this->pivot.x = pivot.x;
                this->pivot.y = pivot.y;
                return output;
            }
            SDL_Point adjustPivot(const SDL_Point &amount) {
                const SDL_Point output = this->pivot;
                this->pivot.x += pivot.x;
                this->pivot.y += pivot.y;
                return output;
            }

            double getAngle() const {
                return this->angle;
            }
            double setAngle(const double &angle) {
                return btils::set<double>(this->angle, angle);
            }
            double adjustAngle(const double &amount) {
                return btils::adj<double>(this->angle, amount);
            }

            SDL_RendererFlip getFlip() const {
                return this->flip;
            }
            SDL_RendererFlip setFlip(const SDL_RendererFlip &flip) {
                return btils::set<SDL_RendererFlip>(this->flip, flip);
            }
    };
}

#endif /* BENGINE_TEXTURE_hpp */
