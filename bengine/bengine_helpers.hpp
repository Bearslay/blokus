#ifndef BENGINE_HELPERS_hpp
#define BENGINE_HELPERS_hpp

#include <vector>
#include <cmath>

namespace bengine {
    typedef enum {
        ALIGN_TOP_LFT = 0,    // Top-Left
        ALIGN_TOP_CTR = 1,    // Top-Center
        ALIGN_TOP_RHT = 2,    // Top-Right
        ALIGN_CTR_LFT = 3,    // Center-Left
        ALIGN_CTR_CTR = 4,    // Center-Center
        ALIGN_CTR_RHT = 5,    // Center-Right
        ALIGN_BTM_LFT = 6,    // Bottom-Left
        ALIGN_BTM_CTR = 7,    // Bottom-Center
        ALIGN_BTM_RHT = 8     // Bottom-Right
    } paddedGridAlignments;

    /// @brief A class to contain dimensional data relating to a grid of cells within a set zone; primarily used to calculate and store the size of said cells
    class paddedGrid {
        private:
            /// @brief The left-right and top-bottom alignment of the grid within the given zone; relates the top-left corner of the zone to the top-left corner of the top-left cell
            unsigned char alignment = ALIGN_CTR_CTR;
            /// @brief Whether the cells are square or not
            bool squareCells = false;

            /// @brief The width of the zone that the grid inhabits
            unsigned int width = 0;
            /// @brief The height of the zone that the grid inhabits
            unsigned int height = 0;
            /// @brief The amount of columns that the grid has
            unsigned short cols = 0;
            /// @brief The amount of rows that the grid has
            unsigned short rows = 0;
            /// @brief The horizontal distance between cells
            unsigned short gapWidth = 0;
            /// @brief The vertical distance between cells
            unsigned short gapHeight = 0;

            /// @brief The width of the area that the cells actually inhabit
            unsigned int usableWidth = 0;
            /// @brief The height of the area that the cells actually inhabit
            unsigned int usableHeight = 0;
            /// @brief The width of each cell
            unsigned int cellWidth = 0;
            /// @brief The height of each cell
            unsigned int cellHeight = 0;
            /// @brief The x-offset of the top-left cell's top-left corner from the zone's top-left corner
            unsigned short xOffset = 0;
            /// @brief The y-offset of the top-left cell's top-left corner from the zone's top-left corner
            unsigned short yOffset = 0;

            /// @brief Update all of the "output" values for the padded grid
            void update() {
                this->usableWidth = this->width - this->gapWidth * (this->cols - 1);
                this->usableHeight = this->height - this->gapHeight * (this->rows - 1);
                this->cellWidth = this->usableWidth / this->cols;
                this->cellHeight = this->usableHeight / this->rows;
                
                // When using square cells, both the cell's width and height are set to the smaller dimension so that the cells don't escape the zone
                if (this->squareCells) {
                    if (this->cellWidth < this->cellHeight) {
                        this->cellHeight = this->cellWidth;
                    } else {
                        this->cellWidth = this->cellHeight;
                    }
                }

                if (this->alignment % 3 == 0) {
                    this->xOffset = 0;
                } else if (this->alignment % 3 == 1) {
                    this->xOffset = (this->usableWidth - this->cellWidth * this->cols) / 2;
                } else {
                    this->xOffset = this->usableWidth - this->cellWidth * this->cols;
                }
                if (this->alignment <= 2) {
                    this->yOffset = 0;
                } else if (this->alignment <= 5) {
                    this->yOffset = (this->usableHeight - this->cellHeight * this->rows) / 2;
                } else {
                    this->yOffset = this->usableWidth - this->cellWidth * this->cols;
                }
            }

