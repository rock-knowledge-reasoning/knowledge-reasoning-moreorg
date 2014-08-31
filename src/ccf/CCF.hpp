#ifndef OWL_OM_CCF_CCF_HPP
#define OWL_OM_CCF_CCF_HPP

#include <set>
#include <vector>
#include <algorithm>
#include <sstream>
#include <boost/foreach.hpp>
#include <base/Logging.hpp>

namespace owl_om
{

template<typename T>
class Set
{
protected:
    std::set<T> mSet;

public:
    typedef typename std::set<T>::const_iterator const_iterator;
    typedef typename std::set<T>::iterator iterator;

    Set()
    {}

    Set(const T& initial)
    {
        insert(initial);
    }

    bool supersetOf(const Set<T>& other) const
    {
        // this set can only be a superset of other
        // if it has more items
        if( other.mSet.size() >= mSet.size())
        {
            return false;
        }

        BOOST_FOREACH(const T& item, other)
        {
            if(!mSet.count(item) )
            {
                return false;
            }
        }
        return true;
    }

    bool includes(const Set<T>& other) const
    {
        return std::includes(this->begin(), this->end(), other.begin(), other.end());
    }

    Set<T> createUnion(const Set<T>& s) const
    {
        Set<T> tmpSet = *this;
        BOOST_FOREACH(const T& item, s)
        {
            tmpSet.insert(item);
        }
        return tmpSet;
    }

    Set<T> without(const T& item) const
    {
        Set<T> tmpSet = *this;
        tmpSet.mSet.erase(item);
        return tmpSet;
    }

    const_iterator begin() const { return mSet.begin(); }
    const_iterator end() const { return mSet.end(); }

    iterator begin() { return mSet.begin(); }
    iterator end() { return mSet.end(); }

    bool empty() const { return mSet.empty(); }
    size_t size() const { return mSet.size(); }

    void insert(const T& item)
    {
        mSet.insert(item);
    }

    const T& first() const { assert(!empty()); return *mSet.begin(); }
    void clear() { mSet.clear(); }

    bool operator<( const Set<T>& other) const
    {
        return mSet < other.mSet;
    }

    bool contains(const T& item) const { return mSet.count(item); }

    std::string toString() const
    {
        std::stringstream ss;
        ss << "{";
        BOOST_FOREACH(const T& item, mSet)
        {
            ss << item;
            ss << ",";
        }
        std::string s = ss.str();
        if(!mSet.empty())
        {
            s = s.substr(0, s.size()-1);
        }
        return s + "}";
    }
};

template<typename T>
class SetOfSets : public Set< Set<T> >
{

public:
    SetOfSets() {}

    SetOfSets(const Set<T>& initial)
    {
        insert( initial );
        assert( Set< Set<T> >::size() == 1 );
    }

    bool containsEmptySet() const
    {
        if(Set< Set<T> >::mSet.empty())
        {
            return false;
        }

        BOOST_FOREACH(const Set<T>& s, Set< Set<T> >::mSet)
        {
            if(s.empty())
            {
                return true;
            }
        }

        return false;
    }

    SetOfSets<T> without(const Set<T>& item) const
    {
        SetOfSets<T> tmpSet = *this;
        tmpSet.mSet.erase(item);
        return tmpSet;
    }

    SetOfSets<T> createUnion(const SetOfSets<T>& other) const
    {
        SetOfSets<T> tmpSet = *this;
        BOOST_FOREACH(const Set<T> item, other)
        {
            tmpSet.insert(item);
        }
        return tmpSet;
    }

    SetOfSets<T> createUnion(const Set<T>& other) const
    {
        SetOfSets<T> tmpSet = *this;
        tmpSet.insert(other);
        return tmpSet;
    }

    Set<T> flatten() const
    {
        Set<T> newSet;
        BOOST_FOREACH(const Set<T>& s, Set< Set<T> >::mSet)
        {
            BOOST_FOREACH(const T& t, s)
            {
                newSet.insert(t);
            }
        }
        return newSet;
    }

    bool includes(const SetOfSets<T>& other) const
    {
        bool includes = false;
        BOOST_FOREACH(const Set<T>& thisSubset, Set< Set<T> >::mSet)
        {
            BOOST_FOREACH(const Set<T>& otherSubset, other)
            {
                if(thisSubset.includes(otherSubset))
                {
                    includes = true;
                    break;
                }
            }
            if(!includes)
            {
                return false;
            }
        }
        return includes;
    }

    std::string toString() const
    {
        std::stringstream ss;
        ss << "{";
        BOOST_FOREACH(const Set<T>& s, Set< Set<T> >::mSet)
        {
            ss << s.toString();
            ss << ",";
        }

        std::string s = ss.str();
        if(!Set< Set<T> >::mSet.empty())
        {
            s = s.substr(0, s.size()-1);
        }
        return s + "}";
    }
};


template<typename T>
struct Coalition
{
    typedef SetOfSets<T> Constraints;

