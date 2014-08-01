#include "Types.hpp"

#include <factpp/Kernel.h>
#include <factpp/Actor.h>

namespace owl_om {

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

DataRange::DataRange(const TDLDataExpression* expression)
    : mExpression(expression)
{}

DataTypeName::DataTypeName(TDLDataTypeName* expression)
    : mExpression(expression)
{}

} // end namespace owl_om
