#ifndef MOREORG_INFERENCE_RULES_COMPOSITE_AGENT_RULE_HPP
#define MOREORG_INFERENCE_RULES_COMPOSITE_AGENT_RULE_HPP

#include "../InferenceRule.hpp"

namespace moreorg {
namespace inference_rules {

class CompositeAgentRule : public InferenceRule
{
public:
    typedef shared_ptr<CompositeAgentRule> Ptr;

    double apply(const facades::Robot& robot) const override;
};

} // end namespace inference_rules
} // end namespace moreorg
#endif // MOREORG_INFERENCE_RULES_COMPOSITE_AGENT_RULE_HPP
