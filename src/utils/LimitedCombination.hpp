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
 * mapping the object for each combination can be done in O(n) since
 * we are using a type maplist that maps in O(1) for each of the n 
 * items
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
     * Prepare limite combinations
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

    std::vector<AtomType> current()
    {
        std::vector<CoreType> current = mpItemCombinationGenerator->current();
        std::vector<AtomType> atomTypeList = mapToAtomTypes(current);
        std::sort(atomTypeList.begin(), atomTypeList.end());
        return atomTypeList;
    }

    bool next()
    {
       return mpItemCombinationGenerator->next(); 
    }

};

} // numeric
#endif // BASE_NUMERIC_LIMITED_COMBINATION_HPP__
