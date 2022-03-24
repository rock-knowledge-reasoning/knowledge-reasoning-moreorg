#include "AtomicAgentRule.hpp"
#include "../Agent.hpp"
#include "../InferenceRule.hpp"
#include "../facades/Robot.hpp"

#include <muParser.h>
#include <stdexcept>

namespace moreorg {
namespace inference_rules {

double AtomicAgentRule::apply(const facades::Robot& robot) const
{
    if(robot.getModelPool().numberOfInstances() > 1)
    {
        throw std::runtime_error(
            "moreorg::inference_rules::AtomicAgentRule::apply"
            " trying to apply AtomicAgentRule to Composite agent" +
            robot.getModelPool().toString(4));
    }

    mu::Parser muParser;
    Agent agent(robot.getModelPool());
    for(const std::pair<std::string, OPCall>& p : mOPCalls)
    {
        const OPCall& opcall = p.second;
        double value = opcall.evalAtomic(agent, mAsk);
        muParser.DefineConst(p.first, value);
    }
    muParser.SetExpr(mPreparedRule);
    // muParser.EnableDebugDump(true,true);
    return muParser.Eval();
}

} // end namespace inference_rules
} // end namespace moreorg
