#ifndef MOREORG_INFERENCE_RULES_COMPOSITE_AGENT_RULE_HPP
#define MOREORG_INFERENCE_RULES_COMPOSITE_AGENT_RULE_HPP

#include "../InferenceRule.hpp"

namespace moreorg {
namespace inference_rules {

class CompositeAgentRule : public InferenceRule
{
public:
    using Ptr = shared_ptr<CompositeAgentRule>;

    double apply(const facades::Robot& robot) const override;
};

} // end namespace inference_rules
} // end namespace moreorg
#endif // MOREORG_INFERENCE_RULES_COMPOSITE_AGENT_RULE_HPP
