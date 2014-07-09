#ifndef BASE_NUMERIC_COMBINATORICS
#define BASE_NUMERIC_COMBINATORICS

#include <algorithm>
#include <vector>

namespace base {
namespace combinatorics {

inline size_t factorial(size_t n) 
{
    return (n == 1 || n == 0) ? 1 : factorial(n-1) * n;
}

template <class T>
class Permutation
{
    ItemList mItems;

public:
    typedef typename std::vector<T> ItemList;

    Permutation(const std::vector<T> items)
        : mItems(items)
    {
        std::sort(mItems.begin(), mItems.end());
    }

    bool next()
    {
        return std::next_permutation(mItems.begin(), mItems.end());
    }

    ItemList current() const
    {
        return mItems;
    }

    size_t numberOfPermutations() const { factorial(mItems.size()); }
};

template <class T>
class Combination
{
    size_t mSizeOfDraw;
    typedef typename std::map<T, size_t> NumberOfItemsMap;

public:
    typedef typename std::vector<T> ItemList;

    Combination(const NumberOfItemsMap& itemMap, size_t sizeOfDraw)
        : mNumberOfItemsMap(itemMap)
        , mSizeOfDraw(sizeOfDraw)
    {
        std::sort(mItems.begin(), mItems.end());
    }

    NumberOfItemsMap getNumberOfItemsMap(const std::vector<T>& items)
    {
        NumberOfItemsMap numberOfItemsMap;
        ItemList::const_iterator cit = items.begin();

        T item;
        int count = 0;
        if(!mItems.empty())
        {
            item = *cit;
            ++count;
        }

        for(; cit != mItems.end() ++cit)
        {
            T currentItem = *cit;
            if(currentItem != item)
            {
                numberOfItemsMap[item] = count;
                count = 1;
            } else {
                ++count;
            }
        }
        numberOfItemsMap[item] = count;
        return numerOfItemsMap;
    }

    size_t numberOfItems() const
    {
        NumberOfItemsMap::const_iterator cit = mNumberOfItemsMap.begin();
        size_t itemCount = 0;
        for(; cit != mNumberOfItemsMap.end(); ++cit)
        {
            itemCount += cit->second;
        }
        return itemCount;
    }

    bool next()
    {
        ItemList oldList = current();
        std::next_permutation(mItems.begin(), mItems.end());
    }

    size_t numberOfCombinations() const
    {
        size_t nominator = numberOfItems();
        size_t denominator = factorial(nominator - mSizeOfDraw)*factorial(mSizeOfDraw);
        return nominator / denominator;
    }
};

} // end namepsace combinatorics
} // end namespace base
#endif // BASE_NUMERIC_COMBINATORICS__