        public:
            /** bengine::paddedGrid constructor
             * @param width The width of the zone that the grid will inhabit
             * @param height The height of the zone that the grid will inhabit
             * @param cols The amount of columns that the grid will have
             * @param rows The amount of rows that the grid will have
             * @param gapWidth The amount of horiztonal space between cells
             * @param gapHeight The amount of vertical space between cells
             * @param alignment The left-right and top-bottom alignment of the grid within the given zone; relates the top-left corner of the zone to the top-left corner of the top-left cell
             */
            paddedGrid(const unsigned int &width, const unsigned int &height, const unsigned short &cols, const unsigned short &rows, const unsigned short &gapWidth, const unsigned short &gapHeight, const unsigned char &alignment = ALIGN_CTR_CTR) : width(width), height(height), cols(cols), rows(rows), gapWidth(gapWidth), gapHeight(gapHeight) {
                this->alignment = alignment > ALIGN_BTM_RHT ? ALIGN_CTR_CTR : alignment;
                this->update();
            }
            /// @brief bengine::paddedGrid deconstructor
            ~paddedGrid() {}

            /** Get the alignment of the padded grid
             * @returns The alignment of the grid
             */
            unsigned char getAlignment() const {
                return this->alignment;
            }
            /** Set the alignmnet for the padded grid
             * @param alignment The new alignment for the padded grid
             * @returns The old alignment for the padded grid
             */
            unsigned char setAlignment(const unsigned char &alignment) {
                const unsigned char output = this->alignment;
                this->alignment = alignment > ALIGN_BTM_RHT ? ALIGN_CTR_CTR : alignment;
                return output;
            }

            /** Check if the grid uses square cells or not
             * @returns Whether the grid uses square cells or not
             */
            bool hasSquareCells() const {
                return this->squareCells;
            }
            /** Toggle whether the grid uses square cells or not
             * @returns Whether the grid used to use square cells or not
             */
            bool toggleSquareCells() {
                const bool output = this->squareCells;
                this->squareCells = !this->squareCells;
                this->update();
                return output;
            }
            /** Set whether the grid should use square cells or not
             * @param state Whether the grid should use square cells or not
             * @returns Whether the grid used to use square cells or not
             */
            bool setCellSquareness(const bool &state = false) {
                const bool output = this->squareCells;
                this->squareCells = state;
                this->update();
                return output;
            }

            /** Get the width of the grid's zone
             * @returns The width of the grid's zone
             */
            unsigned int getWidth() const {
                return this->width;
            }
            /** Get the height of the grid's zone
             * @returns The height of the grid's zone
             */
            unsigned int getHeight() const {
                return this->height;
            }
            /** Get the amount of columns that the grid has
             * @returns The amount of columns that the grid has
             */
            unsigned short getCols() const {
                return this->cols;
            }
            /** Get the amount of rows that the grid has
             * @returns The amount of rows that the grid has
             */
            unsigned short getRows() const {
                return this->rows;
            }
            /** Get the horizontal distance between each cell
             * @returns The horizontal distance between each cell
             */
            unsigned short getGapWidth() const {
                return this->gapWidth;
            }
            /** Get the vertical distance between each cell
             * @returns The vertical distance between each cell
             */
            unsigned short getGapHeight() const {
                return this->gapHeight;
            }

            /** Set the width of the zone that the grid will inhabit
             * @param width The new width of the zone
             * @returns The old width of the zone
             */
            unsigned int setWidth(const unsigned int &width) {
                const int output = this->width;
                this->width = width;
                this->update();
                return output;
            }
            /** Set the height of the zone that the grid will inhabit
             * @param width The new height of the zone
             * @returns The old height of the zone
             */
            unsigned int setHeight(const unsigned int &height) {
                const int output = this->height;
                this->height = height;
                this->update();
                return output;
            }
            /** Set the amount of the columns that the grid will have
             * @param cols The new amount of columns
             * @returns The old amount of columns
             */
            unsigned short setCols(const unsigned short &cols) {
                const unsigned short output = this->cols;
                this->cols = cols;
                this->update();
                return output;
            }
            /** Set the amount of the rows that the grid will have
             * @param cols The new amount of rows
             * @returns The old amount of rows
             */
            unsigned short setRows(const unsigned short &rows) {
                const unsigned short output = this->rows;
                this->rows = rows;
                this->update();
                return output;
            }
            /** Set the horizontal distance between cells
             * @param gapWidth The new horizontal distance between cells
             * @returns The old horiztonal distance between cells
             */
            unsigned short setGapWidth(const unsigned short &gapWidth) {
                const unsigned short output = this->gapWidth;
                this->gapWidth = gapWidth;
                this->update();
                return output;
            }
            /** Set the vertical distance between cells
             * @param gapWidth The new vertical distance between cells
             * @returns The old vertical distance between cells
             */
            unsigned short setGapHeight(const unsigned short &gapHeight) {
                const unsigned short output = this->gapHeight;
                this->gapHeight = gapHeight;
                this->update();
                return output;
            }

