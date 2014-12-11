#ifndef OWLAPI_REASONER_FACTPP_TYPES_HPP
#define OWLAPI_REASONER_FACTPP_TYPES_HPP

#include <string>
#include <cstdlib>
#include <vector>

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
class DlCompletionTree;

namespace owlapi {
namespace reasoner {
namespace factpp {

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

    /**
     * Convert value to double if possible
     * \throw
     */
    double toDouble() const;
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

class ExplorationNode
{
    const DlCompletionTree* mExpression;

public:
    ExplorationNode(const DlCompletionTree* expression = NULL);

    const DlCompletionTree* get() const { return mExpression; }
};

typedef std::vector<ObjectPropertyExpression> ObjectPropertyExpressionList;
typedef std::vector<DataPropertyExpression> DataPropertyExpressionList;

} // end namespace factpp
} // end namespace reasoner
} // end namespace owlapi
#endif
