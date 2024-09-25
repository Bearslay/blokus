#ifndef BTILS_STRING_hpp
#define BTILS_STRING_hpp

#include <type_traits>
#include <string>

namespace btils {
    /** Convert an arithmetic data type to an std::string with some extra formatting for floating-point types
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a string
     * @returns An std::string where each character represents a part of the input; trailing zeros and decimals are omitted from floating-point conversions
     */
    template <typename ArithType> std::string toString(const ArithType &input) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");

        // std::to_string() is acceptable for integral types, floating point types are where the formatting is needed
        if (std::is_integral<ArithType>::value) {return std::to_string(input);}

        std::string output = std::to_string(input);
        // Remove the last character if it is either a zero or a decimal
        while (output.back() == '0') {
            output.pop_back();
            // Break seperately at the decimal so that zeros in positive places aren't removed
            if (output.back() == '.') {
                output.pop_back();
                break;
            }
        }
        return output;
    }

    /** Convert an arithmetic data type to an std::string with some extra formatting
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a string
     * @param alwaysIncludeSign Whether to include a positive sign if the input is positive or not
     * @returns An std::string where each character represents a part of the input and also beginning with either a positive or negative sign; trailing zeros and decimals are omitted from floating-point conversions
     */
    template <typename ArithType> std::string toString(const ArithType &input, const bool alwaysIncludeSign) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        return (alwaysIncludeSign && input >= 0 ? "+" : "") + btils::toString<ArithType>(input);
    }

    /** Convert an arithmetic data type to an std::string with extra leading/trailing zeros if specified
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a string
     * @param leftDigits The minimum amount of digits that will be to the left of the decimal; metric reached by adding leading zeros
     * @param rightDigits The minimum amount of digits that will be to the right of the decimal; metric reached by adding trailing zeros
     * @param alwaysIncludeSign Whether to include a positive sign if the input is positive or not (sign counts as a 'left digit')
     * @returns An std::string where each character represents a part of the input and also beginning with either a positive or negatie sign; leading and trailing zeros are also included in order to meet certain parameters
     */
    template <typename ArithType> std::string tstr_AddZeros(const ArithType &input, const unsigned long int &leftDigits, const unsigned long int &rightDigits, const bool &alwaysIncludeSign) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        std::string output = btils::toString<ArithType>(input, alwaysIncludeSign);
        const unsigned long int decimalPos = output.find('.');
        const unsigned long int before = decimalPos == std::string::npos ? output.length() : decimalPos, after = decimalPos == std::string::npos ? 0 : output.length() - before - 1;
        const bool insertPos = alwaysIncludeSign || input < 0;

        // Add on the leading zeros
        for (unsigned long int i = before; i < leftDigits; i++) {
            output.insert(insertPos, "0");
        }
        // Return early if no trailing zeros are needed
        if (rightDigits == 0) {return output;}

        // Add a decimal place if it is missing
        if (decimalPos == std::string::npos) {output += ".";}
        // Add on the trailing zeros
        for (unsigned long int i = after; i < rightDigits; i++) {
            output += '0';
        }
        return output;
    }

    /** Convert an arithmetic data type to an std::string with extra leading/trailing zeros if specified
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a string
     * @param leftDigits The minimum amount of digits that will be to the left of the decimal; metric reached by adding leading zeros
     * @param rightDigits The minimum amount of digits that will be to the right of the decimal; metric reached by adding trailing zeros
     * @returns An std::string where each character represents a part of the input; leading and trailing zeros are also included in order to meet certain parameters
     */
    template <typename ArithType> std::string tstr_AddZeros(const ArithType &input, const unsigned long int &leadingZeros, const unsigned long int &trailingZeros) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        return btils::tstr_AddZeros<ArithType>(input, leadingZeros, trailingZeros, false);
    }

    /** Convert an arithmetic data type to an std::string with a specified minimum length
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a string
     * @param length The minimum length of the output std::string (reached using leading/trailing zeros)
     * @param append Whether to append zeros (trailing zeros) or not (leading zeros) in order to reach the minimum length
     * @param alwaysIncludeSign Whether to include a positive sign if the input is positive or not (sign contributes to the desired length)
     * @returns An std::string where each character represents a part of the input and also beginning with either a positive or negatie sign; leading or trailing zeros are included to reach a specified minimum string length
     */
    template <typename ArithType> std::string tstr_Length(const ArithType &input, const unsigned long int &length, const bool &append, const bool &alwaysIncludeSign) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        std::string output = btils::toString<ArithType>(input);

        // Adding leading zeros until the desired length is reached
        if (!append) {
            const bool insertPos = alwaysIncludeSign || input < 0;
            while (output.length() < length) {
                output.insert(insertPos, "0");
            }
            return output;
        }
        
        if (output.find('.') == std::string::npos) {
            // If the output would exceed the desired length by adding on a '.0', then a leading zero is added instead
            if (output.length() >= length - 1) {
                output.insert(alwaysIncludeSign || input < 0, "0");
                return output;
            }
            output += ".0";
        }

        // Add trailing zeros until the desired length is reached
        while (output.length() < length) {
            output += "0";
        }
        return output;
    }

    /** Convert an arithmetic data type to an std::string with a specified minimum length
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a string
     * @param length The minimum length of the output std::string (reached using leading/trailing zeros)
     * @param append Whether to append zeros (trailing zeros) or not (leading zeros) in order to reach the minimum length
     * @returns An std::string where each character represents a part of the input; leading or trailing zeros are included to reach a specified minimum string length
     */
    template <typename ArithType> std::string tstr_Length(const ArithType &input, const unsigned long int &length, const bool &append) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        return btils::tstr_Length<ArithType>(input, length, append, false);
    }

    /** Convert an arithmetic data type to an std::u16string making use of the btils::toString() function
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a u16string
     * @returns An std::u16string where each character represents a part of the input; trailing zeros and decimals are omitted from floating-point conversions
     */
    template <typename ArithType> std::u16string to_u16string(const ArithType &input) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        std::string str = btils::toString<ArithType>(input);
        return {str.begin(), str.end()};
    }

    /** Convert an arithmetic data type to an std::u32string making use of the btils::toString() function
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a u32string
     * @returns An std::u32string where each character represents a part of the input; trailing zeros and decimals are omitted from floating-point conversions
     */
    template <typename ArithType> std::u32string to_u32string(const ArithType &input) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        std::string str = btils::toString<ArithType>(input);
        return {str.begin(), str.end()};
    }

    /** Convert an arithmetic data type to an std::wstring making use of the btils::toString() function
     * @tparam ArithType An arithmetic data type to convert from
     * @param input The arithmetic value to convert into a wstring
     * @returns An std::wstring where each character represents a part of the input; trailing zeros and decimals are omitted from floating-point conversions
     */
    template <typename ArithType> std::wstring to_wstring(const ArithType &input) {
        static_assert(std::is_arithmetic<ArithType>::value, "ArithType must be an arithmetic type");
        std::string str = btils::toString<ArithType>(input);
        return {str.begin(), str.end()};
    }

    /** Convert an std::string to an std::u16string
     * @param input The std::string to convert from
     * @returns An std::u16string derived from the inputted std::string
     */
    template <> std::u16string to_u16string<std::string>(const std::string &input) {return {input.begin(), input.end()};}

    /** Convert an std::string to an std::u32string
     * @param input The std::string to convert from
     * @returns An std::u32string derived from the inputted std::string
     */
    template <> std::u32string to_u32string<std::string>(const std::string &input) {return {input.begin(), input.end()};}

    /** Convert an std::string to an std::wstring
     * @param input The std::string to convert from
     * @returns An std::wstring derived from the inputted std::string
     */
    template <> std::wstring to_wstring<std::string>(const std::string &input) {return {input.begin(), input.end()};}
}

#endif // BTILS_STRING_hpp
