#ifndef BTILS_MAIN_hpp
#define BTILS_MAIN_hpp

#include <cmath>
#include <vector>

namespace btils {
    /** Acts as an assignment operator, but while returning the previous value of the thing being modified
     * @tparam Type A something that has a defined assignment operator (primitive type or otherwise)
     * @param lhs The left-hand side of an assignment operator (value will be altered)
     * @param rhs The right-hand side of an assignment operator (value will not be altered)
     * @returns The original value of lhs
     */
    template <typename Type> Type set(Type &lhs, const Type &rhs) {
        const Type output = lhs;
        lhs = rhs;
        return output;
    }
    /** Acts as an addition assignment operator, but while returning the previous value of the thing being modified
     * @tparam Type A something that has a defined addition assignment operator (primitive type or otherwise)
     * @param lhs The left-hand side of an addition assignment operator (value will be altered)
     * @param rhs The right-hand side of an addition assignment operator (value will not be altered)
     * @returns The original value of lhs
     */
    template <typename Type> Type adj(Type &lhs, const Type &rhs) {
        const Type output = lhs;
        lhs += rhs;
        return output;
    }

    /** Maps a value within a range onto a different range
     * @tparam InArithType An arithmetic data type for the input & starting range
     * @tparam OutArithType An arithmetic data type for the output & ending range
     * @param input Value to be mapped (of InArithType type)
     * @param inMin Minimum value for the starting range (of InArithType type)
     * @param inMax Maximum value for the starting range (of InArithType type)
     * @param outMin Minimum value for the ending range (of OutArithType type)
     * @param outMax Maximum value for the ending range (of OutArithType type)
     * @returns A value within the ending range matching the input value's position within the starting range (of OutArithType type)
     */
    template <typename InArithType, typename OutArithType> OutArithType map(const InArithType &input, const InArithType &inMin, const InArithType &inMax, const OutArithType &outMin, const OutArithType &outMax) {
        static_assert(std::is_arithmetic<InArithType>::value, "InArithType must be an arithmetic type");
        static_assert(std::is_arithmetic<OutArithType>::value, "OutArithType must be an arithmetic type");
        return outMin + (OutArithType)(input - inMin) * (outMax - outMin) / (OutArithType)(inMax - inMin);
    }
    /** Maps a value within a range onto a different range
     * @tparam ArithType An arithmetic data type for the input & starting range
     * @param input Value to be mapped
     * @param inMin Minimum value for the starting range
     * @param inMax Maximum value for the starting range
     * @param outMin Minimum value for the ending range
     * @param outMax Maximum value for the ending range
     * @returns A value within the ending range matching the input value's position within the starting range
     */
    template <typename ArithType> ArithType map(const ArithType &input, const ArithType &inMin, const ArithType &inMax, const ArithType &outMin, const ArithType &outMax) {return btils::map<ArithType, ArithType>(input, inMin, inMax, outMin, outMax);}

    #ifndef BTILS_ANGLE_hpp

    /** Normalize a value to a range (assuming the range is a loop)
     * @tparam ArithType An arithmetic data type for the input & range
     * @param input The value to normalize
     * @param rangeMin The smallest value making up the range (inclusive)
     * @param rangeMax The largest value making up the range (exclusive)
     * @returns A value normalized in the given range
     */
    template <typename ArithType> ArithType normalize(const ArithType &input, const ArithType &rangeMin, const ArithType &rangeMax) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        const ArithType width = rangeMax - rangeMin;
        const ArithType value = input - rangeMin;
        return (value - (std::floor(value / width) * width)) + rangeMin;
    }

    /** Normalize a value to a range with a minimum value of 0 (assuming the range is a loop)
     * @tparam ArithType An arithmetic data type for the input & range
     * @param input The value to normalize
     * @param max The largest value making up the range (exclusive)
     * @returns A value normalized in the given range
     */
    template <typename ArithType> ArithType normalize(const ArithType &input, const ArithType &max) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        return input - std::floor(input / max) * max;
    }

    #endif // BTILS_ANGLE_hpp

    /** Clamp a value to a range
     * @tparam ArithType An arithmetic data type for the input & range
     * @param input The value to clamp
     * @param rangeMin The smallest value making up the range (inclusive)
     * @param rangeMax The largest value making up the range (inclusive)
     * @returns A value clammped to the given range
     */
    template <typename ArithType> ArithType clamp(const ArithType &input, const ArithType &rangeMin, const ArithType &rangeMax) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        return input < rangeMin ? rangeMin : (input > rangeMax ? rangeMax : input);
    }
    /** Clamp a value to a range with a minimum value of 0
     * @tparam ArithType An arithmetic data type for the input & range
     * @param input The value to clamp
     * @param max The largest value making up the range (inclusive)
     * @returns A value clammped to the given range
     */
    template <typename ArithType> ArithType clamp(const ArithType &input, const ArithType &max) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        return input < 0 ? 0 : (input > max ? max : input);
    }

    /** Find the greatest common divisor (or greatest common factor) of two numbers
     * @tparam IntegralType An integral data type
     * @param num1 The first number to find the gcd of
     * @param num2 The second number to find the gcd of
     * @returns The greatest common divisor of the two inputted numbers (or 1 if either num1 or num2 are 0)
     */
    template <typename IntegralType> IntegralType gcd(const IntegralType &num1, const IntegralType &num2) {
        static_assert(std::is_integral<IntegralType>::value, "IntegralType must be an integral type");
        if (num1 == 0 || num2 == 0) {
            return 1;
        }
        IntegralType a = num1, b = num2;

        while (a > 0 && b > 0) {
            if (a > b) {
                a %= b;
            } else {
                b %= a;
            }
        }
        return a == 0 ? b : a;
    }
    /** Find the least common multiple between two numbers (with different input/output types)
     * @tparam InIntegralType An integral data type for the two inputs
     * @tparam OutIntegralType An integral data type for the outputted result, it is recommended to make this a larger data type to avoid overflow errors
     * @param num1 The first number to find the lcm of
     * @param num2 The second number to find the lcm of
     * @returns The least common multiple of the two inputted numbers as an OutIntegralType
     */
    template <typename InIntegralType, typename OutIntegralType> OutIntegralType lcm(const InIntegralType &num1, const InIntegralType &num2) {
        static_assert(std::is_integral<InIntegralType>::value, "InIntegralType must be an integral type");
        static_assert(std::is_integral<OutIntegralType>::value, "OutIntegralType must be an integral type");
        return (OutIntegralType)(num1 / btils::gcd<InIntegralType>(num1, num2) * num2);
    }
    /** Find the least common multiple between two numbers (with the same input/output types)
     * @tparam IntegralType An integral data type
     * @param num1 The first number to find the lcm of
     * @param num2 The second number to find the lcm of
     * @returns The least common multiple of the two inputted numbers
     */
    template <typename IntegralType> IntegralType lcm(const IntegralType &num1, const IntegralType &num2) {
        static_assert(std::is_integral<IntegralType>::value, "IntegralType must be an integral type");
        return btils::lcm<IntegralType, IntegralType>(num1, num2);
    }
}

#endif // BTILS_MAIN_hpp
