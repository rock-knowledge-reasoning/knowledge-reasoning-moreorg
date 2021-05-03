#ifndef MOREORG_INFERENCE_RULE_HPP
#define MOREORG_INFERENCE_RULE_HPP

#include <owlapi/model/IRI.hpp>
#include "SharedPtr.hpp"
#include "OrganizationModelAsk.hpp"

namespace moreorg {
namespace facades {
    class Robot;
}

class Agent;
}

namespace moreorg {

struct OPArgument
{
    OPArgument(const owlapi::model::IRI& value, bool inverse = false);

    owlapi::model::IRI value;
    bool inverse;

    std::string toString() const;
};

class OPCall
{
public:
    OPCall() = default;

    OPCall(const std::string& op_name, const std::vector<OPArgument>& arguments);

    const std::string& getOPName() const { return mOPName; }

    const std::vector<OPArgument>& getArguments() const { return mArguments; }

    static double sum(const Agent& agent, const owlapi::model::IRI& dataproperty, const OrganizationModelAsk& ask);

    double evalComposite(const Agent& agent, const OrganizationModelAsk& ask) const;
    double evalAtomic(const Agent& agent, const OrganizationModelAsk& ask) const;

    std::string toString(size_t indent = 0) const;

protected:
    std::string mOPName;
    std::vector<OPArgument> mArguments;
};

class InferenceRule
{
public:
    virtual ~InferenceRule() = default;

    typedef shared_ptr<InferenceRule> Ptr;

    void load(const owlapi::model::IRI& dataproperty, const owlapi::model::IRI& ruleRelation, const OrganizationModelAsk& ask);

    void loadByName(const owlapi::model::IRI& ruleName, const OrganizationModelAsk& ask);

    /**
     * Load a composition rule that is related to a data property
     * via 'hasCompositionInferenceRule' relation
     *
     * A composition is only accounted for when agents are combined
     */
    static InferenceRule::Ptr loadPropertyCompositeAgentRule(const owlapi::model::IRI& dataproperty, const OrganizationModelAsk& ask);

    static InferenceRule::Ptr loadCompositeAgentRule(const owlapi::model::IRI& ruleName, const OrganizationModelAsk& ask);

    /**
     * Load a plain inference rule, for instance a formula for a property that
     * can be derived from others, that is related to a data property
     * via 'hasInferenceRule' relation.
     *
     * A plain rule is always applied, independently of dealing with atomic or composite
     * agents
     */
    static InferenceRule::Ptr loadPropertyAtomicAgentRule(const owlapi::model::IRI& dataproperty, const OrganizationModelAsk& ask);

    static InferenceRule::Ptr loadAtomicAgentRule(const owlapi::model::IRI& ruleName, const OrganizationModelAsk& ask);

    /**
     * Preparing the rule, means to instantiate the function by substituting placeholder
     * with correspondingly bound function calls/calls to inbuilt operations
     * \see OPCall
     */
    void prepare();

    /**
     * Apply the rule to a particular robot, either being atomic or composite
     * agent
     */
    virtual double apply(const facades::Robot& robot) const = 0;

    void addBinding(const owlapi::model::IRI& placeholder,
            const owlapi::model::IRI& name) { mBindings[placeholder] = name; }

    OPArgument resolvePlaceholder(const std::string& placeholder) const;

    std::string toString() const;

protected:
    OrganizationModelAsk mAsk;
    std::string mRule;
    std::map<owlapi::model::IRI, owlapi::model::IRI> mBindings;

    std::string mPreparedRule;
    std::map<std::string, OPCall> mOPCalls;

    static std::map<owlapi::model::IRI, InferenceRule::Ptr> mPropertyCompositeAgentRules;
    static std::map<owlapi::model::IRI, InferenceRule::Ptr> mPropertyAtomicAgentRules;

    static std::map<owlapi::model::IRI, InferenceRule::Ptr> mCompositeAgentRules;
    static std::map<owlapi::model::IRI, InferenceRule::Ptr> mAtomicAgentRules;

};

}
#endif // MOREORG_INFERENCE_RULE_HPP
