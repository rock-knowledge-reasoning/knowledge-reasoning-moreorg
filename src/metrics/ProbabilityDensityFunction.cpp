#include "ProbabilityDensityFunction.hpp"

#include "../vocabularies/OMBase.hpp"
#include "pdfs/ConstantPDF.hpp"
#include "pdfs/WeibullPDF.hpp"
#include "pdfs/ExponentialPDF.hpp"

using namespace owlapi::model;

namespace moreorg {
namespace metrics {

ProbabilityDensityFunction::Ptr ProbabilityDensityFunction::getInstance(const OrganizationModelAsk& organizationModelAsk, const IRI& qualification)
{
    IRIList iriList = organizationModelAsk.ontology().allRelatedInstances(qualification, vocabulary::OM::has(), vocabulary::OMBase::ProbabilityDensityFunction());
    for (const IRI& iri : iriList)
    {
        if (organizationModelAsk.ontology().isInstanceOf(iri, vocabulary::OMBase::ConstantPDF()))
        {
            OWLAnnotationValue::Ptr annotationValue = organizationModelAsk.ontology().getAnnotationValue(iriList.front(), vocabulary::OMBase::p());
            OWLLiteral::Ptr value = annotationValue->asLiteral();      
            // LOG_DEBUG_S << "Retrieved probability of failure for '" << qualification << ": " << value->getDouble();
            return make_shared<pdfs::ConstantPDF>(value->getDouble());
        } else if (organizationModelAsk.ontology().isInstanceOf(iri, vocabulary::OMBase::WeibullPDF()))
        {
            OWLAnnotationValue::Ptr annotationValue_eta = organizationModelAsk.ontology().getAnnotationValue(iriList.front(), vocabulary::OMBase::eta());
            OWLAnnotationValue::Ptr annotationValue_beta = organizationModelAsk.ontology().getAnnotationValue(iriList.front(), vocabulary::OMBase::beta());

            OWLLiteral::Ptr value_eta = annotationValue_eta->asLiteral();
            OWLLiteral::Ptr value_beta = annotationValue_beta->asLiteral();

            // LOG_DEBUG_S << "Retrieved probability of failure for '" << qualification << ": " << value->getDouble();
            return make_shared<pdfs::WeibullPDF>(value_eta->getDouble(), value_beta->getDouble());
        } else if (organizationModelAsk.ontology().isInstanceOf(iri, vocabulary::OMBase::ExponentialPDF()))
        {
            OWLAnnotationValue::Ptr annotationValue = organizationModelAsk.ontology().getAnnotationValue(iriList.front(), vocabulary::OMBase::lambda());
            OWLLiteral::Ptr value = annotationValue->asLiteral();      
            // LOG_DEBUG_S << "Retrieved probability of failure for '" << qualification << ": " << value->getDouble();
            return make_shared<pdfs::ExponentialPDF>(value->getDouble());
        } else {
            throw std::invalid_argument("moreorg::metrics::ProbabilityDensityFunction::getInstance no probability density function instance was found for: "
                                  + iri.toString() + " and " + qualification.toString());
        }
    }

    throw std::invalid_argument("moreorg::metrics::ProbabilityDensityFunction::getInstance no related probability density function instance was found for: "
                              + qualification.toString());
}

} // namespace metrics
} // namespace moreorg