            /** Get the width of the area that the cells inhabit
             * @returns The width of the area that the cells inhabit
             */
            unsigned int getUsableWidth() const {
                return this->usableWidth;
            }
            /** Get the height of the area that the cells inhabit
             * @returns The height of the area that the cells inhabit
             */
            unsigned int getUsableHeight() const {
                return this->usableHeight;
            }
            /** Get the width of each cell
             * @returns The width of each cell
             */
            unsigned int getCellWidth() const {
                return this->cellWidth;
            }
            /** Get the height of each cell
             * @returns The height of each cell
             */
            unsigned int getCellHeight() const {
                return this->cellHeight;
            }
            /** Get the horizontal distance between the top-left corner of the zone and the top-left corner of the top-left cell in the grid
             * @returns The horizontal distance between the top-left corner of the zone and the top-left corner of the top-left cell in the grid
             */
            unsigned short getXOffset() const {
                return this->xOffset;
            }
            /** Get the vertical distance between the top-left corner of the zone and the top-left corner of the top-left cell in the grid
             * @returns The vertical distance between the top-left corner of the zone and the top-left corner of the top-left cell in the grid
             */
            unsigned short getYOffset() const {
                return this->yOffset;
            }
    };

    /// @brief A class containing useful functions designed for 4/8-bit autotiling
    class autotiler {
        private:
            /// @brief Key containing the 47 bitmasks relevant to 8-bit autotiling
            const static unsigned char key[47];

            /// @brief List of unicode characters used in terminal-based 4-bit autotiling
            const static char* fourBitUnicode[32];
            /// @brief List of unicode characters used in terminal-based 8-bit autotiling
            const static char* eightBitUnicode[94];

            /** Calculate the 4-bit mask value for a given tile within a grid
             * 
             * Any bounds-checking needs to happen outside of this function
             * 
             * @param grid The grid containing 4-bit mask values
             * @param x The x-position (col) of the tile to update within the grid
             * @param y The y-position (row) of the tile to update within the grid
             * @param solidBoundaries Whether to consider the edges of the grid as full or empty tiles
             * @returns The updated value of the indicated tile or -1 if the tile is is already -1
             */
            static char calc4BitMaskValue(const std::vector<std::vector<char>> &grid, const unsigned long int &x, const unsigned long int &y, const bool &solidBoundaries = false) {
                // Check to see if the current tile would even display anything
                if (grid.at(y).at(x) < 0) {
                    return -1;
                }
                return (y > 0 ? (grid.at(y - 1).at(x) >= 0) : solidBoundaries) + (x > 0 ? (grid.at(y).at(x - 1) >= 0) : solidBoundaries) * 2 + (x < grid.at(0).size() - 1 ? (grid.at(y).at(x + 1) >= 0) : solidBoundaries) * 4 + (y < grid.size() - 1 ? (grid.at(y + 1).at(x) >= 0) : solidBoundaries) * 8;
            }

