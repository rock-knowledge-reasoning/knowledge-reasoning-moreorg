#ifndef OWL_OM_REASONER_FACTPP_TYPES_HPP
#define OWL_OM_REASONER_FACTPP_TYPES_HPP

#include <string>
#include <cstdlib>

class TExpressionManager;
class TDLAxiom;
class TDLConceptExpression;
class TDLIndividualExpression;
class TDLObjectRoleExpression;
class TDLDataRoleExpression;
class TDLDataValue;
class TDLDataExpression;
class TDLDataTypeName;
class ReasoningKernel;

namespace owl_om {

class Axiom
{
    TDLAxiom* mAxiom;

public:
    Axiom(TDLAxiom* axiom);

    const TDLAxiom* get() const { return mAxiom; } 
    TDLAxiom* get() { return mAxiom; }
};

class ClassExpression
{
    TDLConceptExpression* mExpression;

public:
    ClassExpression(TDLConceptExpression* expression = NULL);

    const TDLConceptExpression* get() const { return mExpression; }
};

class InstanceExpression
{
    TDLIndividualExpression* mExpression;

public:
    InstanceExpression(TDLIndividualExpression* expression = NULL);

    const TDLIndividualExpression* get() const { return mExpression; }
};

class ObjectPropertyExpression
{
    TDLObjectRoleExpression* mExpression;

public:
    ObjectPropertyExpression(TDLObjectRoleExpression* expression = NULL);

    const TDLObjectRoleExpression* get() const { return mExpression; }
};

class DataPropertyExpression
{
    TDLDataRoleExpression* mExpression;

public:
    DataPropertyExpression(TDLDataRoleExpression* expression = NULL);

    const TDLDataRoleExpression* get() const { return mExpression; }
};

class DataValue
{
    const TDLDataValue* mExpression;

public:
    DataValue(const TDLDataValue* expression = NULL);

    const TDLDataValue* get() const { return mExpression; }

    std::string getValue() const;
    std::string getType() const;
};

class DataRange
{
    const TDLDataExpression* mExpression;

public:
    DataRange(const TDLDataExpression* expression = NULL);

    const TDLDataExpression* get() const { return mExpression; }
};

class DataTypeName
{
    TDLDataTypeName* mExpression;

public:
    DataTypeName(TDLDataTypeName* expression = NULL);

    const TDLDataTypeName* get() const { return mExpression; }
    TDLDataTypeName* get() { return mExpression; }
};



} // end namespace owl_om
#endif