    Constraints positive;
    Constraints negative;

    Coalition()
    {}

    Coalition(Constraints positive, Constraints negative)
        : positive(positive)
        , negative(negative)
    {}

    std::string toString(bool positiveOnly = true) const
    {
        std::stringstream ss;
        ss << "(p:";
        ss << positive.toString();
        if(!positiveOnly)
        {
            ss << "; n:";
            ss << negative.toString();
        }
        ss << ")";
        return ss.str();
    }

    bool operator<(const Coalition& other) const
    {
        if(positive < other.positive)
        {
            return true;
        } else if(negative < other.negative)
        {
            return true;
        }
        return false;
    }

    bool constraintsApply(const Constraints& constraints) const
    {
        BOOST_FOREACH(const Set<T>& c, constraints)
        {
            if(positive.includes(c))
            {
                LOG_DEBUG_S << "Positive " << positive.toString() << " includes " << c.toString();
                return true;
            }
        }

        return false;
    }
};

template<class T>
inline std::ostream& operator<<(std::ostream& out, const Coalition<T>& val)
{
    out << val.toString();
    return out;
}


/**
 *    NegativeConstraint == atomSet == Coalition
 *    PositiveConstraint == atomSet == Coalition
 *
 *    p -> Set of Sets
 *    pStar -> Set of sets
 */
template<typename T, template<class> class C = Coalition>
class CCF
{
public:
    typedef T Atom;
    typedef Set<T> Atoms;
    typedef Set<T> Constraint;
    typedef SetOfSets<T> Constraints;
    typedef C<T> Coalition;
    typedef Set< C<T> > Coalitions;
    typedef std::vector<Coalitions> CoalitionsList;
    typedef Constraints PositiveConstraints;
    typedef Constraints NegativeConstraints;
    typedef std::vector<T> AStar;

    Atom selectAtom(const Atoms& atoms, const Constraints& constraints) const
    {
        // find biggest in list and pick first item
        Constraint largestConstraint;
        size_t largestConstraintSize = 0;
        BOOST_FOREACH(const Constraint& c, constraints)
        {
            size_t constraintSize = c.size();
            if(constraintSize > largestConstraintSize)
            {
                largestConstraintSize = constraintSize;
                largestConstraint = c;
            }
        }

        // Make sure we are using only constraints that are relevant
        BOOST_FOREACH(Atom a, largestConstraint)
        {
            if(atoms.contains(a))
            {
                return a;
            }
        }

        throw std::runtime_error("No atoms left");
    }

    void computeConstrainedCoalitions(Atoms atoms, PositiveConstraints p, NegativeConstraints n, Coalitions& coalitions, AStar& aStar)
    {
        LOG_DEBUG_S << "Init compute constrained coalitions: " << std::endl
            << "    atoms:        " << atoms.toString() << std::endl
            << "    p:            " << p.toString() << std::endl
            << "    n:            " << n.toString() << std::endl;

        Constraints constraints;
        computeConstrainedCoalitions(atoms, p, n, constraints, constraints, coalitions, aStar);

        LOG_DEBUG_S << "End of algorithm: Coalitions: " << coalitions.toString();
    }

