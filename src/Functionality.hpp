#ifndef ORGANIZATION_MODEL_FUNCTIONALITY_HPP
#define ORGANIZATION_MODEL_FUNCTIONALITY_HPP

#include <owlapi/model/IRI.hpp>

namespace organization_model {

class Functionality
{
public:
    typedef std::vector<Functionality> List;
    typedef std::set<Functionality> Set;

    Functionality(const owlapi::model::IRI& model);

    virtual ~Functionality() {}

    const owlapi::model::IRI& getModel() const { return mModel; }

    bool operator<(const Functionality& other) const;

    /**
     * Convert list of models to functionality set
     */
    static Set toFunctionalitySet(const owlapi::model::IRIList& model);

    /**
     * Stringify functionality
     * \return string representing the corresponding model
     */
    std::string toString() const { return mModel.getFragment(); }

    /**
     * Stringify functionality set
     * \return string representing the set: "[<f0>, <f1>, ..., <fn>]"
     */
    static std::string toString(const Set& set);

private:
    owlapi::model::IRI mModel;
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_FUNCTIONALITY_HPP