            /** Calculate the 8-bit mask value for a given tile within a grid
             * 
             * Any bounds-checking needs to happen outside of this function
             * 
             * @param grid The grid containing 8-bit mask values
             * @param x The x-position (col) of the tile to update within the grid
             * @param y The y-position (row) of the tile to update within the grid
             * @param solidBoundaries Whether to consider the edges of the grid as full or empty tiles
             * @returns The updated value of the indicated tile or -1 if the tile is is already -1
             */
            static char calc8BitMaskValue(const std::vector<std::vector<char>> &grid, const unsigned long int &x, const unsigned long int &y, const bool &solidBoundaries = false, const bool &doBoundsCheck = false) {
                // Check to see if the current tile would even display anything
                if (grid.at(y).at(x) < 0) {
                    return -1;
                }

                const bool tl = y > 0 && x > 0 ? (grid.at(y - 1).at(x - 1) >= 0) : solidBoundaries;
                const bool t = y > 0 ? (grid.at(y - 1).at(x) >= 0) : solidBoundaries;
                const bool tr = y > 0 && x < grid.at(0).size() - 1 ? (grid.at(y - 1).at(x + 1) >= 0) : solidBoundaries;
                const bool l = x > 0 ? (grid.at(y).at(x - 1) >= 0) : solidBoundaries;
                const bool r = x < grid.at(0).size() - 1 ? (grid.at(y).at(x + 1) >= 0) : solidBoundaries;
                const bool bl = y < grid.size() - 1 && x > 0 ? (grid.at(y + 1).at(x - 1) >= 0) : solidBoundaries;
                const bool b = y < grid.size() - 1 ? (grid.at(y + 1).at(x) >= 0) : solidBoundaries;
                const bool br = y < grid.size() - 1 && x < grid.at(0).size() - 1 ? (grid.at(y + 1).at(x + 1) >= 0) : solidBoundaries;

                const unsigned char maskVal = (tl && t && l) + t * 2 + (tr && t && r) * 4 + l * 8 + r * 16 + (bl && b && l) * 32 + b * 64 + (br && b && r) * 128;
                for (unsigned char i = 0; i < 47; i++) {
                    if (maskVal == bengine::autotiler::key[i]) {
                        return i;
                    }
                }
                return -1;
            }

        public:
            /// @brief bengine::autoTiler constructor
            autotiler() {}
            /// @brief bengine::autoTiler deconstructor
            ~autotiler() {}

            /** Change a tile and update surrounding ones in a 4-bit autotiling grid
             * @param grid Grid of indexing values that dictate the source frame for the texture sheet
             * @param x x-position of the changed tile in the grid
             * @param y y-position of the changed tile in the grid
             * @param addTile Whether to add or remove a tile in the indicated position
             * @param solidBoundaries Whether to consider the edges of the grid as full or empty tiles
             * @returns The value of the updated tile
             */
            static char fourBit(std::vector<std::vector<char>> &grid, const unsigned long int &x, const unsigned long int &y, const bool &addTile = true, const bool &solidBoundaries = false) {
                if (y >= grid.size() || x >= grid.at(0).size()) {
                    return -1;
                }
                grid[y][x] = addTile - 1;

                for (char i = -1; i <= 1; i++) {
                    for (char j = -1; j <= 1; j++) {
                        // A 4-bit autotiling mask only requires cardinal directions, so the corners are skipped
                        if (std::abs(i) == 1 && std::abs(j) == 1) {
                            continue;
                        }
                        // Check if the current tile is in-bounds
                        if (y + i < 0 || y + i >= grid.size() || x + j < 0 || x + j >= grid.at(0).size()) {
                            continue;
                        }
                        // Update the mask value for the current tile
                        grid[y + i][x + j] = bengine::autotiler::calc4BitMaskValue(grid, x + j, y + i, solidBoundaries);
                    }
                }
                return grid.at(y).at(x);
            }
            /** Change a tile and update surrounding ones in an 8-bit autotiling grid
             * @param grid Grid of indexing values that dictate the source frame for the texture sheet
             * @param x x-position of the changed tile in the grid
             * @param y y-position of the changed tile in the grid
             * @param addTile Whether to add or remove a tile in the indicated position
             * @param solidBoundaries Whether to consider the edges of the grid as full or empty tiles
             * @returns The value of the updated tile
             */
            static char eightBit(std::vector<std::vector<char>> &grid, const unsigned long int &x, const unsigned long int &y, const bool &addTile = true, const bool &solidBoundaries = false) {
                if (y >= grid.size() || x >= grid.at(0).size()) {
                    return -1;
                }
                grid[y][x] = addTile - 1;

                for (char i = -1; i <= 1; i++) {
                    for (char j = -1; j <= 1; j++) {
                        // Check if the current tile is in-bounds
                        if (y + i < 0 || y + i >= grid.size() || x + j < 0 || x + j >= grid.at(0).size()) {
                            continue;
                        }
                        // Update the mask value for the current tile
                        grid[y + i][x + j] = bengine::autotiler::calc8BitMaskValue(grid, x + j, y + i, solidBoundaries);
                    }
                }
                return grid.at(y).at(x);
            }

