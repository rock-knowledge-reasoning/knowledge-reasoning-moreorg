#ifndef MULITAGENT_UTILS_INTEGER_PARTITIONING_HPP
#define MULITAGENT_UTILS_INTEGER_PARTITIONING_HPP

#include <map>
#include <set>
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/foreach.hpp>

namespace multiagent {
namespace utils {

typedef std::vector<int> IntegerPartition;

/**
 * Utitility class that allows to compute all integer partitions of a given integer
 * It uses the existing algorithm a Jerome Kelleher
 */
class IntegerPartitioning
{
public:
    typedef std::map<size_t, std::set< IntegerPartition > > PartitionsMap;

    PartitionsMap mPartitionsMap;

public:

    /**
     * Generating All Partitions: A Comparison Of Two Encodings
     * Jerome Kelleher, Barry O'Sullivan
     * 
     * Translated from python and retrieved from http://jeromekelleher.net/partitions.php
     */
    void compute(size_t number);

    /**
     * Print all generated partitions
     */    
    std::string toString() const;

    /**
     * Retrieve the partitions map
     */
    PartitionsMap getPartitionsMap() const { return mPartitionsMap; }

    /**
     * Compute the multiplicity of a value in a given integer partition
     */
    static size_t multiplicity(const IntegerPartition& partition, int value);

};

std::ostream& operator<<(std::ostream& os, const IntegerPartition& list);

} // end namespace utils
} // end namespace multiagent
#endif // MULITAGENT_UTILS_INTEGER_PARTITIONING_HPP
