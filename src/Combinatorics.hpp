#ifndef BASE_NUMERIC_COMBINATORICS_HPP
#define BASE_NUMERIC_COMBINATORICS_HPP

#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <stdint.h>
#include <math.h>
#include <base/Logging.hpp>

#include <iostream>
#include <boost/foreach.hpp>

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
    typedef std::vector< uint32_t > DrawList;

    Mode mMode;

    std::vector<T> mCurrentDraw;

    int x, y, z;
    int* p;
    int* b;
    int mCurrentDrawSize;

    DrawList mDrawList;
    DrawList::const_iterator mCurrentDrawList;

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
        , p(0)
        , b(0)
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
                createStartDraw(numberOfItems, mSizeOfDraw);
                mDrawList.push_back(mSizeOfDraw);
                break;
            }
            case MIN:
            {
                createStartDraw(numberOfItems, mSizeOfDraw);
                mDrawList.push_back(mSizeOfDraw);

                for(uint32_t i = mSizeOfDraw + 1; i <= numberOfItems; ++i)
                {
                    mDrawList.push_back(i);
                }
                break;
            }
            case MAX:
            {
                createStartDraw(numberOfItems, 1);
                mDrawList.push_back(1);

                for(uint32_t i = 2; i <= mSizeOfDraw; ++i)
                {
                    mDrawList.push_back(i);
                }
                break;
            }
            default:
                throw std::runtime_error("Invalid type given to switch");

        }
        mCurrentDrawList = mDrawList.begin();

        LOG_DEBUG_S << "Creating Combination: n = " << numberOfItems << ", k = " << sizeOfDraw << std::endl
            << "    expected number of combinations for (mode: " << ModeTxt[mode] << "): " << numberOfCombinations() << std::endl;
    }

    void createStartDraw(uint32_t n, uint32_t k)
    {
        mCurrentDraw.clear();
        mCurrentDrawSize = k;

        p = new int[n+2];
        b = new int[n];

        init_twiddle(k, n, p);

        size_t i = 0;
        for(; i != n-k; ++i)
        {
            b[i] = 0;
        }

        while(i != n)
        {
            mCurrentDraw.push_back( mItems[i]);
            b[i++] = 1;
        }
    }

    bool next()
    {
        if(!twiddle(&x, &y, &z, p))
        {
            b[x] = 1;
            b[y] = 0;

            mCurrentDraw.clear();
            for(size_t i = 0; i != mItems.size(); ++i)
            {
                if(b[i])
                {
                    mCurrentDraw.push_back(mItems[i]);
                }
            }
            return true;
        } else {
            delete[] p;
            delete[] b;

            if(mCurrentDrawList + 1 != mDrawList.end())
            {
                ++mCurrentDrawList;
                createStartDraw(mItems.size(), *mCurrentDrawList);
                return true;
            }

            return false;
        }

        return false;
    }

    ItemList current() const
    {
        return mCurrentDraw;
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

    /*
      The implementation uses: twiddle.c -- retrieved from http://www.netlib.no/netlib/toms/382
      Twiddle generates all combinations of M elements drawn without replacement
      from a set of N elements.  This routine may be used in two ways:
      (0) To generate all combinations of M out of N objects, let a[0..N-1]
          contain the objects, and let c[0..M-1] initially be the combination
          a[N-M..N-1].  While twiddle(&x, &y, &z, p) is false, set c[z] = a[x] to
          produce a new combination.
      (1) To generate all sequences of 0's and 1's containing M 1's, let
          b[0..N-M-1] = 0 and b[N-M..N-1] = 1.  While twiddle(&x, &y, &z, p) is
          false, set b[x] = 1 and b[y] = 0 to produce a new sequence.
      In either of these cases, the array p[0..N+1] should be initialised as
      follows:
        p[0] = N+1
        p[1..N-M] = 0
        p[N-M+1..N] = 1..M
        p[N+1] = -2
        if M=0 then p[1] = 1
      In this implementation, this initialisation is accomplished by calling
      inittwiddle(M, N, p), where p points to an array of N+2 ints.
    
      Coded by Matthew Belmonte <mkb4@Cornell.edu>, 23 March 1996.  This
      implementation Copyright (c) 1996 by Matthew Belmonte.  Permission for use and
      distribution is hereby granted, subject to the restrictions that this
      copyright notice and reference list be included in its entirety, and that any
      and all changes made to the program be clearly noted in the program text.
    
      This software is provided 'as is', with no warranty, express or implied,
      including but not limited to warranties of merchantability or fitness for a
      particular purpose.  The user of this software assumes liability for any and
      all damages, whether direct or consequential, arising from its use.  The
      author of this implementation will not be liable for any such damages.
    
      Reference:
    
      Phillip J Chase, `Algorithm 382: Combinations of M out of N Objects [G6]',
      Communications of the Association for Computing Machinery 13:6:368 (1970).
    
      The returned indices x, y, and z in this implementation are decremented by 1,
      in order to conform to the C language array reference convention.  Also, the
      parameter 'done' has been replaced with a Boolean return value.
    */
    
    int twiddle(int* x,int* y,int* z,int* p)
    {
        register int i, j, k;
        j = 1;
        while(p[j] <= 0)
          j++;
        if(p[j-1] == 0)
          {
          for(i = j-1; i != 1; i--)
            p[i] = -1;
          p[j] = 0;
          *x = *z = 0;
          p[1] = 1;
          *y = j-1;
          }
        else
          {
          if(j > 1)
            p[j-1] = 0;
          do
            j++;
          while(p[j] > 0);
          k = j-1;
          i = j;
          while(p[i] == 0)
            p[i++] = -1;
          if(p[i] == -1)
            {
            p[i] = p[k];
            *z = p[k]-1;
            *x = i-1;
            *y = k-1;
            p[k] = -1;
            }
          else
            {
            if(i == p[0])
          return(1);
            else
          {
          p[j] = p[i];
          *z = p[i]-1;
          p[i] = 0;
          *x = j-1;
          *y = i-1;
          }
            }
          }
        return(0);
    }
    
    void init_twiddle(int m, int n,int* p)
    {
      int i;
      p[0] = n+1;
      for(i = 1; i != n-m+1; i++)
        p[i] = 0;
      while(i != n+1)
        {
        p[i] = i+m-n;
        i++;
        }
      p[n+1] = -2;
      if(m == 0)
        p[1] = 1;
    }

    void comptwiddle(int N, int M)
    {
      int i, x, y, z, p[N+2], b[N];
      init_twiddle(M, N, p);
      for(i = 0; i != N-M; i++)
      {
        b[i] = 0;
        putchar('0');
      }
      while(i != N)
      {
        b[i++] = 1;
        putchar('1');
      }
      putchar('-');
      putchar('\n');
      while(!twiddle(&x, &y, &z, p))
      {
        b[x] = 1;
        b[y] = 0;
        for(i = 0; i != N; i++)
          putchar(b[i]? '1': '0');
      putchar('x');
        putchar('\n');
      }
    }

    /************************
      Here is a sample use of twiddle() and inittwiddle():
    #define N 5
    #define M 2
    #include <stdio.h>
    void main()
      {
      int i, x, y, z, p[N+2], b[N];
      inittwiddle(M, N, p);
      for(i = 0; i != N-M; i++)
        {
        b[i] = 0;
        putchar('0');
        }
      while(i != N)
        {
        b[i++] = 1;
        putchar('1');
        }
      putchar('\n');
      while(!twiddle(&x, &y, &z, p))
        {
        b[x] = 1;
        b[y] = 0;
        for(i = 0; i != N; i++)
          putchar(b[i]? '1': '0');
        putchar('\n');
        }
      }
    ************************/

    // end of twiddle.c
};



} // end namepsace combinatorics
} // end namespace base
#endif // BASE_NUMERIC_COMBINATORICS_HPP
