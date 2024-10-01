#ifndef BENGINE_TEXTURE_hpp
#define BENGINE_TEXTURE_hpp

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace bengine {
    /// @brief A wrapper class for the SDL_Texture that pretty much just contains the source texture and frame
    class basicTexture {
        protected:
            /// @brief The source texture to use
            SDL_Texture *source = nullptr;
            /// @brief The portion of the source texture to actually display
            SDL_Rect frame = {};

        public:
            /** bengine::basicTexture constructor
             * @param texture The SDL_Texture to use as a source
             * @param frame The portion of the source texture to actually display
             */
            basicTexture(SDL_Texture *texture = NULL, const SDL_Rect &frame = {}) {
                bengine::basicTexture::setTexture(texture);
                bengine::basicTexture::setFrame(frame);
            }
            /// @brief bengine::basicTexture deconstructor; pretty much just handles some SDL cleanup
            ~basicTexture() {
                SDL_DestroyTexture(this->source);
                this->source = nullptr;
            }

            /** Assignment operator overload
             * @param rhs The bengine::basicTexture to inherit from
             */
            void operator=(const bengine::basicTexture &rhs) {
                bengine::basicTexture::setTexture(rhs.getTexture());
                bengine::basicTexture::setFrame(rhs.getFrame());
            }

            /** Get the source texture
             * @returns The source SDL_Texture
             */
            SDL_Texture *getTexture() const {
                return this->source;
            }
            /** Set the source texture to a new one
             * @param texture The new texture
             * @returns The old texture
             */
            SDL_Texture *setTexture(SDL_Texture *texture) {
                SDL_Texture* output = this->source;
                this->source = texture;
                return output;
            }
            /** Get the frame of the texture
             * @returns An SDL_Rect with the frame (NULL = entire texture)
             */
            SDL_Rect getFrame() const {
                return this->frame;
            }
            /** Set the frame of the texture
             * @param frame The new frame (NULL = entire texture)
             * @returns The old frame (NULL = entire texture)
             */
            SDL_Rect setFrame(const SDL_Rect &frame) {
                const SDL_Rect output = this->frame;
                this->frame.x = frame.x;
                this->frame.y = frame.y;
                this->frame.w = frame.w;
                this->frame.h = frame.h;
                return output;
            }
    };

    /// @brief A wrapper class for the SDL_Texture that contains the source texture, frame, and color modifications
    class moddedTexture : public basicTexture {
        protected:
            /// @brief The SDL_BlendMode used for the texture
            SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
            /// @brief The color modification used for the texture (includes opacity if certain blending modes are used)
            SDL_Color colorMod = {255, 255, 255, 255};

        public:
            /** bengine::moddedTexture constructor
             * @param texture The SDL_Texture to use as a source
             * @param frame The portion of the source texture to actually display
             * @param colorMod The color modification for the texture
             */
            moddedTexture(SDL_Texture *texture = nullptr, const SDL_Rect &frame = {}, const SDL_Color &colorMod = {255, 255, 255, 255}) {
                bengine::basicTexture::setTexture(texture);
                bengine::basicTexture::setFrame(frame);
                bengine::moddedTexture::setColorMod(colorMod);
            }
            /// @brief bengine::moddedTexture deconstructor
            ~moddedTexture() {
                bengine::basicTexture::~basicTexture();
            }

            /** Assignment operator overload
             * @param rhs The bengine::moddedTexture to inherit from
             */
            void operator=(const bengine::moddedTexture &rhs) {
                bengine::basicTexture::setTexture(rhs.getTexture());
                bengine::basicTexture::setFrame(rhs.getFrame());
                bengine::moddedTexture::setColorMod(rhs.getColorMod());
                bengine::moddedTexture::setBlendMode(rhs.getBlendMode());
            }

            /** Get the blending mode used on this texture
             * @returns The SDL_BlendMode used on this texture
             */ 
            SDL_BlendMode getBlendMode() const {
                return this->blendMode;
            }
            /** Set the blending mode that will be used on this texture
             * @param blendMode The new SDL_BlendMode that will be used on this texture
             * @returns The old SDL_BlendMode that was used on this texture
             */
            SDL_BlendMode setBlendMode(const SDL_BlendMode &blendMode) {
                SDL_SetTextureBlendMode(this->source, blendMode);

                const SDL_BlendMode output = this->blendMode;
                this->blendMode = blendMode;
                return output;
            }

            /** Get the color modification used on this texture
             * @returns The color modification used on this texture
             */
            SDL_Color getColorMod() const {
                return this->colorMod;
            }
            /** Set the color modification that will be used on this texture
             * @param colorMod The new color mod that will be used on this texture
             * @returns The old color that was used on this texture
             */
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

            /** Get the "amount of red" in the texture
             * @returns The "amount of red" in the texture
             */
            Uint8 getRedMod() const {
                return this->colorMod.r;
            }
            /** Set the "amount of red" in the texture
             * @param redMod The new "amount of red" that will be present in the texture
             * @returns The old "amount of red" that was present in the texture
             */
            Uint8 setRedMod(const Uint8 &redMod) {
                SDL_SetTextureColorMod(this->source, redMod, this->colorMod.g, this->colorMod.b);

                const Uint8 output = this->colorMod.r;
                this->colorMod.r = redMod;
                return output;
            }

            /** Get the "amount of green" in the texture
             * @returns The "amount of green" in the texture
             */
            Uint8 getGreenMod() const {
                return this->colorMod.g;
            }
            /** Set the "amount of green" in the texture
             * @param greenMod The new "amount of green" that will be present in the texture
             * @returns The old "amount of green" that was present in the texture
             */
            Uint8 setGreenMod(const Uint8 &greenMod) {
                SDL_SetTextureColorMod(this->source, this->colorMod.r, greenMod, this->colorMod.b);
                
                const Uint8 output = this->colorMod.g;
                this->colorMod.g = greenMod;
                return output;
            }

            /** Get the "amount of blue" in the texture
             * @returns The "amount of blue" in the texture
             */
            Uint8 getBlueMod() const {
                return this->colorMod.b;
            }
            /** Set the "amount of blue" in the texture
             * @param blueMod The new "amount of blue" that will be present in the texture
             * @returns The old "amount of blue" that was present in the texture
             */
            Uint8 setBlueMod(const Uint8 &blueMod) {
                SDL_SetTextureColorMod(this->source, this->colorMod.r, this->colorMod.g, blueMod);
                
                const Uint8 output = this->colorMod.b;
                this->colorMod.b = blueMod;
                return output;
            }

            /** Get the opacity of the texture (only visable with certain blendmodes)
             * @returns The opacity of the texture
             */
            Uint8 getAlphaMod() const {
                return this->colorMod.a;
            }
            /** Set the opacity of the texture (only visable with certain blendmodes)
             * @param alphaMod The new opacity of the texture
             * @returns The old opacity of the texture
             */
            Uint8 setAlphaMod(const Uint8 &alphaMod) {
                SDL_SetTextureAlphaMod(this->source, alphaMod);
                
                const Uint8 output = this->colorMod.a;
                this->colorMod.a = alphaMod;
                return output;
            }
    };

    /// @brief A wrapper class for the SDL_Texture that contains the source texture, frame, color modifications, and rotation/reflection data
    class shiftingTexture : public moddedTexture {
        protected:
            /// @brief The point that the texture rotates about relative to the source frame's top-left corner
            SDL_Point pivot = {};
            /// @brief The angle that the texture is rotated at (in degrees)
            double angle = 0;
            /// @brief How the texture is flipped (SDL_FLIP_NONE, SDL_FLIP_HORIZONTAL, and SDL_FLIP_VERTICAL OR'd together)
            SDL_RendererFlip flip = SDL_FLIP_NONE;

        public:
            /** bengine::shiftingTexture constructor
             * @param texture The SDL_Texture to use as a source
             * @param frame The portion of the source texture to actually display
             * @param pivot The point for the texture to be rotated about relative to the source frame's top-left corner
             * @param angle The angle for the texture to be rotated at
             * @param colorMod The color modification for the texture
             */
            shiftingTexture(SDL_Texture *texture = NULL, const SDL_Rect &frame = {}, const SDL_Point &pivot = {}, const double &angle = 0, const SDL_Color &colorMod = {255, 255, 255, 255}) {
                bengine::basicTexture::setTexture(texture);
                bengine::basicTexture::setFrame(frame);
                bengine::moddedTexture::setColorMod(colorMod);
                bengine::shiftingTexture::setPivot(pivot);
                bengine::shiftingTexture::setAngle(angle);
            }
            /// @brief bengine::shiftingTexture deconstructor
            ~shiftingTexture() {
                bengine::moddedTexture::~moddedTexture();
            }

            /** Assignment operator overload
             * @param rhs The bengine::shiftingTexture to inherit from
             */
            void operator=(const bengine::shiftingTexture &rhs) {
                bengine::basicTexture::setTexture(rhs.getTexture());
                bengine::basicTexture::setFrame(rhs.getFrame());
                bengine::moddedTexture::setColorMod(rhs.getColorMod());
                bengine::moddedTexture::setBlendMode(rhs.getBlendMode());
                bengine::shiftingTexture::setPivot(rhs.getPivot());
                bengine::shiftingTexture::setAngle(rhs.getAngle());
                bengine::shiftingTexture::setFlip(rhs.getFlip());
            }

            /** Get the point for the texture to rotate about relative to the source frame's top-left corner
             * @returns The point for the texture to rotate about relative to the source frame's top-left corner
             */
            SDL_Point getPivot() const {
                return this->pivot;
            }
            /** Set the point for the texture to rotate about relative to the source frame's top-left corner
             * @param pivot The new point for the texture to rotate about relative to the source frame's top-left corner
             * @returns The old point for the texture to rotate about relative to the source frame's top-left corner
             */
            SDL_Point setPivot(const SDL_Point &pivot) {
                const SDL_Point output = this->pivot;
                this->pivot.x = pivot.x;
                this->pivot.y = pivot.y;
                return output;
            }

            /** Get the angle that the texture is rotated at (in degrees)
             * @returns The angle that the texture is rotated at (in degrees)
             */
            double getAngle() const {
                return this->angle;
            }
            /** Get the angle that the texture is rotated at (in degrees)
             * @param angle The new angle that the texture will be rotated at (in degrees)
             * @returns The old angle that the texture was rotated at (in degrees)
             */
            double setAngle(const double &angle) {
                const double output = this->angle;
                this->angle = angle;
                return output;
            }

            /** Get the state of how the texture is flipped
             * @returns An SDL_RendererFlip mask representing how the texture is flipped
             */
            SDL_RendererFlip getFlip() const {
                return this->flip;
            }
            /** Set how the texture should be flipped
             * @param flip An SDL_RendererFlip mask representing how the texture should be flipped
             * @returns An SDL_RendererFlip mask representing how the texture is flipped
             */
            SDL_RendererFlip setFlip(const SDL_RendererFlip &flip) {
                const SDL_RendererFlip output = this->flip;
                this->flip = flip;
                return output;
            }
    };
}

#endif // BENGINE_TEXTURE_hpp
