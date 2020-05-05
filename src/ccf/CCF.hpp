#ifndef ORGANIZATION_MODEL_CCF_CCF_HPP
#define ORGANIZATION_MODEL_CCF_CCF_HPP

#include <set>
#include <vector>
#include <algorithm>
#include <sstream>
#include <stdint.h>
#include <base-logging/Logging.hpp>

/**
 * This is an implementation of the Constrained Coalition Formation algorithm
 * as suggested by Rahwan et. al "Constrained Coalition Formation",2011
 */

namespace moreorg
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

    bool isSupersetOf(const Set<T>& other) const
    {
        // this set can only be a superset of other
        // if it has more items
        if( other.mSet.size() >= mSet.size())
        {
            return false;
        }

        for(const T& item : other)
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
        for(const T& item : s)
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

    bool insert(const T& item)
    {
        std::pair< typename std::set<T>::iterator, bool> it = mSet.insert(item);
        return it.second;
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
        for(const T& item : mSet)
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
        this->insert( initial );
        assert( Set< Set<T> >::size() == 1 );
    }

    bool containsEmptySet() const
    {
        if(Set< Set<T> >::mSet.empty())
        {
            return false;
        }

        for(const Set<T>& s : Set< Set<T> >::mSet)
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
        for(const Set<T> item : other)
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
        for(const Set<T>& s : Set< Set<T> >::mSet)
        {
            for(const T& t : s)
            {
                newSet.insert(t);
            }
        }
        return newSet;
    }

    bool includes(const SetOfSets<T>& other) const
    {
        bool includes = false;
        for(const Set<T>& thisSubset : Set< Set<T> >::mSet)
        {
            for(const Set<T>& otherSubset : other)
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
        for(const Set<T>& s : Set< Set<T> >::mSet)
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
    typedef T Atom;
    typedef Set<T> Constraint;
    typedef SetOfSets<T> Constraints;

    typedef Set< Coalition<T> > Coalitions;

    Constraints positive;
    Constraints negative;

    Coalition()
    {}

    Coalition(Constraints positive, Constraints negative)
        : positive(positive)
        , negative(negative)
    {}

    std::string toString(bool positiveOnly = false) const
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
        for(const Set<T>& c : constraints)
        {
            if(positive.includes(c))
            {
                LOG_DEBUG_S << "Positive " << positive.toString() << " includes " << c.toString();
                return true;
            }
        }

        return false;
    }

    Set< Coalition<T> > getFeasibleCoalitions() const
    {
        Set< Coalition<T> > coalitions;
        for(const Constraint& p : positive)
        {
            coalitions.insert( Coalition(p, Constraints()) );
        }

        return coalitions;
    }

    static Set<T> getUniqueElements(Coalitions& coalitions)
    {
        Set<T> atoms;
        for(const Coalition& coalition : coalitions)
        {
            for(const Constraint& constraint : coalition.positive)
            {
                for(const Atom& a : constraint)
                {
                    atoms.insert(a);
                }
            }
        }

        return atoms;
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
    typedef std::vector<T> AtomsVector;
    typedef Set<T> Constraint;
    typedef SetOfSets<T> Constraints;
    typedef C<T> Coalition;
    typedef Set< C<T> > Coalitions;
    typedef std::vector<Coalitions> CoalitionsList;
    typedef Constraints PositiveConstraints;
    typedef Constraints NegativeConstraints;
    typedef std::vector<T> AStar;

private:
    Atoms mAtoms;
    Constraints mPositiveConstraints;
    Constraints mNegativeConstraints;
    uint32_t mMaximumCoalitionSize;

public:

    CCF(const AtomsVector& atoms)
    {
        for(const Atom& atom : atoms)
        {
            mAtoms.insert(atom);
        }

        mMaximumCoalitionSize = mAtoms.size();
    }

    CCF(const Atoms& atoms)
        : mAtoms(atoms)
    {
        mMaximumCoalitionSize = mAtoms.size();
    }

    void setMaximumCoalitionSize(uint32_t size) { mMaximumCoalitionSize = size; }

    bool addNegativeConstraint(const Constraint& c) { return mNegativeConstraints.insert(c); }
    bool addPositiveConstraint(const Constraint& c) { return mPositiveConstraints.insert(c); }

    Constraints getNegativeConstraints() const { return mNegativeConstraints; }
    Constraints getPositiveConstraints() const { return mPositiveConstraints; }

    Atom selectAtom(const Atoms& atoms, const Constraints& constraints) const
    {
        // find biggest in list and pick first item
        Constraint largestConstraint;
        size_t largestConstraintSize = 0;
        for(const Constraint& c : constraints)
        {
            size_t constraintSize = c.size();
            if(constraintSize > largestConstraintSize)
            {
                largestConstraintSize = constraintSize;
                largestConstraint = c;
            }
        }

        // Make sure we are using only constraints that are relevant
        for(Atom a : largestConstraint)
        {
            if(atoms.contains(a))
            {
                return a;
            }
        }

        if(!atoms.empty())
                return atoms.first();

        throw std::runtime_error("No atoms left");
    }

    AStar computeConstrainedCoalitions(Coalitions& coalitions)
    {
        LOG_DEBUG_S << "Init compute constrained coalitions: " << std::endl
            << "    atoms:        " << mAtoms.toString() << std::endl
            << "    p:            " << mPositiveConstraints.toString() << std::endl
            << "    n:            " << mNegativeConstraints.toString() << std::endl;

        AStar aStar;
        Constraints constraints;
        computeConstrainedCoalitions(mAtoms, mPositiveConstraints, mNegativeConstraints, constraints, constraints, coalitions, aStar);

        LOG_DEBUG_S << "End of algorithm: Coalitions: " << coalitions.toString();
        return aStar;
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
        // i.e. find the 'smallest' constrained by removing
        // all superset of an existing constraint
        {
            NegativeConstraints tmpN = n;

            for(Constraint nc : tmpN)
            {
                for(Constraint nc_s : tmpN)
                {
                    if(nc_s.isSupersetOf(nc))
                    {
                        n = n.without(nc_s);
                    }
                }
            }
        }

        {
            PositiveConstraints tmpP = p;

            for(Constraint pc : tmpP)
            {
                for(Constraint pc_s : tmpP)
                {
                    if(pc_s.isSupersetOf(pc))
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
        // "Only one constraint, either positive or negative is left to be satisfied", i.e.
        // if there is a constraint in N with exactly one agent
        {
            NegativeConstraints tmpSet = n;
            for(Constraint nc : tmpSet)
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

        // By definition if p contains the empty set, all constraints in P are satisfied
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
        // By definition if p contains the empty set, all constraints in P are satisfied
        // By definition if n contains the empty set, all constraints in n are satisfied
        // Thus we can terminate and add the found coalition
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

        // By definition if P is an empty set and N containts an empty Set the constraints cannot be satisfied
        if( p.empty() || n.containsEmptySet())
        {
            // Termination because the membership constraints cannot be satisfied
            LOG_DEBUG_S << "Termination: p is empty or n contains empty set: p " << p.toString() << ", n " << n.toString();
            return;
        }

        // if ... size constraints. Not important for us right now
        if( pStar.flatten().size() > mMaximumCoalitionSize)
        {
            LOG_DEBUG_S << "Termination: maximum coalition size exceeded: " << mMaximumCoalitionSize;
            return;
        }

        if( mAtoms.empty())
        {
            LOG_DEBUG_S << "Termination: no atoms left";
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
            // Record this for the latter generation of coalitions
            aStar.push_back(atom);
        }

        // Initialize negative constraint sets
        // All negative constraints that do not involve ai
        NegativeConstraints ncs_not_ai; // N Not ai
        // All coalitions that do not involve ai, but when joined with
        // ai are part of the prohibited set
        NegativeConstraints ncs_ai; //N Tilde ai

        // Here we filter out the selected atom and create
        // two sets:
        // ncs_ai -> finally contains all subset when substracting 'atom'
        // ncs_not_ai -> finally all subsets that never related to atom
        for(Constraint nc : n)
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
        // Set of allowed coalitions that do not contain ai
        PositiveConstraints pcs_not_ai;
        // Set of all coalitions that do not contain ai, but lead
        // to permitted coalitions, when merged with ai
        PositiveConstraints pcs_ai;

        for(Constraint pc : p)
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
        // Positive constraints: pcs_not_ai and pcs_ai -> all coalitions that are allowed joined with those that lead to permitted combinations with ai
        // Negative constraints: ncs_not_ai and ncs_ai -> all coalitions that are not allowed joined with those that lead to prohibited combinations with ai
        computeConstrainedCoalitions( remainingAtoms , pcs_not_ai.createUnion(pcs_ai), ncs_not_ai.createUnion(ncs_ai), newPStar, nStar, coalitions, aStar, true, atom);
        computeConstrainedCoalitions( remainingAtoms, pcs_not_ai, ncs_not_ai, pStar, nStar.createUnion(atom), coalitions, aStar, false, atom);
    }

    /**
     * Based on the information on aStar, i.e. the structure of the base cases, we create the necessary set of coalitions
     */
    CoalitionsList createLists(AStar aStar, Coalitions coalitions)
    {
        LOG_DEBUG_S << "Create " << aStar.size() << " lists";
        CoalitionsList list(aStar.size() + 1);
        size_t extraListPosition = aStar.size();

        // Iterate through base cases
        for(const Coalition& coalition : coalitions)
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
                list[extraListPosition].insert(coalition);
            }
        }

        return list;
    }

    bool checkFeasibility(Coalition c)
    {
        return true;
    }

    void computeFeasibleCoalitions(const CoalitionsList& list, std::vector<Coalitions>& coalitions, const Coalitions& baseCoalition = Coalition(), size_t level = 0)
    {
        // Pick coalition
        Coalitions coalitionStructure = baseCoalition;
        for(const Coalition& listCoalition : list[level])
        {
            Coalitions feasibleCoalitions = listCoalition.getFeasibleCoalitions();
            for(Coalition c : feasibleCoalitions)
            {
                coalitionStructure.insert(c);

                // if(checkFeasibility(coalitionStructure))
                if(Coalition::getUniqueElements(coalitionStructure).size() == mAtoms.size())
                {
                    LOG_DEBUG_S << "Check all atoms used in coalition structure, i.e. structure complete: " << coalitionStructure.toString() << " unique elements: " << Coalition::getUniqueElements(coalitionStructure).size() << " atoms: " << mAtoms.size();
                    coalitions.push_back(coalitionStructure);
                    // eval and update
                } else {
                    if(level < list.size() - 1)
                    {
                        LOG_DEBUG_S << "Use list: " << level;
                        level = level + 1;
                        Coalitions nextLevelCoalitions = list[level];

                        for(Coalition nextLevelCoalition : nextLevelCoalitions)
                        {
                            for(const Coalition& coalition : coalitionStructure)
                            {
                                for(const Constraint& constraint : coalition.positive)
                                {
                                    for(const Atom& atom : constraint)
                                    {
                                        nextLevelCoalition.negative.insert( Constraint(atom) );
                                    }
                                }
                            }
                        }

                        LOG_DEBUG_S << "Check feasibility: on level: " << level << " with coalition struct: " << coalitionStructure.toString();
                        computeFeasibleCoalitions(list, coalitions, coalitionStructure, level);
                    } else {
                        LOG_DEBUG_S << "Level " << level << " vs. " << list.size();
                    }
                }
            }
        }
    }

};

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_CCF_CCF_HPP
