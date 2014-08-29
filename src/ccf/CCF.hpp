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

    Set<T> createUnion(const Set<T>& s) const
    {
        Set<T> tmpSet = *this;
        tmpSet.mSet.insert(tmpSet.begin(), s.begin(), s.end());
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

    void insert(const T& item) { mSet.insert(item); }

    T first() { return *mSet.begin(); }
    void clear() { mSet.clear(); }

    bool operator<( const Set<T>& other) const
    {
        return mSet < other.mSet;
    }

    bool contains(const T& item) { return mSet.count(item); }

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


/**   
 *    NegativeConstraint == atomSet == Coalition
 *    PositiveConstraint == atomSet == Coalition
 *
 *    p -> Set of Sets
 *    pStar -> Set of sets
 */
template<typename T>
class CCF
{
public:
    typedef T Atom;
    typedef Set<T> Atoms;
    typedef Set<T> Constraint;
    typedef SetOfSets<T> Constraints;
    typedef SetOfSets<T> Coalitions;
    typedef Constraints PositiveConstraints;
    typedef Constraints NegativeConstraints;

    void computeConstrainedCoalitions(Atoms atoms, PositiveConstraints p, NegativeConstraints n, Coalitions& coalitions)
    {
        Constraints constraints;
        computeConstrainedCoalitions(atoms, p, n, constraints, constraints, coalitions);
    }

    void computeConstrainedCoalitions(Atoms atoms, PositiveConstraints p, NegativeConstraints n, 
            PositiveConstraints pStar, NegativeConstraints nStar, Coalitions& coalitions)
    {

        LOG_DEBUG_S << "Compute constrained coalitions: " << std::endl
            << "    atoms:        " << atoms.toString() << std::endl
            << "    n:            " << n.toString() << std::endl
            << "    p:            " << p.toString() << std::endl
            << "    n*:           " << nStar.toString() << std::endl
            << "    p*:           " << pStar.toString() << std::endl
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
            << "    n:            " << n.toString() << std::endl
            << "    p:            " << p.toString() << std::endl
            << "    n*:           " << nStar.toString() << std::endl
            << "    p*:           " << pStar.toString() << std::endl
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
            LOG_DEBUG_S << "P constains empty set and one negative constraint only" << p.toString();
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
            << "    n:            " << n.toString() << std::endl
            << "    p:            " << p.toString() << std::endl
            << "    n*:           " << nStar.toString() << std::endl
            << "    p*:           " << pStar.toString() << std::endl
            << "    coalitions:   " << coalitions.toString() << std::endl;

        // Check termination criteria
        if( p.containsEmptySet() && n.empty())
        {
            LOG_DEBUG_S << "Termination: p contains empty set and n is empty";
            //coalitions.createUnion( Coalition(pStar, nStar) );
            coalitions = coalitions.createUnion( Coalitions(pStar) );
            LOG_DEBUG_S << "Termination: coalition " << coalitions.toString();
            LOG_DEBUG_S << "Termination: contraints " << nStar.toString();
            // all membership contraints satisfied
            return; 
        }
        if( p.empty() || n.containsEmptySet())
        {
            LOG_DEBUG_S << "Termination: p is empty or n contains empty set: p " << p.toString() << ", n " << n.toString();
            return;
        }

        // if ... size constraints. Not important for us



        // Initialize divide and conquer
        // select an atom
        Atom atom = atoms.first();
        if( pStar.empty())
        {
            // !!??? really
            pStar.createUnion( Constraint(atom) );
        }

        NegativeConstraints ncs_not_ai;
        NegativeConstraints ncs_ai;

        BOOST_FOREACH(Constraint nc, n)
        {
            if( nc.contains(atom) )
            {
                ncs_ai = ncs_ai.createUnion( nc.without(atom) );
            } else {
                ncs_not_ai = ncs_not_ai.createUnion( nc );
            }
        }

        PositiveConstraints pcs_not_ai;
        PositiveConstraints pcs_ai;

        BOOST_FOREACH(Constraint pc, p)
        {
            if( pc.contains(atom) )
            {
                pcs_ai = pcs_ai.createUnion( pc.without(atom) );
            } else {
                pcs_not_ai = pcs_not_ai.createUnion(pc);
            }
        }

        PositiveConstraints newPStar = PositiveConstraints( pStar.createUnion( PositiveConstraints( atom ) ).flatten() );

        LOG_DEBUG_S << "Prepare divide and conquer: " << std::endl
            << "    atoms:        " << atoms.toString() << std::endl
            << "    n:            " << n.toString() << std::endl
            << "    p:            " << p.toString() << std::endl
            << "    n*:           " << nStar.toString() << std::endl
            << "    p*:           " << pStar.toString() << std::endl
            << "    coalitions:   " << coalitions.toString() << std::endl;


        // apply divide and conquer
        Atoms remainingAtoms = atoms.without(atom);
        computeConstrainedCoalitions( remainingAtoms , pcs_not_ai.createUnion(pcs_ai), ncs_not_ai.createUnion(ncs_ai), newPStar, nStar, coalitions);
        computeConstrainedCoalitions( remainingAtoms, pcs_not_ai, ncs_not_ai, pStar, nStar.createUnion(atom), coalitions);
    }
};

} // end namespace owl_om
#endif // OWL_OM_CCF_CCF_HPP
