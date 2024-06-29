#ifndef BENGINE_TEXTURE_hpp
#define BENGINE_TEXTURE_hpp

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "bengine_window.hpp"

namespace bengine {
    class basicTexture {
        protected:
            SDL_Texture *source = nullptr;
            SDL_Rect frame = {};

        public:
            basicTexture(SDL_Texture *texture = NULL, const SDL_Rect &frame = {}) {
                bengine::basicTexture::setTexture(texture);
                bengine::basicTexture::setFrame(frame);
            }
            ~basicTexture() {
                SDL_DestroyTexture(this->source);
                this->source = nullptr;
            }

            void operator=(const bengine::basicTexture &rhs) {
                bengine::basicTexture::setTexture(rhs.getTexture());
                bengine::basicTexture::setFrame(rhs.getFrame());
            }

            SDL_Texture *getTexture() const {
                return this->source;
            }
            SDL_Texture *setTexture(SDL_Texture *texture) {
                SDL_Texture* output = this->source;
                this->source = texture;
                return output;
            }
            SDL_Rect getFrame() const {
                return this->frame;
            }
            SDL_Rect setFrame(const SDL_Rect &frame) {
                const SDL_Rect output = this->frame;
                this->frame.x = frame.x;
                this->frame.y = frame.y;
                this->frame.w = frame.w;
                this->frame.h = frame.h;
                return output;
            }
    };

    class moddedTexture : public basicTexture {
        protected:
            SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
            SDL_Color colorMod = {255, 255, 255, 255};

        public:
            moddedTexture(SDL_Texture *texture = nullptr, const SDL_Rect &frame = {}, const SDL_Color &colorMod = {255, 255, 255, 255}) {
                bengine::basicTexture::setTexture(texture);
                bengine::basicTexture::setFrame(frame);
                bengine::moddedTexture::setColorMod(colorMod);
            }
            ~moddedTexture() {
                bengine::basicTexture::~basicTexture();
            }

            void operator=(const bengine::moddedTexture &rhs) {
                bengine::basicTexture::setTexture(rhs.getTexture());
                bengine::basicTexture::setFrame(rhs.getFrame());
                bengine::moddedTexture::setColorMod(rhs.getColorMod());
                bengine::moddedTexture::setBlendMode(rhs.getBlendMode());
            }

            SDL_BlendMode getBlendMode() const {
                return this->blendMode;
            }
            SDL_BlendMode setBlendMode(const SDL_BlendMode &blendMode) {
                SDL_SetTextureBlendMode(this->source, blendMode);

                const SDL_BlendMode output = this->blendMode;
                this->blendMode = blendMode;
                return output;
            }

            SDL_Color getColorMod() const {
                return this->colorMod;
            }
            SDL_Color setColorMod(const SDL_Color &colorMod) {
                SDL_SetTextureColorMod(this->source, colorMod.r, colorMod.g, colorMod.b);
                SDL_SetTextureAlphaMod(this->source, colorMod.a);

                const SDL_Color output = this->colorMod;
                this->colorMod.r = colorMod.r;
                this->colorMod.g = colorMod.g;
                this->colorMod.b = colorMod.b;
                this->colorMod.a = colorMod.a;
                return output;
            }

            Uint8 getRedMod() const {
                return this->colorMod.r;
            }
            Uint8 setRedMod(const Uint8 &redMod) {
                SDL_SetTextureColorMod(this->source, redMod, this->colorMod.g, this->colorMod.b);

                const Uint8 output = this->colorMod.r;
                this->colorMod.r = redMod;
                return output;
            }

            Uint8 getGreenMod() const {
                return this->colorMod.g;
            }
            Uint8 setGreenMod(const Uint8 &greenMod) {
                SDL_SetTextureColorMod(this->source, this->colorMod.r, greenMod, this->colorMod.b);
                
                const Uint8 output = this->colorMod.g;
                this->colorMod.g = greenMod;
                return output;
            }

            Uint8 getBlueMod() const {
                return this->colorMod.b;
            }
            Uint8 setBlueMod(const Uint8 &blueMod) {
                SDL_SetTextureColorMod(this->source, this->colorMod.r, this->colorMod.g, blueMod);
                
                const Uint8 output = this->colorMod.b;
                this->colorMod.b = blueMod;
                return output;
            }

            Uint8 getAlphaMod() const {
                return this->colorMod.a;
            }
            Uint8 setAlphaMod(const Uint8 &alphaMod) {
                SDL_SetTextureAlphaMod(this->source, alphaMod);
                
                const Uint8 output = this->colorMod.a;
                this->colorMod.a = alphaMod;
                return output;
            }
    };

    class shiftingTexture : public moddedTexture {
        protected:
            SDL_Point pivot = {};
            double angle = 0;
            SDL_RendererFlip flip = SDL_FLIP_NONE;

        public:
            shiftingTexture(SDL_Texture *texture = NULL, const SDL_Rect &frame = {}, const SDL_Point &pivot = {}, const double &angle = 0, const SDL_Color &colorMod = {255, 255, 255, 255}) {
                bengine::basicTexture::setTexture(texture);
                bengine::basicTexture::setFrame(frame);
                bengine::moddedTexture::setColorMod(colorMod);
                bengine::shiftingTexture::setPivot(pivot);
                bengine::shiftingTexture::setAngle(angle);
            }
            ~shiftingTexture() {
                bengine::moddedTexture::~moddedTexture();
            }

            void operator=(const bengine::shiftingTexture &rhs) {
                bengine::basicTexture::setTexture(rhs.getTexture());
                bengine::basicTexture::setFrame(rhs.getFrame());
                bengine::moddedTexture::setColorMod(rhs.getColorMod());
                bengine::moddedTexture::setBlendMode(rhs.getBlendMode());
                bengine::shiftingTexture::setPivot(rhs.getPivot());
                bengine::shiftingTexture::setAngle(rhs.getAngle());
                bengine::shiftingTexture::setFlip(rhs.getFlip());
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
                const double output = this->angle;
                this->angle = angle;
                return output;
            }
            double adjustAngle(const double &amount) {
                const double output = this->angle;
                this->angle += angle;
                return output;
            }

            SDL_RendererFlip getFlip() const {
                return this->flip;
            }
            SDL_RendererFlip setFlip(const SDL_RendererFlip &flip) {
                const SDL_RendererFlip output = this->flip;
                this->flip = flip;
                return output;
            }
    };
}

#endif /* BENGINE_TEXTURE_hpp */
