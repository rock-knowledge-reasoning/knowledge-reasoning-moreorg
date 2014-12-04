#include "OWLAxiom.hpp"
#include <boost/assign/list_of.hpp>

namespace owlapi {
namespace model {

std::map<OWLAxiom::AxiomType, std::string> OWLAxiom::AxiomTypeTxt = boost::assign::map_list_of
                (UNKOWN,                            "UNKNOWN")
                (Declaration,                       "Declaration")
                (ClassAxiom,                        "ClassAxiom")
                (ObjectPropertyAxiom,               "ObjectPropertyAxiom")
                (DataPropertyAxiom,                 "DataPropertyAxiom")
                (DatatypeDefinition,                "DatatypeDefinition")
                (Assertion,                         "Assertion")
                (HasKey,                            "HasKey")
                (AnnotationAxiom,                   "AnnotationAxiom")
                (SubClassOf,                        "SubClassOf")
                (EquivalentClasses,                 "EquivalentClasses")
                (DisjointClasses,                   "DisjointClasses")
                (DisjointUnion,                     "DisjointUnion")
                (EquivalentObjectProperties,        "EquivalentObjectProperties")
                (DisjointObjectProperties,          "DisjointObjectProperties") 
                (SubObjectPropertyOf,               "SubObjectPropertyOf")
                (ObjectPropertyDomain,              "ObjectPropertyDomain")
                (ObjectPropertyRange,               "ObjectPropertyRange")
                (InverseObjectProperties,           "InverseObjectProperties")
                (FunctionalObjectProperty,          "FunctionalObjectProperty")
                (ReflexiveObjectProperty,           "ReflexiveObjectProperty") 
                (InverseFunctionalObjectProperty,   "InverseFunctionalObjectProperty")
                (IrreflexiveObjectProperty,         "IrreflexiveObjectProperty")
                (SymmetricObjectProperty,           "SymmetricObjectProperty")
                (TransitiveObjectProperty,          "TransitiveObjectProperty") 
                (AsymmetricObjectProperty,          "AsymmetricObjectProperty")
                (SubDataPropertyOf,                 "SubDataPropertyOf")
                (DisjointDataProperties,            "DisjointDataProperties") 
                (EquivalentDataProperties,          "EquivalentDataProperties") 
                (FunctionalDataProperties,          "FunctionalDataProperties")
                (DataPropertyDomain,                "DataPropertyDomain")
                (DataPropertyRange,                 "DataPropertyRange") 
                (ClassAssertion,                    "ClassAssertion")
                (SameIndividual,                    "SameIndividual")
                (DifferentIndividuals,              "DifferentIndividuals")
                (ObjectPropertyAssertion,           "ObjectPropertyAssertion")
                (NegativeObjectPropertyAssertion,   "NegativeObjectPropertyAssertion")
                (DataPropertyAssertion,             "DataPropertyAssertion")
                (NegativeDataPropertyAssertion,     "NegativeDataPropertyAssertion")
                (SubAnnotationPropertyOf,           "SubAnnotationPropertyOf")
                (AnnotationPropertyDomain,          "AnnotationPropertyDomain")
                (AnnotationPropertyRange,           "AnnotationPropertyRange")
                (AnnotationAssertion,               "AnnotationAssertion")
                (SubPropertyChainOf,                "SubPropertyChainOf");

std::string OWLAxiom::toString() const { return "OWLAxiom: " + AxiomTypeTxt[mAxiomType]; }
} // end namespace model
} // end namespace owlapi
