#include "Combinatorics.hpp"
#include <boost/assign/list_of.hpp>

namespace base {
namespace combinatorics {

std::map<Mode, std::string> ModeTxt = boost::assign::map_list_of
    (EXACT, "EXACT")
    (MIN, "MIN")
    (MAX, "MAX");


uint64_t binomialCoefficient(uint32_t n, uint32_t k)
{
    if(k == 0 || n == k)
    {
        return 1;
    } else if(k > n || n == 0)
    {
        return 0;
    } else if(k == 1)
    {
        return n;
    } else if(k > n - k)
    {
        k = n - k;
    }

    uint64_t result = n - k + 1;

    for(uint32_t i = 2; i <= k; ++i)
    {
        result *= (n - k + i);
        result /= i;
    }
    return result;
}

} // end namespace combinatorics
} // end namespace base
