#include "XMLUtils.hpp"
// http://icu-project.org/apiref/icu4c/index.html
#include <unicode/unistr.h>

namespace owlapi {
namespace io {


//bool XMLUtils::isXMLNameStartCharacter(int codePoint)
//{
//    return codePoint == ':' || codePoint >= 'A' && codePoint <= 'Z'
//                || codePoint == '_' || codePoint >= 'a' && codePoint <= 'z'
//                || codePoint >= 0xC0 && codePoint <= 0xD6 || codePoint >= 0xD8
//                && codePoint <= 0xF6 || codePoint >= 0xF8 && codePoint <= 0x2FF
//                || codePoint >= 0x370 && codePoint <= 0x37D
//                || codePoint >= 0x37F && codePoint <= 0x1FFF
//                || codePoint >= 0x200C && codePoint <= 0x200D
//                || codePoint >= 0x2070 && codePoint <= 0x218F
//                || codePoint >= 0x2C00 && codePoint <= 0x2FEF
//                || codePoint >= 0x3001 && codePoint <= 0xD7FF
//                || codePoint >= 0xF900 && codePoint <= 0xFDCF
//                || codePoint >= 0xFDF0 && codePoint <= 0xFFFD
//                || codePoint >= 0x10000 && codePoint <= 0xEFFFF;
//}
//
//
//bool XMLUtils::isXMLNameChar(int codePoint)
//{
//    return isXMLNameStartCharacter(codePoint) || codePoint == '-'
//            || codePoint == '.' || codePoint >= '0' && codePoint <= '9'
//            || codePoint == 0xB7 || codePoint >= 0x0300
//            && codePoint <= 0x036F || codePoint >= 0x203F
//            && codePoint <= 0x2040;
//}
//
//bool XMLUtils::isNCName(const std::string& s)
//{
//    if (s.empty())
//    {
//        return false;
//    }
//
//    UnicodeString unicodeString(s.data(), s.size());
//    unicodeString.indexOf(
//    int firstCodePoint = Character.codePointAt(s, 0);
//    if (!isNCNameStartChar(firstCodePoint)) {
//        return false;
//    }
//
//    for (int i = Character.charCount(firstCodePoint); i < s.length();) {
//        int codePoint = Character.codePointAt(s, i);
//        if (!isNCNameChar(codePoint)) {
//            return false;
//        }
//        i += Character.charCount(codePoint);
//    }
//    return true;
//}

size_t XMLUtils::getNCNameSuffixIndex(const std::string& s)
{
    // identify bnode labels and do not try to split them
    if (s.size() > 1 && s.at(0) == '_' && s.at(1) == ':')
    {
       return -1;
    }

    UnicodeString unicodeString(s.data(), s.size());
    UnicodeString colon(":");
    int32_t pos = unicodeString.indexOf(colon);
    if(pos < 0)
    { 
        return std::string::npos;
    } else {
        return pos;
    }
}


} // end namespace io
} // end namespace owlapi
