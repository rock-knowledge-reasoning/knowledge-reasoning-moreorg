#include "ModelBound.hpp"
#include <sstream>
#include <limits>
#include <base/Logging.hpp>
#include <gecode/int.hh>
#include <algorithm>

namespace organization_model {
namespace reasoning {

uint32_t ModelBound::MAX_THRESHOLD = 1E06;

ModelBound::ModelBound()
    : model()
    , min(0)
    , max(MAX_THRESHOLD)
{}

ModelBound::ModelBound(const owlapi::model::IRI& model, uint32_t min, uint32_t max)
    : model(model)
    , min(min)
    , max(std::min(max, MAX_THRESHOLD))
{}

uint32_t ModelBound::getMaxResourceCount(const List& boundList)
{
    ModelBound::List::const_iterator cit = boundList.begin();
    uint32_t max = 0;
    for(; cit != boundList.end(); ++cit)
    {
        const ModelBound& bound = *cit;
        if(bound.max > max)
        {
            // Limit to Gecode limit
            max = std::min(bound.max, MAX_THRESHOLD);
        }
    }
    return max;
}

std::string ModelBound::toString(size_t indent) const
{
    std::stringstream ss;
    ss << std::string(indent,' ');
    ss << "ModelBound: '" << model.toString() << "' ";
    ss << "(" << min << ", " << max << ")";
    return ss.str();
}

std::string ModelBound::toString(const List& boundList, size_t indent)
{
    std::stringstream ss;
    std::string hspace(indent,' ');
    ss << hspace;
    ss << "ModelBound::List" << std::endl;
    ModelBound::List::const_iterator cit = boundList.begin();
    for(; cit != boundList.end(); ++cit)
    {
        const ModelBound& bound = *cit;
        ss << hspace;
        ss << "    " << bound.toString() << std::endl;
    }
    return ss.str();
}

ModelBound ModelBound::substractMin(const ModelBound& other) const
{
    if(other.model != model)
    {
        LOG_DEBUG_S << "models different";
        throw std::invalid_argument("owlapi::csp::ModelBound::substractMin: models are different");
    }

    if(max < other.min)
    {
        LOG_DEBUG_S << "min value greater max: " << other.min << " vs. " << max;
        throw std::invalid_argument("owlapi::csp::ModelBound::substractMin: other model with greater min value "
                " than this max: '" + this->toString() + "' vs '" + other.toString() + "'");
    }

    return ModelBound(model, std::max(0, (int) (min - other.min)), std::max(0, (int) (max - other.min)));
}

ModelBound::List ModelBound::substractMin(const ModelBound::List& _a, const ModelBound::List& b, bool removeNegative)
{
    ModelBound::List a = _a;
    ModelBound::List result;
    ModelBound::List::const_iterator bit = b.begin();
    for(; bit != b.end(); ++bit)
    {
        const ModelBound& modelBound = *bit;
        ModelBound::List::iterator ait = std::find_if(a.begin(), a.end(), [&modelBound](const ModelBound& m)
                {
                    return m.model == modelBound.model;
                });

        if(ait == a.end())
        {
            LOG_ERROR_S << "Lval: '" << modelBound.model << "' not found";
            throw std::invalid_argument("owlapi::csp::ModelBound::substractMin: model '"
                        + modelBound.model.toString() + "' not found in lval list: " + ModelBound::toString(a));
        }

        try {
            LOG_DEBUG_S << "SubstractMin: lval: a " << ait->toString() << 
                " rval: b " << modelBound.toString();
            ModelBound deltaBound = ait->substractMin(modelBound);
            result.push_back(deltaBound);
        } catch(const std::invalid_argument &e)
        {
            if(!removeNegative)
            {
                throw;
            }
        }

        a.erase(ait);
    }

    // Add remaining entries
    result.insert(result.end(), a.begin(), a.end());

    return result;
}

void ModelBound::decrement()
{
    if(min > 0 && max > 0)
    {
        --min;
        --max;
    } else {
        throw std::runtime_error("owlapi::csp::ModelBound::decrement: cannot further decrement 0 value");
    }
}

} // end namespace reasoning
} // end namespace organization_model
