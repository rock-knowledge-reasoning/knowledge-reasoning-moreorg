
#ifndef MOREORG_INFERENCE_RULES_ATOMIC_AGENT_RULE_HPP
#define MOREORG_INFERENCE_RULES_ATOMIC_AGENT_RULE_HPP

#include "../InferenceRule.hpp"

namespace moreorg {
namespace inference_rules {

class AtomicAgentRule : public InferenceRule
{
public:
    using Ptr = shared_ptr<AtomicAgentRule>;

    double apply(const facades::Robot& robot) const override;
};

} // end namespace inference_rules
} // end namespace moreorg
#endif // MOREORG_INFERENCE_RULES_ATOMIC_AGENT_RULE_HPP
