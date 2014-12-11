#ifndef OWLAPI_IO_XML_UTILS_HPP
#define OWLAPI_IO_XML_UTILS_HPP

#include <string>

namespace owlapi {
namespace io {

class XMLUtils
{
public:
//    /** &amp;lt; shortcut */
//    static const std::string LT = "&lt;";
//    /** &amp;gt; shortcut */
//    static const std::string GT = "&gt;";
//    /** &amp;quot; shortcut */
//    static const std::string QUOT = "&quot;";
//    /** &amp;amp; shortcut */
//    static const std::string AMP = "&amp;";
//    /** &amp;apos; shortcut */
//    static const std::string APOS = "&apos;";
//    // For some point in the future
//    /** owl processing instruction */
//    static const std::string OWL_PROCESSING_INSTRUCTION_NAME = "owl";
//
//    /**
//     * Determines if a character is an XML name start character.
//     * 
//     * @param codePoint
//     *        The code point of the character to be tested. For UTF-16
//     *        characters the code point corresponds to the value of the char
//     *        that represents the character.
//     * @return {@code true} if {@code codePoint} is an XML name start character,
//     *         otherwise {@code false}
//     */
//    static bool isXMLNameStartCharacter(int codePoint);
//
// /**
//     * Determines if a character is an XML name character.
//     * 
//     * @param codePoint
//     *        The code point of the character to be tested. For UTF-8 and UTF-16
//     *        characters the code point corresponds to the value of the char
//     *        that represents the character.
//     * @return {@code true} if {@code codePoint} is an XML name start character,
//     *         otherwise {@code false}
//     */
//    static bool isXMLNameChar(int codePoint);
//
///**
//     * Deterimines if a character is an NCName (Non-Colonised Name) start
//     * character.
//     * 
//     * @param codePoint
//     *        The code point of the character to be tested. For UTF-8 and UTF-16
//     *        characters the code point corresponds to the value of the char
//     *        that represents the character.
//     * @return {@code true} if {@code codePoint} is a NCName start character,
//     *         otherwise {@code false}.
//     */
//    static boolean isNCNameStartChar(int codePoint) { return codePoint != ':' && isXMLNameStartCharacter(codePoint); }
//
// /**
//     * Deterimines if a character is an NCName (Non-Colonised Name) character.
//     * 
//     * @param codePoint
//     *        The code point of the character to be tested. For UTF-8 and UTF-16
//     *        characters the code point corresponds to the value of the char
//     *        that represents the character.
//     * @return {@code true} if {@code codePoint} is a NCName character,
//     *         otherwise {@code false}.
//     */
//    static boolean isNCNameChar(int codePoint) { return codePoint != ':' && isXMLNameChar(codePoint); }
//
// /**
//     * Determines if a character sequence is an NCName (Non-Colonised Name). An
//     * NCName is a string which starts with an NCName start character and is
//     * followed by zero or more NCName characters.
//     * 
//     * @param s
//     *        The character sequence to be tested.
//     * @return {@code true} if {@code s} is an NCName, otherwise {@code false}.
//     */
//    static bool isNCName(const std::string& s);
//
//
   /**
     * Gets the index of the longest NCName that is the suffix of a character
     * sequence.
     * 
     * @param s
     *        The character sequence.
     * @return The index of the longest suffix of the specified character
     *         sequence {@code s} that is an NCName, or -1 if the character
     *         sequence {@code s} does not have a suffix that is an NCName.
     */
    static size_t getNCNameSuffixIndex(const std::string& s);
};

} // end namespace io
} // end namespace owlapi
#endif // OWLAPI_IO_XML_UTILS_HPP
