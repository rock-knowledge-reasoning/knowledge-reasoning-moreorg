#ifndef ORGANIZATION_MODEL_FUNCTIONALITY_HPP
#define ORGANIZATION_MODEL_FUNCTIONALITY_HPP

#include <owlapi/model/IRI.hpp>

namespace organization_model {

class Functionality
{
public:
    Functionality(const owlapi::model::IRI& model);

    virtual ~Functionality() {}

    const owlapi::model::IRI& getModel() const { return mModel; }

    bool operator<(const Functionality& other) const;

    static std::set<Functionality> toFunctionalitySet(const owlapi::model::IRIList& model);

    std::string toString() const { return mModel.getFragment(); }

private:
    owlapi::model::IRI mModel;
};

typedef std::vector<Functionality> FunctionalityList;
typedef std::set<Functionality> FunctionalitySet;

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_FUNCTIONALITY_HPP