            /** Populate a grid of full/empty tiles with appropriate 4-bit mask values
             * @param grid The grid containing full (true) or empty (false) tiles to be populated
             * @param solidBoundaries Whether to consider the borders of the grid to have full or empty tiles
             * @returns A grid of the same dimensions as the input grid, but containing 4-bit mask values rather than boolean ones
             */
            static std::vector<std::vector<char>> populateGridFourBit(const std::vector<std::vector<bool>> &grid, const bool &solidBoundaries = false) {
                std::vector<std::vector<char>> output;
                for (std::size_t i = 0; i < grid.size(); i++) {
                    output.emplace_back();
                    for (std::size_t j = 0; j < grid.at(0).size(); j++) {
                        output[i].emplace_back(grid.at(i).at(j) ? 1 : -1);
                    }
                }
                for (std::size_t i = 0; i < grid.size(); i++) {
                    for (std::size_t j = 0; j < grid.at(i).size(); j++) {
                        output[i][j] = bengine::autotiler::calc4BitMaskValue(output, j, i, false);
                    }
                }
                return output;
            }

            /** Populate a grid of full/empty tiles with appropriate 8-bit mask values
             * @param grid The grid containing full (true) or empty (false) tiles to be populated
             * @param solidBoundaries Whether to consider the borders of the grid to have full or empty tiles
             * @returns A grid of the same dimensions as the input grid, but containing 8-bit mask values rather than boolean ones
             */
            static std::vector<std::vector<char>> populateGridEightBit(const std::vector<std::vector<bool>> &grid, const bool &solidBoundaries = false) {
                std::vector<std::vector<char>> output;
                for (std::size_t i = 0; i < grid.size(); i++) {
                    output.emplace_back();
                    for (std::size_t j = 0; j < grid.at(0).size(); j++) {
                        output[i].emplace_back(grid.at(i).at(j) ? 1 : -1);
                    }
                }
                for (std::size_t i = 0; i < grid.size(); i++) {
                    for (std::size_t j = 0; j < grid.at(i).size(); j++) {
                        output[i][j] = bengine::autotiler::calc8BitMaskValue(output, j, i, false);
                    }
                }
                return output;
            }

            /** Print a grid of 4-bit mask values to iostream using unicode block element characters
             * @param grid The grid of 4-bit mask values to print
             */
            static void printFourBitGrid(const std::vector<std::vector<char>> &grid) {
                for (std::size_t i = 0; i < grid.size(); i++) {
                    for (std::size_t j = 0; j < grid.at(i).size(); j++) {
                        if (grid.at(i).at(j) < 0) {
                            std::cout << "    ";
                            continue;
                        }
                        std::cout << bengine::autotiler::fourBitUnicode[(unsigned char)grid.at(i).at(j)];
                    }
                    std::cout << "\n";
                    for (std::size_t j = 0; j < grid.at(i).size(); j++) {
                        if (grid.at(i).at(j) < 0) {
                            std::cout << "    ";
                            continue;
                        }
                        std::cout << bengine::autotiler::fourBitUnicode[(unsigned char)grid.at(i).at(j) + 16];
                    }
                    std::cout << "\n";
                }
            }