    void computeConstrainedCoalitions(Atoms atoms, PositiveConstraints p, NegativeConstraints n,
            PositiveConstraints pStar, NegativeConstraints nStar, Coalitions& coalitions, AStar& aStar, bool positiveBranch = true, Atom a = Atom())
    {
        std::string label;
        if(positiveBranch)
        {
            label = "with ";
        } else {
            label = "without ";
        }
        label += a.toString();

        LOG_DEBUG_S << "compute constrained coalitions [" << label << "]: " << std::endl
            << "    atoms:        " << atoms.toString() << std::endl
            << "    p:            " << p.toString() << std::endl
            << "    n:            " << n.toString() << std::endl
            << "    p*:           " << pStar.toString() << std::endl
            << "    n*:           " << nStar.toString() << std::endl
            << "    coalitions:   " << coalitions.toString() << std::endl;

        // Remove redundant constraints
        {
            NegativeConstraints tmpN = n;

            BOOST_FOREACH(Constraint nc, tmpN)
            {
                BOOST_FOREACH(Constraint nc_s, tmpN)
                {
                    if(nc_s.supersetOf(nc))
                    {
                        n = n.without(nc_s);
                    }
                }
            }
        }

        {
            PositiveConstraints tmpP = p;

            BOOST_FOREACH(Constraint pc, tmpP)
            {
                BOOST_FOREACH(Constraint pc_s, tmpP)
                {
                    if(pc_s.supersetOf(pc))
                    {
                        p = p.without(pc_s);
                    }
                }
            }
        }

        LOG_DEBUG_S << "Removed redunant: " << std::endl
            << "    atoms:        " << atoms.toString() << std::endl
            << "    p:            " << p.toString() << std::endl
            << "    n:            " << n.toString() << std::endl
            << "    p*:           " << pStar.toString() << std::endl
            << "    n*:           " << nStar.toString() << std::endl
            << "    coalitions:   " << coalitions.toString() << std::endl;


        // Dealing with special cases
        {
            NegativeConstraints tmpSet = n;
            BOOST_FOREACH(Constraint nc, tmpSet)
            {
                if(nc.size() == 1)
                {
                    LOG_DEBUG_S << "Constraint with exactly one agent: " << nc.toString();
                    atoms = atoms.without(nc.first());
                    nStar = nStar.createUnion(nc);
                    n = n.without(nc);
                }
            }
        }

        if(p.containsEmptySet() && n.size() == 1)
        {
            LOG_DEBUG_S << "P contains empty set and one negative constraint only" << p.toString();
            nStar = nStar.createUnion(n);
            n.clear();
        }

        // one coalition remaining and no negative contraints
        if(p.size() == 1 && n.empty())
        {
            // Set the resulting coalition by combining all known positive constraints
            pStar = PositiveConstraints( p.createUnion(pStar).flatten() );

            // reset positive constraint set
            p.clear();
            p = p.createUnion( Constraint() );
            LOG_DEBUG_S << "Create p union " << p.toString();
        }

        LOG_DEBUG_S << "Processed special cases: " << std::endl
            << "    atoms:        " << atoms.toString() << std::endl
            << "    p:            " << p.toString() << std::endl
            << "    n:            " << n.toString() << std::endl
            << "    p*:           " << pStar.toString() << std::endl
            << "    n*:           " << nStar.toString() << std::endl
            << "    coalitions:   " << coalitions.toString() << std::endl;

        // Check termination criteria
        if( p.containsEmptySet() && n.empty())
        {
            Coalition coalition(pStar, nStar);
            LOG_DEBUG_S << "Termination: p contains empty set and n is empty";
            LOG_DEBUG_S << "Termination: existing coalition " << coalitions.toString();
            LOG_DEBUG_S << "Termination: adding coalition " << coalition.toString();
            coalitions.insert(coalition);
            LOG_DEBUG_S << "Termination: resulting coalitions " << coalitions.toString();
            LOG_DEBUG_S << "Termination: negative constraints " << nStar.toString();
            // all membership contraints satisfied
            return;
        }
        if( p.empty() || n.containsEmptySet())
        {
            LOG_DEBUG_S << "Termination: p is empty or n contains empty set: p " << p.toString() << ", n " << n.toString();
            return;
        }

        // if ... size constraints. Not important for us
        if( atoms.empty())
        {
            return;
        }



        // Initialize divide and conquer
        // select an atom

        Atom atom;
        try {
            if(positiveBranch)
            {
                atom = selectAtom(atoms, p);
            } else {
                atom = selectAtom(atoms, n);
            }
        } catch(const std::runtime_error& e)
        {
            LOG_DEBUG_S << e.what();
            return;
        }

        if( pStar.empty())
        {
            aStar.push_back(atom);
        }

        NegativeConstraints ncs_not_ai; // N Not ai
        NegativeConstraints ncs_ai; //N Tilde ai

        // Here we filter out the selected atom and create 
        // two sets: 
        // ncs_ai -> finally contains all subset when substracting 'atom'
        // ncs_not_ai -> finally all subsets that never related to atom
        BOOST_FOREACH(Constraint nc, n)
        {
            if( nc.contains(atom) )
            { 
                Constraint constraint = nc.without(atom);
                //if(!constraint.empty())
                //{
                    ncs_ai = ncs_ai.createUnion(constraint);
                //}
            } else {
                ncs_not_ai = ncs_not_ai.createUnion( nc );
            }
        }

        // Respectively for the set of positive constraints
        PositiveConstraints pcs_not_ai;
        PositiveConstraints pcs_ai;

        BOOST_FOREACH(Constraint pc, p)
        {
            if( pc.contains(atom) )
            {
                Constraint constraint = pc.without(atom);
                //if(!constraint.empty())
                //{
                    pcs_ai = pcs_ai.createUnion(constraint);
                //}
            } else {
                pcs_not_ai = pcs_not_ai.createUnion(pc);
            }
        }

        PositiveConstraints newPStar = PositiveConstraints( pStar.createUnion( PositiveConstraints( atom ) ).flatten() );

        LOG_DEBUG_S << "Prepare divide and conquer for: " << atom << std::endl
            << "    atoms:        " << atoms.toString() << std::endl
            << "    p:            " << p.toString() << std::endl
            << "    n:            " << n.toString() << std::endl
            << "    p*:           " << pStar.toString() << std::endl
            << "    n*:           " << nStar.toString() << std::endl
            << "    pcs_ai~:      " << pcs_ai.toString() << std::endl
            << "    pcs_not_ai:   " << pcs_not_ai.toString() << std::endl
            << "    ncs_ai~:      " << ncs_ai.toString() << std::endl
            << "    ncs_not_ai:   " << ncs_not_ai.toString() << std::endl
            << "    coalitions:   " << coalitions.toString() << std::endl;


        // apply divide and conquer
        Atoms remainingAtoms = atoms.without(atom);
        computeConstrainedCoalitions( remainingAtoms , pcs_not_ai.createUnion(pcs_ai), ncs_not_ai.createUnion(ncs_ai), newPStar, nStar, coalitions, aStar, true, atom);
        computeConstrainedCoalitions( remainingAtoms, pcs_not_ai, ncs_not_ai, pStar, nStar.createUnion(atom), coalitions, aStar, false, atom);
    }

