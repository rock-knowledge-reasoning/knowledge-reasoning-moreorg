#include "gecode/int.hh"
#include <map>

namespace moreorg {
namespace utils {

class GecodeUtils
{
public:

    static std::map<Gecode::IntValBranch::Select, std::string> IntValSelect2Txt;
    static std::map<Gecode::IntVarBranch::Select, std::string> IntVarSelect2Txt;


    static Gecode::IntValBranch::Select getIntValSelect(const std::string& txt);
    static Gecode::IntVarBranch::Select getIntVarSelect(const std::string& txt);


};

} // end namespace utils
} // end namespace moreorg
