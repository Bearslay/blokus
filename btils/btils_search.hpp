#ifndef BTILS_SEARCH_hpp
#define BTILS_SEARCH_hpp

#include <vector>
#include <optional>

namespace btils {
    /** Get the index of a target element within an unsorted/sorted list (note that it is recommended to use binary search for sorted lists)
     * @tparam Type Any datatype/class; must have equality operators available
     * @param list The unsorted/sorted list to search through
     * @param target The element being targetted
     * @param lowerBounds The lowest index within the list to search from (can be used to search a particular section of a larger list)
     * @param upperBounds the highest index within the list to search to (can be used to search a particular section of a larger list)
     * @returns An std::optional object that either contains the index of the target element if found, or will be an std::nullopt if the target is not present in the list
     */
    template <typename Type> std::optional<std::size_t> linearSearch(const std::vector<Type> &list, const Type &target, const std::size_t &lowerBounds = 0, const std::size_t &upperBounds = __UINT64_MAX__) {
        if (list.size() == 0) {return std::nullopt;}

        std::size_t lb = lowerBounds >= list.size() ? list.size() - 1 : lowerBounds;
        std::size_t ub = upperBounds >= list.size() ? list.size() - 1 : upperBounds;
        for (std::size_t index = lb; index <= ub; index++) {
            if (list.at(index) == target) {return index;}
        }
        return std::nullopt;
    }
    
    /** Get the index of a target element within a sorted list
     * @tparam Type Any datatype/class; must have relational and equality operators available
     * @param list The sorted list to search through
     * @param target The element being targetted
     * @param lowerBounds The lowest index within the list to search from (can be used to search a particular section of a larger list)
     * @param upperBounds the highest index within the list to search to (can be used to search a particular section of a larger list)
     * @returns An std::optional object that either contains the index of the target element if found, or will be an std::nullopt if the target is not present in the list
     */
    template <typename Type> std::optional<std::size_t> binarySearch(const std::vector<Type> &list, const Type &target, const std::size_t &lowerBounds = 0, const std::size_t &upperBounds = __UINT64_MAX__) {
        if (list.size() == 0) {return std::nullopt;}

        std::size_t lb = lowerBounds > list.size() ? list.size() - 1 : lowerBounds;
        std::size_t ub = upperBounds > list.size() ? list.size() - 1 : upperBounds;
        while (lb <= ub) {
            std::size_t index = lb + (ub - lb) / 2;
            if (list.at(index) == target) {return index;}
            if (list.at(index) < target) {lb = index + 1;}
            else {ub = index - 1;}
        }
        return std::nullopt;
    }
}

#endif // BTILS_SEARCH_hpp
