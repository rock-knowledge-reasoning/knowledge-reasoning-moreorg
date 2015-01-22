/*
  
  Curious set of integers problem in Gecode.

  Martin Gardner (February 1967):
  """
  The integers 1,3,8, and 120 form a set with a remarkable property:
  the product of any two integers is one less than a perfect square. 
  Find a fifth number that can be added to the set without destroying 
  this property.
  """
  
  Solution: The number is 0.
 
  There are however other sets of five numbers with this property.
  Here are the one below 1000:
    {0, 1, 3, 8, 120}
    {0, 1, 8, 15, 528}
    {0, 2, 4, 12, 420}


  Compare with the following models:
  * MiniZinc: http://www.hakank.org/minizinc/curious_set_of_integers.mzn
  * SICStus Prolog: http://www.hakank.org/sicstus/curious_set_of_integers.pl
  * ECLiPSE: http://www.hakank.org/eclipse/curious_set_of_integers.ecl


  This Gecode model was created by Hakan Kjellerstrand (hakank@gmail.com)
  Also, see my Gecode page: http://www.hakank.org/gecode/ .

*/

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

using std::cout;
using std::endl;
using std::setw;
using std::string;


class CuriousSet : public Script {
protected:

  static const int n = 5;
  static const int max_num = 1000;

  IntVarArray x;

public:

  CuriousSet(const Options& opt) 
    : 
    x(*this, n, 0, max_num)
  {

    distinct(*this, x);

    // increasing(x)
    rel(*this, x, IRT_LQ);

    for(int i = 0; i < n; i++) {
      for(int j = 0; j < i; j++) {
        if (i != j) {
          IntVar p(*this, 0, max_num);
          rel(*this, (p*p)-1 == x[i]*x[j]);
        }
      }
    }
    
    // branching
    branch(*this, x, INT_VAR_SIZE_MIN(), INT_VAL_SPLIT_MIN());

  }

  // Print solution
  virtual void
  print(std::ostream& os) const {
    os << x << endl;
  }


  // Constructor for cloning s
  CuriousSet(bool share, CuriousSet& s) : Script(share,s) {
    x.update(*this, share, s.x);
  }

  // Copy during cloning
  virtual Space*
  copy(bool share) {
    return new CuriousSet(share,*this);
  }
};


int
main(int argc, char* argv[]) {

  Options opt("CuriousSet");

  opt.solutions(0);

  opt.parse(argc,argv);

  Script::run<CuriousSet,DFS,Options>(opt);
    
  return 0;
}