    CoalitionsList createLists(AStar aStar, Coalitions coalitions)
    {
        LOG_DEBUG_S << "Create " << aStar.size() << " lists";
        CoalitionsList list(aStar.size());

        // Iterate through base cases
        BOOST_FOREACH(const Coalition& coalition, coalitions)
        {
            bool foundList = false;
            // Picking a feasible coaltion -- in our case they should be feasible by default
            for(size_t i = 0; i < aStar.size(); ++i)
            {
                Atom a = aStar[i];
                if( coalition.positive.first().contains(a) )
                {
                    list[i].insert(coalition);

                    foundList = true;
                    break;
                } else {
                    LOG_DEBUG_S << a << " not in " << coalition.positive.toString();
                }
            }

            if(!foundList)
            {
                LOG_DEBUG_S << "Extra list for: " << coalition;
                size_t extraListPosition = list.size() + 1;
                list.resize(extraListPosition);
                list[extraListPosition - 1].insert(coalition);
            }
        }

        return list;
    }

    bool checkFeasibility(Coalition c)
    {
        return true;
    }

    void feasibleCoalitions(const CoalitionsList& list, Coalitions& coalitions, const Coalition& baseCoalition = Coalition(), size_t level = 0)
    {
        //if(level >= list.size())
        //{
        //    return;
        //} else if(level > 0)
        //{
        //    coalitions.insert(baseCoalition);
        //}

        //Coalition coalitionStructure = baseCoalition;
        //// At level 0 with require exactly one coalition
        //BOOST_FOREACH(const Coalition& c, list[level])
        //{
        //    LOG_DEBUG_S << "Check to merge " << baseCoalition << "with " << c;
        //    // if c feasible && level = 0
        //    // return
        //    if(! c.constraintsApply( coalitionStructure.negative ) && ! coalitionStructure.constraintsApply(c.negative) )
        //    {
        //        coalitionStructure.positive = coalitionStructure.positive.createUnion(c.positive);
        //        coalitionStructure.negative = coalitionStructure.negative.createUnion(c.negative);
        //        LOG_DEBUG_S << "success";

        //    } else {
        //        coalitionStructure.negative = coalitionStructure.negative.createUnion(c.positive);
        //        LOG_DEBUG_S << "fail";
        //    }

        //    feasibleCoalitions(list, coalitions, coalitionStructure, level + 1);
        //}

        // Pick coalition
        BOOST_FOREACH(const Coalition& c, list[level])
        {
            Coalition coalitionStructure = baseCoalition;
            if(!c.constraintsApply(c.negative))
            {
                // not in algo
                //coalitions.insert(c)

                // if(checkFeasibility(coalitionStructure))
                LOG_DEBUG_S << "Check feasibility: " << coalitionStructure.toString() << " vs. " << c.toString();
                if(!coalitionStructure.constraintsApply(c.negative) && !c.constraintsApply(coalitionStructure.negative))
                {
                    LOG_DEBUG_S << "Check feasibility: is feasible";
                    coalitions.insert(c);
                    // eval and update
                } else {
                    if(level - 1 < list.size())
                    {
                        level = level + 1;
                        Coalitions nextLevelCoalitions = list[level];

                        BOOST_FOREACH(Coalition nextLevelCoalition, nextLevelCoalitions)
                        {
                            BOOST_FOREACH(const Constraint& atom, coalitionStructure.positive)
                            {
                                nextLevelCoalition.negative.insert( Constraint(atom) );
                            }
                        }

                        LOG_DEBUG_S << "Check feasibility: on level: " << level;
                        feasibleCoalitions(list, coalitions, coalitionStructure, level);
                    }
                }
            }
        }
    }
};

} // end namespace owl_om
#endif // OWL_OM_CCF_CCF_HPP
