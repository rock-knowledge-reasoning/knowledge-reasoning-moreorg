#include <iostream>
#include <numeric/LimitedCombination.hpp>
#include <set>

// Compute the 'simple' number of combination or a set of types with limited
// occurrence per type, vs. the occurrence of up to a given number for all types
// (which is the standard formula using the binomialcoefficent of |n + k -1 | |
// k       |
int main()
{
    size_t numberOfTypes;

    std::vector<char> input;
    std::cout << "How many different types are being used:" << std::endl;

    std::cin >> numberOfTypes;

    std::map<char, size_t> availabilityMap;
    char currentType = 'a';
    std::cout << "Please provide the available number of items per type:"
              << std::endl;
    for(size_t i = 0; i < numberOfTypes; ++i)
    {
        size_t numberPerType;
        std::cout << currentType << ": ";
        std::cin >> numberPerType;
        input.insert(input.begin(), numberPerType, currentType);
        availabilityMap[currentType] = numberPerType;
        ++currentType;
    }

    size_t numberOfAtoms =
        numeric::LimitedCombination<char>::totalNumberOfAtoms(availabilityMap);

    std::vector<size_t> exactCombinations;
    for(size_t i = 1; i <= numberOfAtoms; ++i)
    {
        int count = 0;
        numeric::LimitedCombination<char> combinations(availabilityMap,
                                                       i,
                                                       numeric::EXACT);
        do
        {
            std::vector<char> charactorCombo = combinations.current();
            count++;
        } while(combinations.next());
        exactCombinations.push_back(count);
    }

    std::vector<size_t> maxCombinations;
    for(size_t i = 1; i <= numberOfAtoms; ++i)
    {
        int count = 0;
        numeric::LimitedCombination<char> combinations(availabilityMap,
                                                       i,
                                                       numeric::MAX);
        do
        {
            std::vector<char> charactorCombo = combinations.current();
            count++;
        } while(combinations.next());
        maxCombinations.push_back(count);
    }

    std::cout << "# Computing the number of agent given and exact or maximum "
                 "coalition size"
              << std::endl;
    std::cout << "#SizeOfDraw ExactCoalitionSize MaxCoalitionSize" << std::endl;
    for(size_t i = 1; i <= numberOfAtoms; ++i)
    {
        std::cout << i << " " << exactCombinations[i - 1] << " "
                  << maxCombinations[i - 1] << std::endl;
    }

    return 0;
}
