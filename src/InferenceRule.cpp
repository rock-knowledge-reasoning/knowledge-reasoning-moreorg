#include "InferenceRule.hpp"
#include "Agent.hpp"
#include "facades/Robot.hpp"
#include "vocabularies/OM.hpp"
#include <base-logging/Logging.hpp>
#include "policies/SelectionPolicy.hpp"
#include "inference_rules/CompositeAgentRule.hpp"
#include "inference_rules/AtomicAgentRule.hpp"

using namespace owlapi::model;

namespace moreorg {

OPArgument::OPArgument(const owlapi::model::IRI& value,
        bool inverse)
    : value(value)
    , inverse(inverse)
{}

std::string OPArgument::toString() const
{
    std::stringstream ss;
    ss << "OPArgument: " << value.toString() << ", inverse:" << inverse;
    return ss.str();
}

OPCall::OPCall(const std::string& op_name, const std::vector<OPArgument>& arguments)
    : mOPName(op_name)
    , mArguments(arguments)
{}

double OPCall::evalComposite(const Agent& agent, const OrganizationModelAsk& ask) const
{
    if(mArguments.size() != 2)
    {
        std::stringstream ss;
        ss << "moreorg::OPCall::evalComposite: "
           << mOPName << " requires 2 arguments"
           << ", but received " << mArguments.size();
        throw std::runtime_error(ss.str());
    }

    owlapi::model::IRI selectionPolicyName = mArguments[0].value;
    owlapi::model::IRI dataPropertyName = mArguments[1].value;

    std::set<ModelPool> combinations = agent.getType().allCombinations();
    Agent::Set agents;
    for(const ModelPool& pool : combinations)
    {
        agents.insert(pool);
    }

    Policy::Ptr policy = Policy::getInstance(selectionPolicyName, ask);
    policies::SelectionPolicy::Ptr selectionPolicy = dynamic_pointer_cast<policies::SelectionPolicy>(policy);

    policies::Selection selection = selectionPolicy->apply(agents, ask);
    Agent selectedAgent = *selection.begin();

    if(mArguments[0].inverse)
    {
        ModelPoolDelta delta = Algebra::delta(selectedAgent.getType(), agent.getType());
        selectedAgent = Agent(delta.toModelPool());
    }

    if(mOPName == "SUM")
    {
        return sum(selectedAgent, dataPropertyName, ask);
    }

    throw std::runtime_error("moreorg::OPCall::eval: operation '" + mOPName + "' is not supported");
}


double OPCall::evalAtomic(const Agent& agent, const OrganizationModelAsk& ask) const
{
    if(mArguments.size() != 1)
    {
        std::stringstream ss;
        ss << "moreorg::OPCall::evalComposite: "
           << mOPName << " requires 1 argument"
           << ", but received " << mArguments.size();
        throw std::runtime_error(ss.str());
    }

    owlapi::model::IRI dataPropertyName = mArguments[0].value;

    if(mOPName == "VALUE")
    {
        return sum(agent, dataPropertyName, ask);
    }

    throw std::runtime_error("moreorg::OPCall::eval: operation '" + mOPName + "' is not supported");
}

double OPCall::sum(const Agent& agent, const IRI& dataproperty, const OrganizationModelAsk& ask)
{
    double sum = 0.0;
    for(const ModelPool::value_type& pair : agent.getType())
    {
        facades::Robot robot = facades::Robot::getInstance(pair.first, ask);
        sum += pair.second*robot.getDataPropertyValue(dataproperty);
    }
    return sum;
}

std::string OPCall::toString(size_t indent) const
{
    std::stringstream ss;
    std::string hspace(indent,' ');
    ss << hspace << "OPCall " << mOPName;
    for(const OPArgument& arg : mArguments)
    {
        ss << hspace << arg.toString() << std::endl;
    }
    return ss.str();
}


std::map<IRI, InferenceRule::Ptr> InferenceRule::mPropertyCompositeAgentRules;
std::map<IRI, InferenceRule::Ptr> InferenceRule::mPropertyAtomicAgentRules;

std::map<IRI, InferenceRule::Ptr> InferenceRule::mCompositeAgentRules;
std::map<IRI, InferenceRule::Ptr> InferenceRule::mAtomicAgentRules;

InferenceRule::Ptr InferenceRule::loadPropertyCompositeAgentRule(const IRI& dataproperty,
        const OrganizationModelAsk& ask)
{
    std::map<IRI, InferenceRule::Ptr>::const_iterator cit = mPropertyCompositeAgentRules.find(dataproperty);
    if(cit != mPropertyCompositeAgentRules.end())
    {
        return cit->second;
    }

    inference_rules::CompositeAgentRule::Ptr rule = make_shared<inference_rules::CompositeAgentRule>();
    rule->load(dataproperty, vocabulary::OM::resolve("hasCompositeAgentRule"), ask);
    mPropertyCompositeAgentRules[dataproperty] = rule;
    return rule;
}

InferenceRule::Ptr InferenceRule::loadCompositeAgentRule(const IRI& ruleName,
        const OrganizationModelAsk& ask)
{
    std::map<IRI, InferenceRule::Ptr>::const_iterator cit =
        mCompositeAgentRules.find(ruleName);
    if(cit != mCompositeAgentRules.end())
    {
        return cit->second;
    }

    inference_rules::CompositeAgentRule::Ptr rule = make_shared<inference_rules::CompositeAgentRule>();
    rule->loadByName(ruleName, ask);
    mCompositeAgentRules[ruleName] = rule;
    return rule;
}

InferenceRule::Ptr InferenceRule::loadPropertyAtomicAgentRule(const IRI& dataproperty,
        const OrganizationModelAsk& ask)
{
    std::map<IRI, InferenceRule::Ptr>::const_iterator cit = mPropertyAtomicAgentRules.find(dataproperty);
    if(cit != mPropertyAtomicAgentRules.end())
    {
        return cit->second;
    }

    inference_rules::AtomicAgentRule::Ptr rule = make_shared<inference_rules::AtomicAgentRule>();
    rule->load(dataproperty, vocabulary::OM::resolve("hasAtomicAgentRule"), ask);
    mPropertyAtomicAgentRules[dataproperty] = rule;
    return rule;
}

InferenceRule::Ptr InferenceRule::loadAtomicAgentRule(const IRI& ruleName,
        const OrganizationModelAsk& ask)
{
    std::map<IRI, InferenceRule::Ptr>::const_iterator cit =
        mAtomicAgentRules.find(ruleName);
    if(cit != mAtomicAgentRules.end())
    {
        return cit->second;
    }

    inference_rules::AtomicAgentRule::Ptr rule = make_shared<inference_rules::AtomicAgentRule>();
    rule->loadByName(ruleName, ask);
    mAtomicAgentRules[ruleName] = rule;
    return rule;
}

void InferenceRule::load(const IRI& dataproperty, const IRI& roleRelation, const OrganizationModelAsk& ask)
{
    OWLAnnotationValue::Ptr annotationValue;
    IRI pickFromProperty = dataproperty;
    while(true)
    {
        annotationValue =
            ask.ontology().getAnnotationValue(pickFromProperty, roleRelation);
        if(annotationValue)
        {
            break;
        }

        owlapi::model::IRIList ancestors = ask.ontology().ancestors(pickFromProperty, true);
        if(ancestors.empty())
        {
            break;
        }
        pickFromProperty = ancestors.front();
    }

    if(!annotationValue)
    {
        throw std::invalid_argument("moreorg::InferenceRule::load:"
                " failed to retrieve inference rule via '"
                + roleRelation.toString() + "'");
    }


    owlapi::model::IRI ruleName = annotationValue->asIRI();

    // default binding for self
    addBinding(vocabulary::OM::resolve("_self"), dataproperty);

    loadByName(ruleName, ask);
}


void InferenceRule::loadByName(const IRI& ruleName, const OrganizationModelAsk& ask)
{
    OWLAnnotationValue::Ptr ruleTxt = ask.ontology().getAnnotationValue(ruleName, vocabulary::OM::resolve("inferFrom"));
    OWLLiteral::Ptr literal = ruleTxt->asLiteral();

    if(!literal)
    {
        throw std::runtime_error("moreorg::InferenceRule::load:"
                " failed to identify literal from rule '"
                + ruleName.toString() + "'");
    }

    mRule = literal->getValue();
    mAsk = ask;

    std::vector<char> placeholders = {'a','b','c','d','e','f','g','h','i'};
    for(auto c : placeholders)
    {
        std::string fragment("_");
        fragment += c;
        owlapi::model::IRI placeholder = vocabulary::OM::resolve(fragment);

        OWLAnnotationValue::Ptr placeholderValue;
        try
        {
             placeholderValue = ask.ontology().getAnnotationValue(ruleName, placeholder);
        } catch(const std::invalid_argument& e)
        {
            break;
        }

        if(placeholderValue)
        {
            addBinding(placeholder, placeholderValue->asIRI());
        } else {
            break;
        }
    }

    prepare();
}

void InferenceRule::prepare()
{
    size_t placeholder_id = 0;

    std::string rule = mRule;

    // rewrite inverse
    while(true)
    {
        size_t startNOT = rule.find("NOT(");
        if(startNOT == std::string::npos)
        {
            break;
        }
        size_t endNOT = rule.find(")",startNOT);
        std::string expr = rule.substr(startNOT+4, endNOT-startNOT-4);

        rule.replace(startNOT,endNOT-startNOT+1, "__NOT__" + expr);
    }

    while(true)
    {
        size_t startOP = rule.find("OP_");
        if(startOP == std::string::npos)
        {
            break;
        }

        size_t startARGS = rule.find("(", startOP);
        size_t endARGS = rule.find(")", startARGS);

        std::string op_name = rule.substr(startOP+3, startARGS-startOP-3);
        std::string args = rule.substr(startARGS+1,endARGS-startARGS-1);


        std::stringstream ss;
        ss << "__" << ++placeholder_id << "__";
        std::string placeholder = ss.str();

        std::string updatedRule = rule.replace(startOP, endARGS-startOP+1, placeholder);
        rule = updatedRule;

        // remove whitespaces
        std::vector<OPArgument> arguments;
        args.erase( std::remove_if( args.begin(), args.end(), ::isspace ), args.end() );
        size_t startpos = 0;
        while(true)
        {
            size_t pos = args.find_first_of(",", startpos);

            if(pos == std::string::npos)
            {
                std::string p = args.substr(startpos);
                arguments.push_back(resolvePlaceholder(p));
                break;
            } else {
                std::string p = args.substr(startpos,pos);
                arguments.push_back(resolvePlaceholder(p));
            }
            startpos = pos + 1;
        }

        OPCall oc(op_name, arguments);
        LOG_DEBUG_S << "ADDING OPCALL FOR " << placeholder
            << " : " << oc.toString(4);
        mOPCalls[placeholder] = oc;
    }
    LOG_DEBUG_S << "Prepared rule: " << rule << " with #" << mOPCalls.size() << " operation calls";
    mPreparedRule = rule;
}

std::string InferenceRule::toString() const
{
    std::string txt;
    txt += mRule;
    for(const std::pair<IRI,IRI>& binding : mBindings)
    {
        txt += ", " + binding.first.getFragment() + ": " + binding.second.getFragment();
    }
    return txt;
}

OPArgument InferenceRule::resolvePlaceholder(const std::string& _placeholder) const
{
    bool inverse = false;
    std::string placeholder = _placeholder;
    if(_placeholder.find("__NOT__") != std::string::npos)
    {
        placeholder = _placeholder.substr(7,placeholder.size() - 7);
        inverse = true;
    }

    for(const std::pair<IRI,IRI>& pair : mBindings)
    {
        if(pair.first.getFragment() == placeholder || pair.first == IRI(placeholder))
        {
            return OPArgument(pair.second, inverse);
        }
    }

    throw std::runtime_error("moreorg::InferenceRule::resolvePlaceholder: failed to resolve '" + placeholder + "'");
}

} // end namespace moreorg
