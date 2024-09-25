#ifndef BTILS_MATRIX_hpp
#define BTILS_MATRIX_hpp

#include <vector>

namespace btils {
    /** Checks whether a 2D std::vector is rectangular or not
     * @tparam Type Any datatype/class (not relevant for this function)
     * @param input The 2D std::vector to check
     * @returns Whether the input is rectangular (true) or jagged (false)
     */
    template <typename Type> bool isRectangular(const std::vector<std::vector<Type>> &input) {
        for (std::size_t i = 1; i < input.size(); i++) {
            if (input.at(0).size() != input.at(i).size()) {
                return false;
            }
        }
        return true;
    }

    /** Rotate any 2D std::vector (matrix) 90 degrees
     * @tparam Type Any datatype/class
     * @param matrix The inputted 2D std::vector to be rotated
     * @param ccw Whether to rotate counter-clockwise or not
     * @param checkInput Whether to verify that the input is rectangular (not jagged) or not; helps prevent crashes at a small-ish performance cost
     * @returns The inputted matrix, but rotated 90 degrees
     */
    template <typename Type> std::vector<std::vector<Type>> rotateMatrix(const std::vector<std::vector<Type>> &matrix, const bool &ccw = true, const bool &checkInput = false) {
        if (checkInput && btils::isRectangular(matrix)) {return matrix;}
        std::vector<std::vector<Type>> output;

        if (ccw) {
            for (std::size_t col = matrix.at(0).size(); col--;) {
                output.emplace_back();
                for (std::size_t row = 0; row < matrix.size(); row++) {
                    output[matrix.at(0).size() - 1 - col].emplace_back(matrix.at(row).at(col));
                }
            }
            return output;
        }

        for (std::size_t col = 0; col < matrix.at(0).size(); col++) {
            output.emplace_back();
            for (std::size_t row = matrix.size(); row--;) {
                output[col].emplace_back(matrix.at(row).at(col));
            }
        }
        return output;
    }

    /** Flip any 2D std::vector (matrix)
     * @tparam Type Any datatype/class
     * @param matrix The inputted 2D std::vector to be flipped
     * @param ccw Whether to flip vertically (true) or horizontally (false)
     * @param checkInput Whether to verify that the input is rectangular (not jagged) or not; helps prevent crashes at a small-ish performance cost
     * @returns The inputted matrix, but flipped
     */
    template <typename Type> std::vector<std::vector<Type>> flipMatrix(const std::vector<std::vector<Type>> &matrix, const bool &vertical = true, const bool &checkInput = false) {
        if (checkInput && btils::isRectangular(matrix)) {return matrix;}
        std::vector<std::vector<Type>> output;

        if (vertical) {
            for (std::size_t row = matrix.size(); row--;) {
                output.emplace_back();
                for (std::size_t col = 0; col < matrix.at(0).size(); col++) {
                    output[matrix.size() - 1 - row].emplace_back(matrix.at(row).at(col));
                }
            }
            return output;
        }

        for (std::size_t row = 0; row < matrix.size(); row++) {
            output.emplace_back();
            for (std::size_t col = matrix.at(0).size(); col--;) {
                output[row].emplace_back(matrix.at(row).at(col));
            }
        }
        return output;
    }
}

#endif // BTILS_MATRIX_hpp
