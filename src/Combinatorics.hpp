#ifndef BASE_NUMERIC_COMBINATORICS
#define BASE_NUMERIC_COMBINATORICS

#include <algorithm>
#include <vector>
#include <stdexcept>
#include <stdint.h>
#include <base/Logging.hpp>

namespace base {
namespace combinatorics {

inline size_t factorial(size_t n) 
{
    return (n == 1 || n == 0) ? 1 : factorial(n-1) * n;
}

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

template <class T>
class Combination
{
public:
    typedef typename std::vector<T> ItemList;
    enum Type { EXACT = 0, MAX, MIN };

private:
    std::vector<T> mItems;

    size_t mSizeOfDraw;
    // Binary code to define the current combination
    // 1 stands for present items, 0 for not present items
    uint64_t mCurrentCombination;

    typedef std::vector<size_t> Draw;
    Draw mDraw;


    Type mType;
    size_t mNumberOfCombinations;
    uint64_t mFoundCombinations;

    std::vector<size_t> getDraw()
    {
        uint64_t binaryCode = mCurrentCombination;
        std::vector<size_t> drawPositions;

        size_t position = 0;
        std::stringstream ss;
        while(true)
        {
            if( 0x01 & binaryCode)
            {
                ss << 1;
                drawPositions.push_back(position);
            } else {
                ss << 0;
            }

            binaryCode = binaryCode >> 1;
            if(binaryCode == 0)
            {
                return drawPositions;
            }
            ++position;
        }
        return drawPositions;
    }

public:
    /**
     * \class Combination of a unique item map
     * \param itemMap map of unique item types
     */
    Combination(const std::vector<T>& uniqueItems, size_t sizeOfDraw, Type type = EXACT)
        : mItems(uniqueItems)
        , mSizeOfDraw(sizeOfDraw)
        , mCurrentCombination(0)
        , mType(type)
        , mNumberOfCombinations(0)
        , mFoundCombinations(0)
    {
        std::sort(mItems.begin(), mItems.end());
        mNumberOfCombinations = numberOfCombinations();
        if(sizeOfDraw > uniqueItems.size())
        {
            throw std::invalid_argument("base::combinatorics::Combination: size of draw is greater than number of available items");
        }
    }

    bool next()
    {
        if(mFoundCombinations >= mNumberOfCombinations)
        {
            return false;
        }

        ++mCurrentCombination;
        mDraw = getDraw();
        while(true)
        {
            switch(mType)
            {
                case EXACT:
                {
                    if(mDraw.size() == mSizeOfDraw)
                    {
                        ++mFoundCombinations;
                        return true;
                    }
                    break;
                }
                case MIN:
                {
                    if(mDraw.size() >= mSizeOfDraw)
                    {
                        ++mFoundCombinations;
                        return true;
                    }
                    break;
                }
                case MAX:
                {
                    if(mDraw.size() <= mSizeOfDraw)
                    {
                        ++mFoundCombinations;
                        return true;
                    }
                    break;
                }
                default:
                    throw std::runtime_error("Invalid type given to switch");
            } // end switch
        }
    }

    ItemList current() const
    {
        ItemList draw;
        Draw::const_iterator cit = mDraw.begin();
        for(; cit != mDraw.end(); ++cit)
        {
            draw.push_back( mItems[*cit] );
        }
        return draw;
    }

    uint32_t numberOfCombinations() const
    {
        switch(mType)
        {
            case EXACT:
            {
                return numberOfCombinations(mSizeOfDraw);
            }
            case MIN:
            {
                uint32_t sum = 0;
                for(uint32_t i = mSizeOfDraw; i <= mItems.size(); ++i)
                {
                    sum += numberOfCombinations(i);
                }
                return sum;
            }
            case MAX:
            {
                uint32_t sum = 0;
                for(uint32_t i = 1; i <= mSizeOfDraw; ++i)
                {
                    sum += numberOfCombinations(i);
                }
                return sum;
            }
            default:
                throw std::runtime_error("Invalid type given to switch");
        } // end switch
    }

    uint32_t numberOfCombinations(uint32_t drawSize) const
    {
        uint32_t nominator = factorial(mItems.size());
        LOG_DEBUG_S << "Items: " << mItems.size();
        uint32_t denominator = factorial(mItems.size() - drawSize)*factorial(drawSize);
        LOG_DEBUG_S << "denominator: " << denominator;
        return nominator / denominator;
    }
};

} // end namepsace combinatorics
} // end namespace base
#endif // BASE_NUMERIC_COMBINATORICS__

