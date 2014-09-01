#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <owl_om/ccf/CCF.hpp>
#include <owl_om/OrganizationModel.hpp>
#include <set>

using namespace owl_om;

BOOST_AUTO_TEST_CASE(it_should_handle_sets)
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


BOOST_AUTO_TEST_CASE(it_should_handle_reference_ccf)
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

        CCF<IRI>::Constraints positiveConstraints;
        CCF<IRI>::Constraints negativeConstraints;
        CCF<IRI>::Coalitions coalitions;

        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(a);
            constraint.insert(e);
            constraint.insert(h);
            positiveConstraints.insert(constraint);
        }
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(b);
            constraint.insert(e);
            constraint.insert(g);
            positiveConstraints.insert(constraint);
        }
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(e);
            constraint.insert(g);
            constraint.insert(h);
            positiveConstraints.insert(constraint);
        }
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(a);
            constraint.insert(b);
            constraint.insert(c);
            negativeConstraints.insert(constraint);
        }
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(b);
            constraint.insert(c);
            constraint.insert(e);
            negativeConstraints.insert(constraint);
        }

        IRIList aStar;
        CCF<IRI> ccf(atoms);
        ccf.computeConstrainedCoalitions(positiveConstraints, negativeConstraints, coalitions, aStar);

        BOOST_TEST_MESSAGE("Atoms: " << atoms.toString());
        BOOST_TEST_MESSAGE("Positive constraints: " << positiveConstraints.toString() );
        BOOST_TEST_MESSAGE("Negative constraints: " << negativeConstraints.toString() );
        BOOST_TEST_MESSAGE("Coalitions: " << coalitions.toString());
        BOOST_TEST_MESSAGE("AStar: " << aStar);

        CCF<IRI>::CoalitionsList list = ccf.createLists(aStar, coalitions);
        BOOST_FOREACH(CCF<IRI>::Coalitions c, list)
        {
            BOOST_TEST_MESSAGE("Coalitions: " << c.toString());
        }
    }
}

