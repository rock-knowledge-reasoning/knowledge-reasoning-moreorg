#include <boost/test/unit_test.hpp>
#include <owl_om/Combinatorics.hpp>
#include <sstream>

using namespace base::combinatorics;


BOOST_AUTO_TEST_CASE(it_should_generate_permutations)
{
    std::vector<int> items;
    items.push_back(1);
    items.push_back(2);
    items.push_back(0);
    items.push_back(4);

    Permutation<int> permutation(items);

    BOOST_ASSERT(permutation.numberOfPermutations() == 24);

    do
    {
        const std::vector<int>& permutedItems = permutation.current();
        BOOST_TEST_MESSAGE("Permutation " << permutedItems[0] << permutedItems[1] << permutedItems[2] << permutedItems[3]);
    } while(permutation.next());
}

BOOST_AUTO_TEST_CASE(it_should_generate_combinations)
{
    std::vector<int> items;
    items.push_back(1);
    items.push_back(2);
    items.push_back(0);
    items.push_back(4);

    Combination<int> combination(items,items.size(), Combination<int>::MAX);
    //BOOST_REQUIRE_MESSAGE( combination.numberOfCombinations(4) == 1, "Expect 1 got " << combination.numberOfCombinations(4));
    BOOST_TEST_MESSAGE( "Number of expected combinations: " << combination.numberOfCombinations() );

    do
    {
        std::vector<int> combinatedItems = combination.current();
        std::vector<int>::const_iterator cit = combinatedItems.begin();
        std::stringstream ss;
        for(; cit != combinatedItems.end(); ++cit)
        {
            ss << *cit;
        }
        BOOST_TEST_MESSAGE("Combination: " << ss.str());
    } while(combination.next());

    


}
