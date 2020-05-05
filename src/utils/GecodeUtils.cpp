#include "GecodeUtils.hpp"

namespace moreorg {
namespace utils {

std::map<Gecode::IntValBranch::Select, std::string> GecodeUtils::IntValSelect2Txt =
{
    { Gecode::IntValBranch::SEL_MIN, "MIN" },
    { Gecode::IntValBranch::SEL_MED, "MED" },
    { Gecode::IntValBranch::SEL_MAX, "MAX" },
    { Gecode::IntValBranch::SEL_RND, "RND" },
    { Gecode::IntValBranch::SEL_SPLIT_MIN, "SPLIT_MIN" },
    { Gecode::IntValBranch::SEL_SPLIT_MAX, "SPLIT_MAX" },
    { Gecode::IntValBranch::SEL_RANGE_MIN, "RANGE_MIN" },
    { Gecode::IntValBranch::SEL_RANGE_MAX, "RANGE_MAX" },
    { Gecode::IntValBranch::SEL_VAL_COMMIT, "VAL_COMMIT" },
    { Gecode::IntValBranch::SEL_VALUES_MIN, "VALUES_MIN" },
    { Gecode::IntValBranch::SEL_VALUES_MAX, "VALUES_MAX" }
};

std::map<Gecode::IntVarBranch::Select, std::string> GecodeUtils::IntVarSelect2Txt =
{
    { Gecode::IntVarBranch::SEL_NONE, "NONE" },
    { Gecode::IntVarBranch::SEL_RND,  "RND" },
    { Gecode::IntVarBranch::SEL_MERIT_MIN, "MERIT_MIN" },
    { Gecode::IntVarBranch::SEL_MERIT_MAX, "MERIT_MAX" },
    { Gecode::IntVarBranch::SEL_DEGREE_MIN, "DEGREE_MIN" },
    { Gecode::IntVarBranch::SEL_DEGREE_MAX, "DEGREE_MAX" },
    { Gecode::IntVarBranch::SEL_AFC_MIN, "AFC_MIN" },
    { Gecode::IntVarBranch::SEL_AFC_MAX, "AFC_MAX" },
    { Gecode::IntVarBranch::SEL_ACTION_MIN, "ACTION_MIN" },
    { Gecode::IntVarBranch::SEL_ACTION_MAX, "ACTION_MAX" },
    { Gecode::IntVarBranch::SEL_CHB_MIN, "CHB_MIN" },
    { Gecode::IntVarBranch::SEL_CHB_MAX, "CHB_MAX" },
    { Gecode::IntVarBranch::SEL_MIN_MIN, "MIN_MIN" },
    { Gecode::IntVarBranch::SEL_MIN_MAX, "MIN_MAX" },
    { Gecode::IntVarBranch::SEL_MAX_MIN, "MAX_MIN" },
    { Gecode::IntVarBranch::SEL_MAX_MAX, "MAX_MAX" },
    { Gecode::IntVarBranch::SEL_SIZE_MIN, "SIZE_MIN" },
    { Gecode::IntVarBranch::SEL_SIZE_MAX, "SIZE_MAX" },
    { Gecode::IntVarBranch::SEL_DEGREE_SIZE_MIN, "DEGREE_SIZE_MIN" },
    { Gecode::IntVarBranch::SEL_DEGREE_SIZE_MAX, "DEGREE_SIZE_MAX" },
    { Gecode::IntVarBranch::SEL_AFC_SIZE_MIN, "AFC_SIZE_MIN" },
    { Gecode::IntVarBranch::SEL_AFC_SIZE_MAX, "AFC_SIZE_MAX" },
    { Gecode::IntVarBranch::SEL_ACTION_SIZE_MIN, "ACTION_SIZE_MIN" },
    { Gecode::IntVarBranch::SEL_ACTION_SIZE_MAX, "ACTION_SIZE_MAX" },
    { Gecode::IntVarBranch::SEL_CHB_SIZE_MIN, "CHB_SIZE_MIN" },
    { Gecode::IntVarBranch::SEL_CHB_SIZE_MAX, "CHB_SIZE_MAX" },
    { Gecode::IntVarBranch::SEL_REGRET_MIN_MIN, "REGRET_MIN_MIN" },
    { Gecode::IntVarBranch::SEL_REGRET_MIN_MAX, "REGRET_MIN_MAX" },
    { Gecode::IntVarBranch::SEL_REGRET_MAX_MIN, "REGRET_MAX_MIN" },
    { Gecode::IntVarBranch::SEL_REGRET_MAX_MAX, "REGRET_MAX_MAX" }
};


Gecode::IntValBranch::Select GecodeUtils::getIntValSelect(const std::string& txt)
{
    for(const std::pair<Gecode::IntValBranch::Select, std::string>& p : IntValSelect2Txt)
    {
        if(p.second == txt)
        {
            return p.first;
        }
    }
    throw std::invalid_argument("moreorg::utils::GecodeUtils::getIntValSelect: could not find value for '" + txt + "'");
}

Gecode::IntVarBranch::Select GecodeUtils::getIntVarSelect(const std::string& txt)
{
    for(const std::pair<Gecode::IntVarBranch::Select, std::string>& p : IntVarSelect2Txt)
    {
        if(p.second == txt)
        {
            return p.first;
        }
    }
    throw std::invalid_argument("moreorg::utils::GecodeUtils::getIntVarSelect: could not find value for '" + txt + "'");
}

} // end namespace utils
} // end namespace moreorg
