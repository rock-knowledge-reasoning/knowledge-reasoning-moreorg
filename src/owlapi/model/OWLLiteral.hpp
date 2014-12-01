#ifndef OWLAPI_MODEL_OWL_LITERAL_HPP
#define OWLAPI_MODEL_OWL_LITERAL_HPP

#include <string.h>
#include <owl_om/owlapi/model/IRI.hpp>
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
protected:
    std::string mValue;
    std::string mType;

    OWLLiteral(const std::string& value);
public:
    typedef boost::shared_ptr<OWLLiteral> Ptr;

    /**
     * Test if literal is typed,
     * e.g. "1^^http://www.w3.org/2001/XMLSchema#nonNegativeInteger"
     */
    bool isTyped() const;

    bool hasType(const IRI& typeIRI) const;

    std::string getType() const { return mType; }

    // http://owlapi.sourceforge.net/javadoc/index.html?org/semanticweb/owlapi/model/OWLClassExpression.html
    //OWLDatatype getDatatype()
    //std::string getL
    //
    std::string toString() const;

    /**
     * Create a literal based on the given type information
     * \return pointer to an inbuilt literal type
     */
    static OWLLiteral::Ptr create(const std::string& literal);

    virtual int getInteger() const;
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_LITERAL_HPP
