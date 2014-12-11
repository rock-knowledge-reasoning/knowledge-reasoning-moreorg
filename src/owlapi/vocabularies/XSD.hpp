#ifndef OWLAPI_VOCABULARIES_XSD_HPP
#define OWLAPI_VOCABULARIES_XSD_HPP

#ifndef VOCABULARY_BASE_IRI
#error "Direct inclusion of this header is forbidden. Use Vocabulary.hpp instead"
#endif

namespace owlapi {
namespace vocabulary {

/**
 * For inbuilt type in c++: long, int, short, double, float
 * use the syntax XSD::resolve("long")
 * \see http://www.w3.org/TR/owl2-syntax/#Real_Numbers.2C_Decimal_Numbers.2C_and_Integers
 */
class XSD
{
public:
    VOCABULARY_BASE_IRI("http://www.w3.org/2001/XMLSchema#");
    VOCABULARY_DYNAMIC_EXTENSION

    VOCABULARY_ADD_WORD(decimal);
    VOCABULARY_ADD_WORD(integer);
    VOCABULARY_ADD_WORD(nonNegativeInteger);
    VOCABULARY_ADD_WORD(nonPositiveInteger);
    VOCABULARY_ADD_WORD(positiveInteger);
    VOCABULARY_ADD_WORD(negativeInteger);

    /// C++ inbuilt types use resolve instead
    //VOCABULARY_ADD_WORD(long);
    //VOCABULARY_ADD_WORD(int);
    //VOCABULARY_ADD_WORD(short);

    VOCABULARY_ADD_WORD(byte);
    VOCABULARY_ADD_WORD(unsignedLong);
    VOCABULARY_ADD_WORD(unsignedInt);
    VOCABULARY_ADD_WORD(unsignedShort);
    VOCABULARY_ADD_WORD(unsignedByte);

    /// C++ inbuilt types use resolve instead
    //VOCABULARY_ADD_WORD(double);
    //VOCABULARY_ADD_WORD(float);

    VOCABULARY_ADD_WORD(string);
    VOCABULARY_ADD_WORD(normalizedString);
    VOCABULARY_ADD_WORD(token);
    VOCABULARY_ADD_WORD(language);
    VOCABULARY_ADD_WORD(Name);
    VOCABULARY_ADD_WORD(NCName);
    VOCABULARY_ADD_WORD(NMTOKEN);

    VOCABULARY_ADD_WORD(hexBinary);
    VOCABULARY_ADD_WORD(base64Binary);

    VOCABULARY_ADD_WORD(dateTime);
    VOCABULARY_ADD_WORD(dateTimeStamp);
};

} // end namespace vocabulary
} // end namespace owlapi
#endif // OWLAPI_VOCABULARIES_XSD_HPP
