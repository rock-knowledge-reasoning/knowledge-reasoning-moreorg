#include <set>
#include <iostream>
#include <owl_om/Combinatorics.hpp>

void append(std::vector<char>& v, char c, size_t n)
{
        for(size_t i = 0; i < n; ++i)
        {
            v.push_back(c);
        }
}

int main()
{
    size_t numberOfTypes;

    std::vector<char> input;
    std::cout << "How many different types are being used:" << std::endl;

    std::cin >> numberOfTypes;

    char currentType = 'a';
    std::cout << "Please provide the available number of items per type:" << std::endl;
    for(int i = 0; i < numberOfTypes; ++i)
    {
        size_t numberPerType;
        std::cout << currentType << ": ";
        std::cin >> numberPerType;
        append(input, currentType, numberPerType);
        ++currentType;
    }

    std::string startString(input.begin(), input.end());
    std::cout << "Computing combinations for: " << startString << std::endl;
    std::cout << "Total size: " << startString.size() << std::endl;

    {
        std::cout << "#SizeOfDraw   #Combinations" << std::endl;
        for(int i=1; i <= input.size();++i)
        {
            base::combinatorics::Combination<char> combinations( input, i, base::combinatorics::EXACT);
            size_t count = 0;
            do {
                std::vector<char> characterCombo = combinations.current();
                //std::cout << std::string(characterCombo.begin(), characterCombo.end()) << std::endl;
                ++count;
            } while(combinations.next());
            std::cout << i << " " << count << std::endl;
        }

        std::cout << "#MaxSizeOfDraw   #Combinations" << std::endl;
        for(int i=1; i <= input.size();++i)
        {
            base::combinatorics::Combination<char> combinations( input, i , base::combinatorics::MAX);
            size_t count = 0;
            do {
                std::vector<char> characterCombo = combinations.current();
                //std::cout << std::string(characterCombo.begin(), characterCombo.end()) << std::endl;
                ++count;
            } while(combinations.next());
            std::cout << i << " " << count << std::endl;
        }
    }

    return 0;
}

