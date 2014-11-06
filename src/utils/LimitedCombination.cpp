#include <set>
#include <iostream>
#include <owl_om/utils/LimitedCombination.hpp>

void append(std::vector<char>& v, char c, size_t n)
{
        for(size_t i = 0; i < n; ++i)
        {
            v.push_back(c);
        }
}

// Compute the 'simple' number of combination or a set of types with limited occurrence per type, vs.
// the occurrence of up to a given number for all types (which is the standard formula using the
// binomialcoefficent of
// |n + k -1 |
// | k       |
int main()
{
    size_t numberOfTypes;

    std::vector<char> input;
    std::cout << "How many different types are being used:" << std::endl;

    std::cin >> numberOfTypes;

    std::map<char, size_t> availabilityMap;
    char currentType = 'a';
    std::cout << "Please provide the available number of items per type:" << std::endl;
    for(size_t i = 0; i < numberOfTypes; ++i)
    {
        size_t numberPerType;
        std::cout << currentType << ": ";
        std::cin >> numberPerType;
        append(input, currentType, numberPerType);
        availabilityMap[currentType] = numberPerType;
        ++currentType;
    }

    size_t numberOfAtoms = numeric::LimitedCombination<char>::totalNumberOfAtoms(availabilityMap);

    std::cout << "#SizeOfDraw   #Combinations" << std::endl;
    for(int i=1; i <= numberOfAtoms; ++i)
    {
        int count = 0;
        numeric::LimitedCombination<char> combinations(availabilityMap, i, numeric::EXACT);
        do {
            std::vector<char> charactorCombo = combinations.current();
            count++;
        } while(combinations.next());
        std::cout << i << " " << count << std::endl;
    }

    std::cout << "#MaxSizeOfDraw   #Combinations" << std::endl;
    for(int i=1; i <= numberOfAtoms; ++i)
    {
        int count = 0;
        numeric::LimitedCombination<char> combinations(availabilityMap, i, numeric::MAX);
        do {
            std::vector<char> charactorCombo = combinations.current();
            count++;
        } while(combinations.next());
        std::cout << i << " " << count << std::endl;
    }

    return 0;
}

