#include "ModelGenerator.hpp"
#include <ctemplate/template.h>

namespace owl_om
{

std::map<std::string, std::string> ModelGenerator::msTemplates;

ModelGenerator::ModelGenerator(const std::string& filename, const std::string& ns)
    : mNamespace(ns)
{
    mKnowledgeBase = db::KnowledgeBase::fromFile(filename);
}

RenderedFiles ModelGenerator::renderClassFiles()
{
    if(!mKnowledgeBase)
        return RenderedFiles();

    const Registry& registry = mKnowledgeBase->getRegistry();

    Class::Map classes = registry.getAllClasses();
    Class::Map::const_iterator cit = classes.begin();

    RenderedFiles files;
    for(; cit != classes.end(); ++cit)
    {
        std::string templateFile = msTemplates["class-hpp"];
        if(templateFile.empty())
        {
            throw std::runtime_error("ModelGenerator: could not find template for 'class' -- given file is:'" + templateFile + "'"); 

        }
        RenderedFiles renderedClassFiles = render(templateFile, cit->second, mNamespace);
        files.insert(files.end(), renderedClassFiles.begin(), renderedClassFiles.end());
    }

    return files;
}

std::string ModelGenerator::generateInclude(Class::Ptr klass, const std::string& ns)
{
    return "#include <" + ns + "/" + klass->getName() + ".hpp>";
}

void ModelGenerator::registerTemplate(const std::string& name, const std::string& file)
{
    msTemplates[name] = file;
}

RenderedFiles ModelGenerator::render(const std::string& templateFile, Class::Ptr klass, const std::string& ns)
{
    // Prepare template
    ctemplate::TemplateDictionary dict("class-hpp");
    dict.SetValue("classname", klass->getName());
    dict.SetValue("beginnamespaces", "namespace " + ns + " {");
    dict.SetValue("endnamespaces", "} // end namespace " + ns);

    Class::Map ancestors = klass->getAncestors();
    Class::Map::const_iterator ait = ancestors.begin();
    std::string parentClasses;
    for(; ait != ancestors.end();)
    {
        Class::Ptr ancestorKlass = ait->second;
        ctemplate::TemplateDictionary* includesDict = dict.AddSectionDictionary("includes");
        includesDict->SetValue("include", generateInclude(ancestorKlass, ns));

        parentClasses += ancestorKlass->getName();
        if(++ait != ancestors.end())
        {
            parentClasses += ", ";
        }
    }

    dict.SetValue("parentclasses","public " + parentClasses);

    std::string output;
    ctemplate::ExpandTemplate(templateFile, ctemplate::DO_NOT_STRIP, &dict, &output);

    RenderedFiles files;
    std::string filename = klass->getName() + ".hpp";
    files.push_back( RenderedFile(filename, output) );

    return files;
}


}