BOOST_AUTO_TEST_CASE(it_should_compute_coalitions)
{
//    {
//        CCF<IRI>::Atoms atoms;
//
//        IRI a("1"); 
//        IRI b("2"); 
//        IRI c("3"); 
//        IRI d("4"); 
//        IRI e("5"); 
//        IRI f("6"); 
//        IRI g("7"); 
//        IRI h("8"); 
//
//        atoms.insert(a);
//        atoms.insert(b);
//        atoms.insert(c);
//        atoms.insert(d);
//        atoms.insert(e);
//        atoms.insert(f);
//        atoms.insert(g);
//        atoms.insert(g);
//
//        CCF<IRI>::Constraints positiveConstraints;
//        CCF<IRI>::Constraints negativeConstraints;
//        CCF<IRI>::Coalitions coalitions;
//
//        positiveConstraints = atoms;
//
//        CCF<IRI> ccf(atoms);
//        ccf.computeConstrainedCoalitions(positiveConstraints, negativeConstraints, coalitions);
//        BOOST_TEST_MESSAGE("Atoms: " << atoms.toString());
//        BOOST_TEST_MESSAGE("Positive constraints: " << positiveConstraints.toString() );
//        BOOST_TEST_MESSAGE("Negative constraints: " << negativeConstraints.toString() );
//        BOOST_TEST_MESSAGE("Coalitions: " << coalitions.toString());
//    }
//
//
//    {
//        CCF<IRI>::Atoms atoms;
//
//        IRI a("1"); 
//        IRI b("2"); 
//        IRI c("3"); 
//        IRI d("4"); 
//        IRI e("5"); 
//        IRI f("6"); 
//        IRI g("7"); 
//        IRI h("8"); 
//
//        atoms.insert(a);
//        atoms.insert(b);
//        atoms.insert(c);
//        atoms.insert(d);
//        atoms.insert(e);
//        //atoms.insert(f);
//        //atoms.insert(g);
//        //atoms.insert(g);
//
//        CCF<IRI>::Constraints positiveConstraints;
//        CCF<IRI>::Constraints negativeConstraints;
//        CCF<IRI>::Coalitions coalitions;
//    
//        BOOST_FOREACH(CCF<IRI>::Atom atom, atoms)
//        {
//            CCF<IRI>::Constraint constraint;
//            constraint.insert(atom);
//            positiveConstraints.insert(constraint);
//        }
//        //BOOST_FOREACH(CCF<IRI>::Atom atom, atoms)
//        //{
//            CCF<IRI>::Constraint constraint;
//            constraint.insert(a);
//            constraint.insert(b);
//            negativeConstraints.insert(constraint);
//        //}
//       // {
//       //     CCF<IRI>::Constraint constraint;
//       //     constraint.insert(b);
//       //     constraint.insert(e);
//       //     constraint.insert(g);
//       //     positiveConstraints.insert(constraint);
//       // }
//       // {
//       //     CCF<IRI>::Constraint constraint;
//       //     constraint.insert(e);
//       //     constraint.insert(h);
//       //     constraint.insert(h);
//       //     positiveConstraints.insert(constraint);
//       // }
//       // {
//       //     CCF<IRI>::Constraint constraint;
//       //     constraint.insert(a);
//       //     constraint.insert(b);
//       //     constraint.insert(c);
//       //     negativeConstraints.insert(constraint);
//       // }
//       // {
//       //     CCF<IRI>::Constraint constraint;
//       //     constraint.insert(b);
//       //     constraint.insert(c);
//       //     constraint.insert(e);
//       //     negativeConstraints.insert(constraint);
//       // }
//
//        ccf.computeConstrainedCoalitions(atoms, positiveConstraints, negativeConstraints, coalitions);
//        BOOST_TEST_MESSAGE("Atoms: " << atoms.toString());
//        BOOST_TEST_MESSAGE("Positive constraints: " << positiveConstraints.toString() );
//        BOOST_TEST_MESSAGE("Negative constraints: " << negativeConstraints.toString() );
//        BOOST_TEST_MESSAGE("Coalitions: " << coalitions.toString());
//    }
//    {
//        CCF<IRI>::Atoms atoms;
//
//        IRI a("1"); 
//        IRI b("2"); 
//        IRI c("3"); 
//        IRI d("4"); 
//        IRI e("5"); 
//        IRI f("6"); 
//        IRI g("7"); 
//        IRI h("8"); 
//
//        atoms.insert(a);
//        atoms.insert(b);
//        atoms.insert(c);
//        atoms.insert(d);
//        atoms.insert(e);
//        //atoms.insert(f);
//        //atoms.insert(g);
//        //atoms.insert(g);
//
//        CCF<IRI>::Constraints positiveConstraints;
//        CCF<IRI>::Constraints negativeConstraints;
//        CCF<IRI>::Coalitions coalitions;
//   
//        {
//            CCF<IRI>::Constraint constraint;
//            BOOST_FOREACH(CCF<IRI>::Atom atom, atoms)
//            {
//                constraint.insert(atom);
//            }
//            positiveConstraints.insert(constraint);
//        }
//        //BOOST_FOREACH(CCF<IRI>::Atom atom, atoms)
//        //{
//            CCF<IRI>::Constraint constraint;
//            constraint.insert(a);
//            constraint.insert(b);
//            negativeConstraints.insert(constraint);
//        //}
//       // {
//       //     CCF<IRI>::Constraint constraint;
//       //     constraint.insert(b);
//       //     constraint.insert(e);
//       //     constraint.insert(g);
//       //     positiveConstraints.insert(constraint);
//       // }
//       // {
//       //     CCF<IRI>::Constraint constraint;
//       //     constraint.insert(e);
//       //     constraint.insert(h);
//       //     constraint.insert(h);
//       //     positiveConstraints.insert(constraint);
//       // }
//       // {
//       //     CCF<IRI>::Constraint constraint;
//       //     constraint.insert(a);
//       //     constraint.insert(b);
//       //     constraint.insert(c);
//       //     negativeConstraints.insert(constraint);
//       // }
//       // {
//       //     CCF<IRI>::Constraint constraint;
//       //     constraint.insert(b);
//       //     constraint.insert(c);
//       //     constraint.insert(e);
//       //     negativeConstraints.insert(constraint);
//       // }
//
//        ccf.computeConstrainedCoalitions(atoms, positiveConstraints, negativeConstraints, coalitions);
//        BOOST_TEST_MESSAGE("Atoms: " << atoms.toString());
//        BOOST_TEST_MESSAGE("Positive constraints: " << positiveConstraints.toString() );
//        BOOST_TEST_MESSAGE("Negative constraints: " << negativeConstraints.toString() );
//        BOOST_TEST_MESSAGE("Coalitions: " << coalitions.toString());
//    }
    {
        CCF<IRI>::Atoms atoms;

        // Links
        IRI a("1-2"); 
        IRI b("1-3"); 
        IRI c("1-4"); 
        IRI d("2-3"); 
        IRI e("2-4"); 
        //IRI f("2-5"); 
        //IRI g("3-4"); 

        atoms.insert(a);
        atoms.insert(b);
        //atoms.insert(c);
        atoms.insert(d);
        //atoms.insert(e);
        //atoms.insert(f);
        //atoms.insert(g);
        //atoms.insert(g);

        CCF<IRI>::Constraints positiveConstraints;
        CCF<IRI>::Constraints negativeConstraints;
        BOOST_FOREACH(CCF<IRI>::Atom atom, atoms)
        {
            positiveConstraints.insert(atom);
        }

        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(a);
            constraint.insert(d);
            positiveConstraints.insert(constraint);
        }
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(b);
            constraint.insert(d);
            positiveConstraints.insert(constraint);
        }
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(c);
            constraint.insert(d);
            positiveConstraints.insert(constraint);
        }
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(a);
            constraint.insert(e);
            positiveConstraints.insert(constraint);
        }
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(b);
            constraint.insert(e);
            positiveConstraints.insert(constraint);
        }
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(c);
            constraint.insert(e);
            positiveConstraints.insert(constraint);
        }
        //{
        //    CCF<IRI>::Constraint constraint;
        //    constraint.insert(a);
        //    constraint.insert(g);
        //    positiveConstraints.insert(constraint);
        //}

        // Negative
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(a);
            constraint.insert(b);
            negativeConstraints.insert(constraint);
        }
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(a);
            constraint.insert(c);
            negativeConstraints.insert(constraint);
        }
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(c);
            constraint.insert(b);
            negativeConstraints.insert(constraint);
        }
        {
            CCF<IRI>::Constraint constraint;
            constraint.insert(d);
            constraint.insert(e);
            negativeConstraints.insert(constraint);
        }
        //{
        //    CCF<IRI>::Constraint constraint;
        //    constraint.insert(d);
        //    constraint.insert(f);
        //    negativeConstraints.insert(constraint);
        //}

        IRIList aStar;
        CCF<IRI>::Coalitions coalitions;
        CCF<IRI> ccf(atoms);
        ccf.computeConstrainedCoalitions(positiveConstraints, negativeConstraints, coalitions, aStar);
        BOOST_TEST_MESSAGE("Atoms: " << atoms.toString());
        BOOST_TEST_MESSAGE("Positive constraints: " << positiveConstraints.toString() );
        BOOST_TEST_MESSAGE("Negative constraints: " << negativeConstraints.toString() );
        BOOST_TEST_MESSAGE("Coalitions: " << coalitions.toString());
        BOOST_TEST_MESSAGE("AStar: " << aStar);

        CCF<IRI>::CoalitionsList list = ccf.createLists(aStar, coalitions);
        BOOST_FOREACH(CCF<IRI>::Coalitions c, list)
        {
            BOOST_TEST_MESSAGE("Coalitions: " << c.toString());
        }

        CCF<IRI>::Coalitions feasibleCoalitions;
        ccf.computeFeasibleCoalitions(list, feasibleCoalitions);
        BOOST_TEST_MESSAGE("Feasible coalitions: " << feasibleCoalitions.toString());
    }
}
