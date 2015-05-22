#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <organization_model/ccf/CCF.hpp>
#include <organization_model/OrganizationModel.hpp>
#include <set>

using namespace organization_model;
using namespace owlapi::model;

BOOST_AUTO_TEST_SUITE(constraint_coalition_formation)

BOOST_AUTO_TEST_CASE(handle_sets)
{
    {
        Set<int> set;
        SetOfSets<int> setOfSets;

        SetOfSets<int> setUnion = setOfSets.createUnion(set);
        BOOST_REQUIRE_MESSAGE( setUnion.size() == 1, "Union with empty set " << setUnion.toString());
        BOOST_REQUIRE_MESSAGE( setUnion.containsEmptySet(), "Union with empty set " << setUnion.toString());
    }
    {
        Set<int> set;
        set.insert(0);
        set.insert(1);
        BOOST_REQUIRE_MESSAGE( set.size() == 2, "Set of size 2");

        SetOfSets<int> setOfSets;
        SetOfSets<int> setUnion = setOfSets.createUnion(set);
        BOOST_REQUIRE_MESSAGE( setUnion.size() == 1, "Union with non empty set " << setUnion.toString());
        BOOST_REQUIRE_MESSAGE( !setUnion.containsEmptySet(), "Union does not contain empty set " << setUnion.toString());
    }

}

BOOST_AUTO_TEST_CASE(handle_reference_ccf)
{

    { // Scenario form Rahwan et. al.

        CCF<IRI>::Atoms atoms;

        IRI a("1"); 
        IRI b("2"); 
        IRI c("3"); 
        IRI d("4"); 
        IRI e("5"); 
        IRI f("6"); 
        IRI g("7"); 
        IRI h("8"); 

        atoms.insert(a);
        atoms.insert(b);
        atoms.insert(c);
        atoms.insert(d);
        atoms.insert(e);
        atoms.insert(f);
        atoms.insert(g);
        atoms.insert(h);

        CCF<IRI> ccf(atoms);
        CCF<IRI>::Coalitions coalitions;

        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(a);
            constraint.insert(e);
            constraint.insert(h);
            ccf.addPositiveConstraint(constraint);
        }
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(b);
            constraint.insert(e);
            constraint.insert(g);
            ccf.addPositiveConstraint(constraint);
        }
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(e);
            constraint.insert(g);
            constraint.insert(h);
            ccf.addPositiveConstraint(constraint);
        }
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(a);
            constraint.insert(b);
            constraint.insert(c);
            ccf.addNegativeConstraint(constraint);
        }
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(b);
            constraint.insert(c);
            constraint.insert(e);
            ccf.addNegativeConstraint(constraint);
        }

        IRIList aStar = ccf.computeConstrainedCoalitions(coalitions);

        BOOST_TEST_MESSAGE("Atoms: " << atoms.toString());
        BOOST_TEST_MESSAGE("Positive constraints: " << ccf.getPositiveConstraints().toString() );
        BOOST_TEST_MESSAGE("Negative constraints: " << ccf.getNegativeConstraints().toString() );
        BOOST_TEST_MESSAGE("Coalitions: " << coalitions.toString());
        BOOST_TEST_MESSAGE("AStar: " << aStar);

        CCF<IRI>::CoalitionsList list = ccf.createLists(aStar, coalitions);
        BOOST_FOREACH(CCF<IRI>::Coalitions c, list)
        {
            BOOST_TEST_MESSAGE("Coalitions: " << c.toString());
        }
    }
}

BOOST_AUTO_TEST_CASE(compute_coalitions)
{
    {
        CCF<IRI>::Atoms atoms;

        // Links
        IRI a("1-2"); 
        IRI b("1-3"); 
        IRI c("1-4"); 

        atoms.insert(a);
        atoms.insert(b);
        atoms.insert(c);

        CCF<IRI> ccf(atoms);

        BOOST_FOREACH(CCF<IRI>::Atom atom, atoms)
        {
            ccf.addPositiveConstraint( CCF<IRI>::Constraint(atom));
        }

        // Negative
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(a);
            constraint.insert(b);
            ccf.addNegativeConstraint(constraint);
        }

        CCF<IRI>::Coalitions coalitions;
        IRIList aStar = ccf.computeConstrainedCoalitions(coalitions);
        BOOST_TEST_MESSAGE("Atoms: " << atoms.toString());
        BOOST_TEST_MESSAGE("Positive constraints: " << ccf.getPositiveConstraints().toString() );
        BOOST_TEST_MESSAGE("Negative constraints: " << ccf.getNegativeConstraints().toString() );
        BOOST_TEST_MESSAGE("Coalitions: " << coalitions.toString());
        BOOST_TEST_MESSAGE("AStar: " << aStar);

        CCF<IRI>::CoalitionsList list = ccf.createLists(aStar, coalitions);
        BOOST_FOREACH(CCF<IRI>::Coalitions c, list)
        {
            BOOST_TEST_MESSAGE("Coalitions: " << c.toString());
        }

        std::vector< CCF<IRI>::Coalitions > feasibleCoalitionStructure;
        ccf.computeFeasibleCoalitions(list, feasibleCoalitionStructure);
        BOOST_FOREACH(CCF<IRI>::Coalitions structure, feasibleCoalitionStructure)
        {
            BOOST_TEST_MESSAGE("Feasible coalition structure: " << structure.toString());
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