            /** Print a grid of 8-bit mask values to iostream using unicode block element characters
             * @param grid The grid of 8-bit mask values to print
             */
            static void printEightBitGrid(const std::vector<std::vector<char>> &grid) {
                for (std::size_t i = 0; i < grid.size(); i++) {
                    for (std::size_t j = 0; j < grid.at(i).size(); j++) {
                        if (grid.at(i).at(j) < 0) {
                            std::cout << "    ";
                            continue;
                        }
                        std::cout << bengine::autotiler::eightBitUnicode[(unsigned char)grid.at(i).at(j)];
                    }
                    std::cout << "\n";
                    for (std::size_t j = 0; j < grid.at(i).size(); j++) {
                        if (grid.at(i).at(j) < 0) {
                            std::cout << "    ";
                            continue;
                        }
                        std::cout << bengine::autotiler::eightBitUnicode[(unsigned char)grid.at(i).at(j) + 47];
                    }
                    std::cout << "\n";
                }
            }
    };
    /// @brief Key containing the 47 bitmasks relevant to 8-bit autotiling
    const unsigned char bengine::autotiler::key[47] = {0, 2, 8, 10, 11, 16, 18, 22, 24, 26, 27, 30, 31, 64, 66, 72, 74, 75, 80, 82, 86, 88, 90, 91, 94, 95, 104, 106, 107, 120, 122, 123, 126, 127, 208, 210, 214, 216, 218, 219, 222, 223, 248, 250, 251, 254, 255};
    /// @brief List of unicode characters used in terminal-based 4-bit autotiling
    const char* bengine::autotiler::fourBitUnicode[32] = {" ▄▄ ", " ██ ", "▄▄▄ ", "▄██ ", " ▄▄▄", " ██▄", "▄▄▄▄", "▄██▄", " ▄▄ ", " ██ ", "▄▄▄ ", "▄██ ", " ▄▄▄", " ██▄", "▄▄▄▄", "▄██▄", " ▀▀ ", " ▀▀ ", "▀▀▀ ", "▀▀▀ ", " ▀▀▀", " ▀▀▀", "▀▀▀▀", "▀▀▀▀", " ██ ", " ██ ", "▀██ ", "▀██ ", " ██▀", " ██▀", "▀██▀", "▀██▀"};
    /// @brief List of unicode characters used in terminal-based 8-bit autotiling
    const char* bengine::autotiler::eightBitUnicode[94] = {" ▄▄ ", " ██ ", "▄▄▄ ", "▄██ ", "███ ", " ▄▄▄", " ██▄", " ███", "▄▄▄▄", "▄██▄", "███▄", "▄███", "████", " ▄▄ ", " ██ ", "▄▄▄ ", "▄██ ", "███ ", " ▄▄▄", " ██▄", " ███", "▄▄▄▄", "▄██▄", "███▄", "▄███", "████", "▄▄▄ ", "▄██ ", "███ ", "▄▄▄▄", "▄██▄", "███▄", "▄███", "████", " ▄▄▄", " ██▄", " ███", "▄▄▄▄", "▄██▄", "███▄", "▄███", "████", "▄▄▄▄", "▄██▄", "███▄", "▄███", "████", " ▀▀ ", " ▀▀ ", "▀▀▀ ", "▀▀▀ ", "▀▀▀ ", " ▀▀▀", " ▀▀▀", " ▀▀▀", "▀▀▀▀", "▀▀▀▀", "▀▀▀▀", "▀▀▀▀", "▀▀▀▀", " ██ ", " ██ ", "▀██ ", "▀██ ", "▀██ ", " ██▀", " ██▀", " ██▀", "▀██▀", "▀██▀", "▀██▀", "▀██▀", "▀██▀", "███ ", "███ ", "███ ", "███▀", "███▀", "███▀", "███▀", "███▀", " ███", " ███", " ███", "▀███", "▀███", "▀███", "▀███", "▀███", "████", "████", "████", "████", "████"};
}

#endif // BENGINE_HELPERS_hpp
