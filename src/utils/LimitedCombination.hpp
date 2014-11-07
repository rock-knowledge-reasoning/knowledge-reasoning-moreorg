#ifndef BASE_NUMERIC_LIMITED_COMBINATION_HPP__
#define BASE_NUMERIC_LIMITED_COMBINATION_HPP__

#include <map>
#include <vector>
#include <numeric/Combinatorics.hpp>

namespace numeric {

/**
 * Allow combinatorics on a given set of limited resources
 *
 * Use integers to compute the core combinatorics, i.e. avoiding the 
 * computational effort for comparison of custom objects
 *
 \beginverbatim
 using namespace numeric;
 std::map<std::string, size_t> items;
 items["item-1"] = 2;
 items["item-2"] = 2;
 items["item-3"] = 2;

 LimitedCombination<std::string> combinations(items, LimitedCombination<std::string>::totalNumberOfAtoms(items), MAX);
 do {
    std::vector<std::string> currentCombination = combinations.current();
 } while(combinations.next());
 \endverbatim
 */
template<typename AtomType>
class LimitedCombination
{
    typedef uint32_t CoreType ;
    typedef std::map< AtomType, size_t> AtomType2CountMap;
    typedef std::vector<AtomType> AtomTypeList;
    typedef std::vector<CoreType> CombinationDescriptor;

    // Model with the number of available models of this type
    AtomType2CountMap mAtomTypeAvailablilityMap;
    AtomTypeList mAtomTypeList;

    std::vector<CoreType> mItems;
    numeric::Combination<CoreType>* mpItemCombinationGenerator;

    size_t mSize;
    numeric::Mode mMode;

    /**
     * Prepare limited combinations, i.e. setup the internal
     * item vector which will be used to determine the combination
     */
    void prepare()
    {
        CoreType currentType = 0;
        typename AtomType2CountMap::const_iterator cit = mAtomTypeAvailablilityMap.begin();
        for(; cit != mAtomTypeAvailablilityMap.end(); ++cit)
        {
            mAtomTypeList.push_back(cit->first);
            size_t numberPerType = cit->second;
            mItems.insert(mItems.begin(), numberPerType, currentType);
            ++currentType;
        }

        mpItemCombinationGenerator = new numeric::Combination<CoreType>(mItems, mSize, mMode);
    }

    /**
     * Map the internal combination to external types
     * \ŗeturn Combination of custom types
     */
    std::vector<AtomType> mapToAtomTypes(const std::vector<CoreType>& combination)
    {
        std::vector<AtomType> atomTypeList;
        std::vector<CoreType>::const_iterator cit = combination.begin();
        for(; cit != combination.end(); ++cit)
        {
            atomTypeList.push_back( mAtomTypeList[*cit] );
        }
        return atomTypeList;
    }

public:
    /**
     * Construct limited combination generator
     * \param countMap Mapping a item (type) to the maximum possible number of occurence
     * \param size In combination with mode, define the combination size 
     * \param mode Interpretation of the combination size, i.e. can be exact, min or max
     */
    LimitedCombination(const AtomType2CountMap& countMap, size_t size, numeric::Mode mode)
        : mAtomTypeAvailablilityMap(countMap)
        , mpItemCombinationGenerator(0)
        , mSize(size)
        , mMode(mode)
    {
        size_t totalCount = totalNumberOfAtoms(mAtomTypeAvailablilityMap);
        if(mSize > totalCount)
        {
            mSize = totalCount;
        }

        prepare();
    }

    /**
     * Get the totalNumberOfAtoms, i.e. the sum of occurrence
     * \return total number of atoms defined by the given map
     */
    static size_t totalNumberOfAtoms(const AtomType2CountMap& countMap)
    {
        typename AtomType2CountMap::const_iterator cit = countMap.begin();
        size_t count = 0;
        for(; cit != countMap.end(); ++cit)
        {
            count += cit->second;
        }
        return count;
    }

    /**
     * Get the current custom combination
     * \return current custom combination
     */
    std::vector<AtomType> current()
    {
        std::vector<CoreType> current = mpItemCombinationGenerator->current();
        std::vector<AtomType> atomTypeList = mapToAtomTypes(current);
        std::sort(atomTypeList.begin(), atomTypeList.end());
        return atomTypeList;
    }

    /**
     * Check if there is a next combination and 
     * forward internal iterator, so that current allows
     * to retrieve this combination
     * \return true if there is another valid combination
     */
    bool next()
    {
       return mpItemCombinationGenerator->next(); 
    }

};

} // numeric
#endif // BASE_NUMERIC_LIMITED_COMBINATION_HPP__
