#ifndef BASE_NUMERIC_COMBINATORICS
#define BASE_NUMERIC_COMBINATORICS

#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <stdint.h>
#include <math.h>
#include <base/Logging.hpp>

namespace base {
namespace combinatorics {

inline uint64_t factorial(uint32_t n)
{
    return (n == 1 || n == 0) ? 1 : factorial(n-1) * n;
}

// Compute the binomial coefficient
// see http://de.wikipedia.org/wiki/Binomialkoeffizient for details
uint64_t binomialCoefficient(uint32_t n, uint32_t k);

template <class T>
class Permutation
{
public:
    typedef typename std::vector<T> ItemList;

private:
    ItemList mItems;
    bool mInitalized;

public:

    Permutation(const std::vector<T> items)
        : mItems(items)
        , mInitalized(false)
    {
        std::sort(mItems.begin(), mItems.end());
    }

    bool next()
    {
        return std::next_permutation(mItems.begin(), mItems.end());
    }

    const ItemList& current() const
    {
        return mItems;
    }

    size_t numberOfPermutations() const { return factorial(mItems.size()); }
};

enum Mode { EXACT = 0, MAX, MIN };
extern std::map<Mode, std::string> ModeTxt;

/**
 * \brief Combination of a unique item map Binomialcoefficient (n k)
 * \tparam Type of items that should be combined
 * \details A code example
 * \verbatim 
   #include <vector>
   #include <string>

   using namespace base::combinatorics;

   ...

   std::vector<std::string> items;
   items.push_back("A");
   items.push_back("B");
   items.push_back("C");

   Combination<std::string> combinations(items, EXACT);
   do {
       std::vector<std::string> combination = combinations.current();
       ...
   } while(combinations.next());
 \endverbatim
 */
template <class T>
class Combination
{
public:
    typedef typename std::vector<T> ItemList;

private:
    std::vector<T> mItems;
    uint32_t mSizeOfDraw;

    typedef std::vector<bool> Draw;
    typedef std::vector< Draw > DrawList;

    Mode mMode;
    DrawList mDrawList;
    DrawList::iterator mDrawListIterator;

public:
    /**
     * \brief Combination of a unique item map Binomialcoefficient (n k)
     * \param uniqueItems Map of unique items
     * \param sizeOfDraw Size of the draw (k)
     * \param mode One of Mode to get the exact type of combination all up to (MAX) or from to maximum use (MIN)
     */
    Combination(const std::vector<T>& uniqueItems, size_t sizeOfDraw, Mode mode = EXACT)
        : mItems(uniqueItems)
        , mSizeOfDraw(sizeOfDraw)
        , mMode(mode)
    {
        std::sort(mItems.begin(), mItems.end());
        if(sizeOfDraw > uniqueItems.size())
        {
            throw std::invalid_argument("base::combinatorics::Combination: size of draw is greater than number of available items");
        }

        uint32_t numberOfItems = mItems.size();
        switch(mMode)
        {
            case EXACT:
            {
                mDrawList.push_back( createStartDraw(numberOfItems, mSizeOfDraw) );
                break;
            }
            case MIN:
            {
                for(uint32_t i = mSizeOfDraw; i <= numberOfItems; ++i)
                {
                    mDrawList.push_back( createStartDraw(numberOfItems, i) );
                }
                break;
            }
            case MAX:
            {
                for(uint32_t i = 1; i <= mSizeOfDraw; ++i)
                {
                    mDrawList.push_back( createStartDraw(numberOfItems, i) );
                }
                break;
            }
            default:
                throw std::runtime_error("Invalid type given to switch");

        }
        LOG_DEBUG_S << "Creating Combination: n = " << numberOfItems << ", k = " << sizeOfDraw << std::endl
            << "    expected number of combinations for (mode: " << ModeTxt[mode] << "): " << numberOfCombinations() << std::endl;

        mDrawListIterator = mDrawList.begin();
    }

    Draw createStartDraw(uint32_t n, uint32_t k)
    {
        Draw draw;
        for(uint32_t i = 0; i < k; ++i)
        {
            draw.push_back(true);
        }
        for(uint32_t i = k; i < n; ++i)
        {
            draw.push_back(false);
        }
        std::sort(draw.begin(), draw.end());
        return draw;
    }

    bool next()
    {
        if(mDrawListIterator == mDrawList.end())
        {
            return false;
        }

        Draw& currentDraw = *mDrawListIterator;
        if(! std::next_permutation(currentDraw.begin(), currentDraw.end()) )
        {
            return ++mDrawListIterator != mDrawList.end();
        }
        return true;
    }

    ItemList current() const
    {
        ItemList draw;
        if(mDrawListIterator == mDrawList.end())
        {
            return draw;
        }

        const Draw& currentDraw = *mDrawListIterator;
        Draw::const_iterator cit = currentDraw.begin();

        uint32_t position = 0;
        for(; cit != currentDraw.end(); ++cit)
        {
            if( *cit )
            {
                draw.push_back( mItems[position] );
            }
            ++position;
        }
        return draw;
    }

    uint32_t numberOfCombinations() const
    {
        uint32_t numberOfItems = mItems.size();
        switch(mMode)
        {
            case EXACT:
            {
                return binomialCoefficient(numberOfItems, mSizeOfDraw);
            }
            case MIN:
            {
                uint32_t sum = 0;
                for(uint32_t i = mSizeOfDraw; i <= mItems.size(); ++i)
                {
                    sum += binomialCoefficient(numberOfItems, i);
                }
                return sum;
            }
            case MAX:
            {
                uint32_t sum = 0;
                for(uint32_t i = 1; i <= mSizeOfDraw; ++i)
                {
                    sum += binomialCoefficient(numberOfItems, i);
                }
                return sum;
            }
            default:
                throw std::runtime_error("Invalid type given to switch");
        } // end switch
    }
};

} // end namepsace combinatorics
} // end namespace base
#endif // BASE_NUMERIC_COMBINATORICS__

