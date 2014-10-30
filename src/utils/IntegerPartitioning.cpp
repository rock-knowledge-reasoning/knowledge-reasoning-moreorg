#include "IntegerPartitioning.hpp"

namespace multiagent {
namespace utils {

size_t IntegerPartitioning::multiplicity(const IntegerPartition& partition, int value)
{
    size_t count = 0;
    IntegerPartition::const_iterator cit = partition.begin();
    for(; cit != partition.end(); ++cit)
    {
        if(*cit == value)
        {
            ++count;
        }
    }
    return count;
}

std::ostream& operator<<(std::ostream& os, const std::vector<int>& list)
{
    os << "["; 
    std::vector<int>::const_iterator cit = list.begin();
    for(; cit != list.end(); ++cit)
    {
        os << *cit;
        if(cit + 1 != list.end())
        {
            os << ",";
        }
    }
    os << "]"; 
    return os;
}


//
// Generating All Partitions: A Comparison Of Two Encodings
// Jerome Kelleher, Barry O'Sullivan
// 
// from http://jeromekelleher.net/partitions.php
//    a = [0 for i in range(n + 1)]
//    k = 1
//    y = n - 1
//    while k != 0:
//        x = a[k - 1] + 1
//        k -= 1
//        while 2*x <= y:
//            a[k] = x
//            y -= x
//            k += 1
//        l = k + 1
//        while x <= y:
//            a[k] = x
//            a[l] = y
//            yield a[:k + 2]
//            x += 1
//            y -= 1
//        a[k] = x + y
//        y = x + y - 1
//        yield a[:k + 1]

void IntegerPartitioning::compute(size_t number)
{
    int n = number;
    std::vector<int> a;
    for(int i = 0; i <= n+1; ++i)
    {
        a.push_back(0);
    }
    int k = 1;
    int y = n -1;
    while(k != 0)
    {
        int x = a[k-1]+1;
        k -= 1;
        while( 2*x <= y)
        {
            a[k] = x;
            y -= x;
            k += 1;
        }
        int l = k + 1;
        while( x <= y)
        {
            a[k] = x;
            a[l] = y;
            std::vector<int> partition(a.begin(), a.begin() + k +2);
            mPartitionsMap[partition.size()].insert(partition);
            x += 1;
            y -= 1;
        }
        a[k] = x + y;
        y = x + y -1;

        std::vector<int> partition(a.begin(), a.begin() + k+1);
        mPartitionsMap[partition.size()].insert(partition);
    }
}

std::string IntegerPartitioning::toString() const
{
    std::stringstream ss;

    std::map<size_t, std::set< std::vector<int> > >::const_iterator it = mPartitionsMap.begin();

    for(; it != mPartitionsMap.end(); ++it)
    {
        ss << it->first << " : "; 
        std::set< std::vector<int> >::const_iterator plit = it->second.begin();
        for(; plit != it->second.end(); ++plit)
        {
            ss << *plit;
        }
        ss << std::endl;
    }
    return ss.str();
}

} // end namespace utils
} // end namespace multiagent
