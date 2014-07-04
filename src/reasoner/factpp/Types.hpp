#ifndef OWL_OM_REASONER_FACTPP_TYPES_HPP
#define OWL_OM_REASONER_FACTPP_TYPES_HPP

class TExpressionManager;
class TDLAxiom;
class TDLConceptExpression;
class TDLIndividualExpression;
class TDLObjectRoleExpression;
class TDLDataRoleExpression;
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

} // end namespace owl_om
#endif
