#ifndef BTILS_ANGLE_hpp
#define BTILS_ANGLE_hpp

#include <type_traits>
#include <cmath>

/// @brief pi/8 rad or 22.5 deg
#define C_PI_8     0.39269908169872415481
/// @brief pi/6 rad or 30 deg
#define C_PI_6     0.52359877559829887308
/// @brief pi/4 rad or 45 deg
#define C_PI_4     0.78539816339744830961
/// @brief pi/3 rad or 60 deg
#define C_PI_3     1.04719755119659774615
/// @brief 3pi/8 rad or 67.5 deg
#define C_3PI_8    1.17809724509617246442
/// @brief pi/2 rad or 90 deg
#define C_PI_2     1.57079632679489661923
/// @brief 5pi/8 rad or 112.5 deg
#define C_5PI_8    1.96349540849362077404
/// @brief 2pi/3 rad or 120 deg
#define C_2PI_3    2.09439510239319549231
/// @brief 3pi/4 rad or 135 deg
#define C_3PI_4    2.35619449019234492885
/// @brief 5pi/6 rad or 150 deg
#define C_5PI_6    2.61799387799149436539
/// @brief 7pi/8 rad or 157.5 deg
#define C_7PI_8    2.74889357189106908365
/// @brief pi rad or 180 deg
#define C_PI       3.14159265358979323846
/// @brief 9pi/8 rad or 202.5 deg
#define C_9PI_8    3.53429173528851739327
/// @brief 7pi/6 rad or 210 deg
#define C_7PI_6    3.66519142918809211154
/// @brief 5pi/4 rad or 225 deg
#define C_5PI_4    3.92699081698724154808
/// @brief 4pi/3 rad or 240 deg
#define C_4PI_3    4.18879020478639098462
/// @brief 11pi/8 rad or 247.5 deg
#define C_11PI_8   4.31968989868596570289
/// @brief 3pi/2 rad or 270 deg
#define C_3PI_2    4.71238898038468985769
/// @brief 13pi/8 rad or 292.5 deg
#define C_13PI_8   5.10508806208341401250
/// @brief 5pi/3 rad or 300 deg
#define C_5PI_3    5.23598775598298873077
/// @brief 7pi/4 rad or 315 deg
#define C_7PI_4    5.49778714378213816731
/// @brief 11pi/6 rad or 330 deg
#define C_11PI_6   5.75958653158128760385
/// @brief 15pi/8 rad or 337.5 deg
#define C_15PI_8   5.89048622548086232212
/// @brief 2pi rad or 360 deg
#define C_2PI      6.28318530717958647693

/// @brief The value of sqrt(3)/2
#define C_SQRT3_2  0.86602540378443864676
/// @brief The value of sqrt(2)/2
#define C_SQRT2_2  0.70710678118654752440

/// @brief The value of cos(pi/8) or sqrt(2+sqrt(2))/2
#define C_MULT_1_8 0.92387953251128675613
/// @brief The value of cos(pi/6) or sqrt(3)/2
#define C_MULT_1_6 0.86602540378443864676
/// @brief The value of cos(pi/4) or sqrt(2)/2
#define C_MULT_1_4 0.70710678118654752440
/// @brief The value of cos(pi/3) or 1/2
#define C_MULT_1_3 0.50000000000000000000
/// @brief The value of cos(3pi/8) or sqrt(2-sqrt(2))/2
#define C_MULT_3_8 0.38268343236508977173

/// @brief The value of sqrt(2)
#define C_SQRT2    1.41421356237309504880
/// @brief The value of sqrt(3)
#define C_SQRT3    1.73205080756887729353

/// @brief The conversion factor from degrees to radians (pi/180)
#define U_PI_180   0.01745329251994329577
/// @brief The conversion factor from radians to degrees (180/pi)
#define U_180_PI   57.2957795130823208770

namespace btils {
    #ifndef BTILS_MAIN_hpp

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

    #endif // BTILS_MAIN_hpp

    /** Convert an angle from degrees to radians
     * @param angle An angle in degrees
     * @returns An angle in radians
     */
    double degToRad(const double &angle) {return angle * U_PI_180;}
    /** Convert an angle from radians to degrees
     * @param angle An angle in radians
     * @returns An angle in degrees
     */
    double radToDeg(const double &angle) {return angle * U_180_PI;}
    /** Convert an angle from either degrees to radians or radians to degrees
     * @param angle An angle in either radians or degrees
     * @param toRadians Whether to convert to radians or degrees
     * @returns An angle measured with the specified unit
     */
    double convertAngle(const double &angle, const bool &toRadians = true) {return toRadians ? btils::degToRad(angle) : btils::radToDeg(angle);}
    /** Normalize an angle (in degrees) to be in the range of [0, 360)
     * @param angle An angle in degrees
     * @returns An angle in degrees, but in the range of [0, 360)
     */
    double normalizeDegree(const double &angle) {return btils::normalize<double>(angle, 360.0);}
    /** Normalize an angle (in radians) to be in the range of [0, 2 * pi)
     * @param angle An angle in radians
     * @returns An angle in radians, but in the range of [0, 2 * pi)
     */
    double normalizeRadian(const double &angle) {return btils::normalize<double>(angle, C_2PI);}
    /** Normalize an angle to be in the range of either [0, 2 * pi) or [0, 360)
     * @param angle An angle in either radians or degrees
     * @param useRadians Whether to normalize to [0, 2 * pi) for radians or [0, 360) for degrees
     * @returns An angle, but in the range of either [0, 2 * pi) or [0, 360)
     */
    double normalizeAngle(const double &angle, const bool &useRadians = true) {return useRadians ? btils::normalizeRadian(angle) : btils::normalizeDegree(angle);}
}

#endif // BTILS_ANGLE_hpp
