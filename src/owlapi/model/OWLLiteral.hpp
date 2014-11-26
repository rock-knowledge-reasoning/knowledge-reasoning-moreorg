#ifndef OWLAPI_MODEL_OWL_LITERAL_HPP
#define OWLAPI_MODEL_OWL_LITERAL_HPP

#include <owl_om/owlapi/model/OWLPropertyAssertionObject.hpp>

namespace owlapi {
namespace model {

/**
 *  Represents a Literal in the OWL 2 Specification.
 *
 *  Each literal consists of a lexical form, which is a string, and a datatype.
 *  A literal consisting of a lexical form "abc" and a datatype identified by
 *  the IRI datatypeIRI is written as "abc"^^datatypeIRI.
 *
 *  Note that literals whose datatype is rdf:PlainLiteral can be abbreviated.
 *  For example, literals of the form "abc@"^^rdf:PlainLiteral can be
 *  abbreviated in the functional-style syntax, and other concrete syntaxes to
 *  "abc". Literals of the form "abc@langTag"^^rdf:PlainLiteral where "langTag"
 *  is not empty are abbreviated in functional-style syntax documents (and other
 *  concrete syntaxes) to "abc"@langTag whenever possible
 */
class OWLLiteral : public OWLPropertyAssertionObject //OWLAnnotationObject, OWLAnnotationValue
{
public:
    typedef boost::shared_ptr<OWLLiteral> Ptr;

    // http://owlapi.sourceforge.net/javadoc/index.html?org/semanticweb/owlapi/model/OWLClassExpression.html
    //OWLDatatype getDatatype()
    //std::string getL

};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_LITERAL_HPP
