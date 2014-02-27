#ifndef OWL_OM_GEN_NAMING_CONVERSION_HPP
#define OWL_OM_GEN_NAMING_CONVERSION_HPP

#include <string>
#include <vector>

namespace owl_om
{
typedef std::vector<std::string> NamespaceList;

class CanonizedName
{
    std::string mBasename;
    NamespaceList mNamespaces;

public:
    CanonizedName(const std::string basename, const NamespaceList& namespaces)
        : mBasename(basename)
        , mNamespaces(namespaces)
    {}

    std::string getBasename() const { return mBasename; }
    std::string getNamespace() const;
    NamespaceList getNamespaceList() const { return mNamespaces; }
};

class NamingConversion
{
public:
    static std::string snakeCase(const std::string& className);
    static std::string camelCase(const std::string& className);

    static CanonizedName canonizeClassName(const std::string& className);
};

} // end namespace owl_om
#endif // OWL_OM_GEN_NAMING_CONVERSION_HPP
