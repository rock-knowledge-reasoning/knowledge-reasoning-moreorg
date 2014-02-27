#ifndef OWL_OM_GEN_MODEL_GENERATOR_HPP
#define OWL_OM_GEN_MODEL_GENERATOR_HPP

#include <owl_om/db/KnowledgeBase.hpp>

namespace owl_om 
{

struct RenderedFile
{
    std::string filename;
    std::string content;

    RenderedFile(const std::string& filename, const std::string& content)
        : filename(filename)
        , content(content)
    {}
};

typedef std::vector<RenderedFile> RenderedFiles;

class ModelGenerator
{
    db::KnowledgeBase::Ptr mKnowledgeBase;

    std::string mNamespace;

    static std::map<std::string, std::string> msTemplates;

public:
    static RenderedFiles render(const std::string& templateFile, owl_om::Class::Ptr klass, const std::string& ns);

    static std::string generateInclude(owl_om::Class::Ptr c, const std::string& ns);

    static void registerTemplate(const std::string& name, const std::string& file);

    ModelGenerator(const std::string& ontologyFile, const std::string& ns);

    void load(const std::string& filename);

    RenderedFiles renderClassFiles();

};

} // end namespace owl_om
#endif // OWL_OM_GEN_MODEL_GENERATOR_HPP
