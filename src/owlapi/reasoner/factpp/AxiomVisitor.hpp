#ifndef OWLAPI_REASONER_FACTPP_TYPE_VISITOR_HPP
#define OWLAPI_REASONER_FACTPP_TYPE_VISITOR_HPP

#include <factpp/tDLAxiom.hpp>

namespace owlapi {
namespace reasoner {
namespace factpp {

class AxiomTypeGetter : DLAxiomVisitor
{
public:         // visitor interface
    virtual void visit ( const TDLAxiomDeclaration& axiom )
    {
        const TDLExpression* decl = axiom.getDeclaration();
        if( dynamic_cast<const TDLConceptName*>(decl) != NULL )
        {
        } else if ( dynamic_cast<const TDLIndividualName*>(decl) != NULL )
        {
        } else if ( dynamic_cast <const TDLObjectRoleName*>(decl) != NULL )
        {
        } else if ( dynamic_cast<const TDLDataRoleName*>(decl) != NULL )
        {
        }
    }
    virtual void visit ( const TDLAxiomEquivalentConcepts& axiom )
    { 
      // print ( axiom.begin(), axiom.end() );
    }
    virtual void visit ( const TDLAxiomDisjointConcepts& axiom )
    { 
        //print ( axiom.begin(), axiom.end() );
    }
    virtual void visit ( const TDLAxiomDisjointUnion& axiom )
    {
//                 { o << "(disjoint_c"; print ( axiom.begin(), axiom.end() ); o << ")\n(equal_c (or" << axiom.getC(); print ( axiom.begin(), axiom.end() ); o << "))\n"; }
    }
    virtual void visit ( const TDLAxiomEquivalentORoles& axiom )
    { //o << "(equal_r"; print ( axiom.begin(), axiom.end() ); o << ")\n"; 
    }
    virtual void visit ( const TDLAxiomEquivalentDRoles& axiom )
    { //o << "(equal_r"; print ( axiom.begin(), axiom.end() ); o << ")\n"; 
    }
    virtual void visit ( const TDLAxiomDisjointORoles& axiom )
    { //o << "(disjoint_r"; print ( axiom.begin(), axiom.end() ); o << ")\n"; 
    }
    virtual void visit ( const TDLAxiomDisjointDRoles& axiom ) 
    { //o << "(disjoint_r"; print ( axiom.begin(), axiom.end() ); o << ")\n"; 
    }
    virtual void visit ( const TDLAxiomSameIndividuals& axiom ) 
    { //o << "(same"; print ( axiom.begin(), axiom.end() ); o << ")\n"; 
    }
    virtual void visit ( const TDLAxiomDifferentIndividuals& axiom ) 
    { //o << "(different"; print ( axiom.begin(), axiom.end() ); o << ")\n"; 
    }
    virtual void visit ( const TDLAxiomFairnessConstraint& axiom )
    { //o << "(fairness"; print ( axiom.begin(), axiom.end() ); o << ")\n"; 
    }
    virtual void visit ( const TDLAxiomRoleInverse& axiom )
    { 
        //*this << "(equal_r" << axiom.getRole() << " (inv" << axiom.getInvRole() << "))\n"; 
    }
    virtual void visit ( const TDLAxiomORoleSubsumption& axiom )
    { 
        //*this << "(implies_r" << axiom.getSubRole() << axiom.getRole() << ")\n"; 
    }
    virtual void visit ( const TDLAxiomDRoleSubsumption& axiom )
    {
        //*this << "(implies_r" << axiom.getSubRole() << axiom.getRole() << ")\n"; 
    }
    virtual void visit ( const TDLAxiomORoleDomain& axiom )
    {
        //*this << "(domain" << axiom.getRole() << axiom.getDomain() << ")\n";
    }
    virtual void visit ( const TDLAxiomDRoleDomain& axiom )
    { 
        //*this << "(domain" << axiom.getRole() << axiom.getDomain() << ")\n"; 
    }
    virtual void visit ( const TDLAxiomORoleRange& axiom )
    {
        //*this << "(range" << axiom.getRole() << axiom.getRange() << ")\n"; 
    }
    virtual void visit ( const TDLAxiomDRoleRange& axiom )i
    {
        //*this << "(range" << axiom.getRole() << axiom.getRange() << ")\n";
    }
    virtual void visit ( const TDLAxiomRoleTransitive& axiom )
    { 
        //*this << "(transitive" << axiom.getRole() << ")\n"; 
    }
    virtual void visit ( const TDLAxiomRoleReflexive& axiom )
    { 
        //*this << "(reflexive" << axiom.getRole() << ")\n";
    }
    virtual void visit ( const TDLAxiomRoleIrreflexive& axiom )
    {  
        //*this << "(irreflexive" << axiom.getRole() << ")\n";
    }
    virtual void visit ( const TDLAxiomRoleSymmetric& axiom )
    { 
        //*this << "(symmetric" << axiom.getRole() << ")\n"; 
    }
    virtual void visit ( const TDLAxiomRoleAsymmetric& axiom )
    { 
        //*this << "(asymmetric" << axiom.getRole() << ")\n";
    }
    virtual void visit ( const TDLAxiomORoleFunctional& axiom )
    { 
        //*this << "(functional" << axiom.getRole() << ")\n";
    }
    virtual void visit ( const TDLAxiomDRoleFunctional& axiom )
    { 
        //*this << "(functional" << axiom.getRole() << ")\n"; 
    }
    virtual void visit ( const TDLAxiomRoleInverseFunctional& axiom ) 
    { 
        //*this << "(functional (inv" << axiom.getRole() << "))\n";
    }
    virtual void visit ( const TDLAxiomConceptInclusion& axiom )
    { 
        //*this << "(implies_c" << axiom.getSubC() << axiom.getSupC() << ")\n";
    }
    virtual void visit ( const TDLAxiomInstanceOf& axiom )
    {
        // *this << "(instance" << axiom.getIndividual()  << axiom.getC() << ")\n";
    }
    virtual void visit ( const TDLAxiomRelatedTo& axiom )
    {
        //*this << "(related" << axiom.getIndividual() << axiom.getRelation() << axiom.getRelatedIndividual() << ")\n";
    }
    virtual void visit ( const TDLAxiomRelatedToNot& axiom )
    { 
        //*this<< "(instance" << axiom.getIndividual() << " (all" << axiom.getRelation() << "(not" << axiom.getRelatedIndividual() << ")))\n";
    }
    virtual void visit ( const TDLAxiomValueOf& axiom )
    {
        //*this << "(instance" << axiom.getIndividual() << " (some" << axiom.getAttribute() << axiom.getValue() << "))\n";
    }
    virtual void visit ( const TDLAxiomValueOfNot& axiom )
    {
        //*this << "(instance" << axiom.getIndividual() << " (all" << axiom.getAttribute() << "(not " << axiom.getValue() << ")))\n";
    }
};

} // end namespace factpp
} // end namespace reasoner
} // end namespace owlapi
#endif // OWLAPI_REASONER_FACTPP_TYPE_VISITOR_HPP
