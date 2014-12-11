#include "Types.hpp"
#include <sstream>
#include <stdexcept>

#include <factpp/Kernel.h>
#include <factpp/Actor.h>

namespace owlapi {
namespace reasoner {
namespace factpp {

Axiom::Axiom(TDLAxiom* axiom)
    : mAxiom(axiom)
{
}

ClassExpression::ClassExpression(TDLConceptExpression* expression)
    : mExpression(expression)
{}

InstanceExpression::InstanceExpression(TDLIndividualExpression* expression)
    : mExpression(expression)
{}

ObjectPropertyExpression::ObjectPropertyExpression(TDLObjectRoleExpression* expression)
    : mExpression(expression)
{}

DataPropertyExpression::DataPropertyExpression(TDLDataRoleExpression* expression)
    : mExpression(expression)
{}

DataValue::DataValue(const TDLDataValue* expression)
    : mExpression(expression)
{}

std::string DataValue::getValue() const
{ 
    if(mExpression)
    {
        // relies on functionality of TNamedEntity
        return mExpression->getName(); 
    }
    return std::string();
}

std::string DataValue::getType() const
{
    if(mExpression)
    {
        TDLDataTypeExpression* dataTypeName = const_cast<TDLDataTypeExpression*>(mExpression->getExpr());
        return getBasicDataType(dataTypeName)->getName();
    }
    return std::string();

}

double DataValue::toDouble() const
{
    std::stringstream ss;
    ss.exceptions(std::ios::failbit);
    ss << getValue();

    double numericValue;
    try {
        ss >> numericValue;
        return numericValue;
    } catch(const std::exception& e)
    {
        throw std::runtime_error("owl_om::DataValue::toDouble invalid value format -- '" + getValue() + "' cannot be converted to double");
    }

}

DataRange::DataRange(const TDLDataExpression* expression)
    : mExpression(expression)
{}

DataTypeName::DataTypeName(TDLDataTypeName* expression)
    : mExpression(expression)
{}

ExplorationNode::ExplorationNode(const DlCompletionTree* expression)
    : mExpression(expression)
{}

} // end namespace factpp
} // end namespace reasoner
} // end namespace owlapi
