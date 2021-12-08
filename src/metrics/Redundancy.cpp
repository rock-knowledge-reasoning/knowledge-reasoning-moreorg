#include "Redundancy.hpp"
#include "../ResourceInstance.hpp"
#include "../reasoning/ResourceInstanceMatch.hpp"
#include "../Agent.hpp"
#include "../reasoning/ResourceMatch.hpp"
#include "../vocabularies/OM.hpp"
#include "../vocabularies/OMBase.hpp"
#include "DistributionFunctions.hpp"
#include "ModelSurvivability.hpp"
#include <base-logging/Logging.hpp>
#include <math.h>
#include <vector>

using namespace owlapi::model;
using namespace owlapi::vocabulary;

namespace moreorg
{
    namespace metrics
    {

        Redundancy::Redundancy(const OrganizationModelAsk &organization,
                               const ProbabilityDensityFunction::Ptr &defaultPDF,
                               const owlapi::model::IRI &objectProperty)

            : Metric(REDUNDANCY, organization, objectProperty),
              mDefaultProbabilityDensityFunction(defaultPDF)
        {
        }

        double Redundancy::computeSequential(const owlapi::model::IRIList &functions,
                                             const ResourceInstance::List &modelPool) const
        {
            std::vector<double> metrics;
            for (const IRI &function : functions)
            {
                double metric = Metric::compute(function, modelPool);
                LOG_DEBUG_S << "Function: '" << function.toString()
                            << "' --> metric: " << metric;
                metrics.push_back(metric);
            }

            // serial connection of all functions
            return serial(metrics);
        }

        double Redundancy::computeSequential(
            const std::vector<owlapi::model::IRISet> &functionalRequirement,
            const ResourceInstance::List &modelPool, bool sharedUse) const
        {
            std::vector<double> metrics;
            for (const IRISet &functions : functionalRequirement)
            {
                double metric;
                if (sharedUse)
                {
                    metric = Metric::computeSharedUse(functions, modelPool);
                }
                else
                {
                    metric = Metric::computeExclusiveUse(functions, modelPool);
                }
                LOG_DEBUG_S << "Function: '" << functions << "' --> metric: " << metric;
                metrics.push_back(metric);
            }

            // serial connection of all functions
            return serial(metrics);
        }

        double Redundancy::computeMetric(
            const std::vector<OWLCardinalityRestriction::Ptr> &required,
            const ResourceInstance::List &availableAgents, double t0,
            double t1) const
        {
            if (required.empty())
            {
                throw std::invalid_argument(
                    "moreorg::metrics::Redundancy: set of cardinality restriction to "
                    "define requirements is empty");
            }

            // Strategies to compute redundancy:
            // 1. account for relevant functionality only
            //
            // Best common redundancy:
            //  --- account for full replacements
            //   >> assign minimal resources
            //   >>   only successful full assignment, remove these resources and try
            //   again with the rest
            //
            //   >> check how often that can be done -- compute redundancy for serial
            //   system, allow parallel system being
            //
            // Best redundancy: assign resource where it contributes the most, i.e.
            //  --- system parts with lowest level of survivability gets resource first
            //  (since they will most likely fail, that should be fair enough)

            // Firstly -- we need to find a proper match of required resources to
            // available resources, thus defining here a small Constraint Satisfaction
            // Problem already
            using namespace moreorg::reasoning;
            ModelBound::List modelBoundRequired =
                ResourceInstanceMatch::toModelBoundList(required);

            ResourceInstance::List available(availableAgents);

            if (!ResourceMatch::hasMinRequirements(modelBoundRequired))
            {
                throw std::invalid_argument(
                    "moreorg::metrics::Redundancy: model bound requires minimum "
                    "requirements"
                    "to complete redundancy computation, but none are provided");
            }

            LOG_DEBUG_S << "Available: " << ResourceInstance::toString(available)
                        << std::endl
                        << "Required: " << ModelBound::toString(modelBoundRequired);

            ResourceInstanceMatch::Solution solution;
            uint32_t fullModelRedundancy = 0;
            std::map<ModelBound, ResourceInstance::List> assignments;
            try
            {
                // Check how often a full redundancy of the top level model is given
                while (true)
                {
                    solution = ResourceInstanceMatch::solve(modelBoundRequired, available,
                                                            mOrganizationModelAsk);
                    ++fullModelRedundancy;
                    for (ModelBound model : modelBoundRequired)
                    {
                        ResourceInstance::List currentAssignments = solution.getAssignments(model.model);
                        assignments[model].insert(std::end(assignments[model]), std::begin(currentAssignments), std::end(currentAssignments));
                    }
                    // Remove the consumed models from the list of available and try to
                    // repeat solving
                    // throws invalid_argument when model bounds are exceeded
                    available = solution.removeAssignmentsFromList(available);
                    LOG_DEBUG_S << "Solution: " << solution.toString() << std::endl
                                << "Remaining: " << ResourceInstance::toString(available);
                }
            }
            catch (const std::exception &e)
            {
                LOG_DEBUG_S << "ResourceMatch failed: " << e.what();
            }

            LOG_INFO_S << "Full model redundancy count is at: " << fullModelRedundancy
                       << std::endl
                       << "   remaining: "
                       << ResourceInstance::toString(available, 8);

            if (fullModelRedundancy == 0)
            {
                LOG_WARN_S << "Redundancy: the minimal resource requirements have not been "
                              "fulfilled. Redundancy cannot be computed"
                           << "available: " << ResourceInstance::toString(available, 4)
                           << "required: " << ModelBound::toString(modelBoundRequired, 4);
                throw std::runtime_error("owlapi::metrics::Redundancy: minimal resource "
                                         "requirement have not been fulfilled");
            }

            //  Find weakest spot in the model
            //  Put all resources in that match
            //  iterate until no resources are left

            std::vector<ProbabilityOfFailure> models;

            for (const auto required : modelBoundRequired)
            {
                // Mean probability of failure
                // Probability of component failure
                // default is p=0.5
                ProbabilityDensityFunction::Ptr probabilityDensityFunction;
                try
                {
                    // Model should have an associated probability of failure if not
                    // failure of parent component which be used (see punning strategy // in
                    // owlapi)
                    probabilityDensityFunction = ProbabilityDensityFunction::getInstance(
                        mOrganizationModelAsk, required.model);
                    // if(!probabilityDensityFunction)
                    // {
                    //     throw
                    //     std::invalid_argument("moreorg::metrics::Redundancy::computeMetrics
                    //     probability density function was not set!");
                    // }
                }
                catch (...)
                {

                    probabilityDensityFunction = mDefaultProbabilityDensityFunction;
                }

                // TODO replace logic above to read distribution function + parameters from
                // OM -> then init Distribution function and create PoF object
                ProbabilityOfFailure survivability(required, assignments[required], probabilityDensityFunction);
                models.push_back(survivability);
            }

            // Best model fit: redundancy
            bool updated = false;
            do
            {
                updated = false;
                // Sort based on probability of survival -- try to maximize redundancy
                std::sort(
                    models.begin(), models.end(),
                    [&t0, &t1](const ProbabilityOfFailure &a,
                               const ProbabilityOfFailure &b)
                    {
                        return a.getProbabilityOfSurvivalConditionalWithRedundancy(t0, t1) <
                               b.getProbabilityOfSurvivalConditionalWithRedundancy(t0, t1);
                    });

                ResourceInstance::List::iterator rit = available.begin();
                for (; rit != available.end(); ++rit)
                {
                    ResourceInstance &remaining = *rit;
                    bool hasPossibleMatch = false;

                    // Try to fit remaining resources
                    for (ProbabilityOfFailure &survivability : models)
                    {
                        // Check if model can be used to strengthen the survivability
                        if (survivability.getQualification() == remaining.getModel() ||
                            mOrganizationModelAsk.ontology().isSubClassOf(
                                remaining.getModel(), survivability.getQualification()))
                        {
                            hasPossibleMatch = true;
                            try
                            {
                                survivability.addAssignment(remaining);
                                available.erase(rit);
                                --rit;
                                updated = true;
                                break;
                            }
                            catch (...)
                            {
                                available.erase(rit);
                                --rit;
                                break;
                            }
                        }
                    }
                    if (!hasPossibleMatch)
                    {
                        available.erase(rit);
                        --rit;
                    }
                }
            } while (updated);

            // Serial model of all subcomponents --> the full system
            double fullModelSurvival = 1;
            for (const ProbabilityOfFailure &survivability : models)
            {
                LOG_INFO_S << "Probability of survival: " << survivability.toString();
                std::cout << "time 0: " << t0 << " time 1: " << t1 << " Survivability: " << survivability.getProbabilityOfSurvivalConditionalWithRedundancy(t0, t1) << " , " << survivability.getRequirement().toString() << std::endl;
                fullModelSurvival *=
                    survivability.getProbabilityOfSurvivalConditionalWithRedundancy(t0, t1);
            }

            return fullModelSurvival;
        }

        // ProbabilityOfFailure::List getSharedUseMetricModelsList(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr> &required, std::vector<owlapi::model::OWLCardinalityRestriction::Ptr> &available,
        //                                                         double t0, double t1) const
        // {
        //     if (required.empty())
        //     {
        //         throw std::invalid_argument("moreorg::metrics::Redundancy: set of cardinality restriction to "
        //                                     "define requirements is empty");
        //     }

        //     using namespace moreorg::reasoning;

        //     ModelBound::List modelBoundRemaining = ResourceMatch::toModelBoundList(available);
        //     ModelBound::List modelBoundRequired = ResourceMatch::toModelBoundList(required);

        //     if (!ResourceMatch::hasMinRequirements(modelBoundRequired))
        //     {
        //         throw std::invalid_argument(
        //             "moreorg::metrics::Redundancy: model bound requires minimum "
        //             "requirements"
        //             "to complete redundancy computation, but none are provided");
        //     }

        //     LOG_DEBUG_S << "Available: " << ModelBound::toString(modelBoundRemaining)
        //                 << std::endl
        //                 << "Required: " << ModelBound::toString(modelBoundRequired);

        //     ResourceMatch::Solution solution;
        //     uint32_t fullModelRedundancy = 0;
        //     std::map<ModelBound, ModelBound::List> modelAssignmentsMap;
        //     try
        //     {

        //         // Check how often a full redundancy of the top level model is given
        //         while (true)
        //         {
        //             solution = ResourceMatch::solve(modelBoundRequired, modelBoundRemaining,
        //                                             mOrganizationModelAsk);
        //             ++fullModelRedundancy;
        //             // Remove the consumed models from the list of available and try to
        //             // repeat solving
        //             // throws invalid_argument when model bounds are exceeded
        //             modelBoundRemaining = solution.substractMinFrom(modelBoundRemaining);
        //             std::map<ModelBound, ModelBound::List> solutionModelAssignmentsMap =
        //                 solution.getAssignmentsMap();
        //             for (auto modelBoundPair : solutionModelAssignmentsMap)
        //             {
        //                 auto it = std::find_if(
        //                     modelAssignmentsMap.begin(), modelAssignmentsMap.end(),
        //                     [&modelBoundPair](
        //                         const std::pair<ModelBound, ModelBound::List> &pair)
        //                     {
        //                         return modelBoundPair.first.model == pair.first.model;
        //                     });
        //                 if (it == modelAssignmentsMap.end())
        //                 {
        //                     modelAssignmentsMap.insert(modelBoundPair);
        //                 }
        //                 else
        //                 {
        //                     it->second.insert(std::end(it->second),
        //                                       std::begin(modelBoundPair.second),
        //                                       std::end(modelBoundPair.second));
        //                 }
        //             }
        //             LOG_DEBUG_S << "Solution: " << solution.toString() << std::endl
        //                         << "Remaining: " << ModelBound::toString(modelBoundRemaining);
        //         }
        //     }
        //     catch (const std::exception &e)
        //     {
        //         LOG_DEBUG_S << "ResourceMatch failed: " << e.what();
        //     }

        //     LOG_INFO_S << "Full model redundancy count is at: " << fullModelRedundancy
        //                << std::endl
        //                << "   remaining: "
        //                << ModelBound::toString(modelBoundRemaining, 8);

        //     if (fullModelRedundancy == 0)
        //     {
        //         LOG_WARN_S << "Redundancy: the minimal resource requirements have not been "
        //                       "fulfilled. Redundancy cannot be computed"
        //                    << "available: " << ModelBound::toString(modelBoundRemaining, 4)
        //                    << "required: " << ModelBound::toString(modelBoundRequired, 4);
        //         throw std::runtime_error("owlapi::metrics::Redundancy: minimal resource "
        //                                  "requirement have not been fulfilled");
        //     }

        //     //  Find weakest spot in the model
        //     //  Put all resources in that match
        //     //  iterate until no resources are left

        //     ProbabilityOfFailure::List models;

        //     for (const OWLCardinalityRestriction::Ptr &cRestriction : required)
        //     {
        //         OWLObjectCardinalityRestriction::Ptr restriction =
        //             dynamic_pointer_cast<OWLObjectCardinalityRestriction>(cRestriction);
        //         if (!restriction)
        //         {
        //             throw std::runtime_error(
        //                 "moreorg::metrics::Redundancy::computeSequential:"
        //                 " expected OWLObjectCardinalityRestriction");
        //         }
        //         IRI qualification = restriction->getQualification();

        //         // Mean probability of failure
        //         // Probability of component failure
        //         // default is p=0.5
        //         ProbabilityDensityFunction::Ptr probabilityDensityFunction;
        //         try
        //         {
        //             // Model should have an associated probability of failure if not
        //             // failure of parent component which be used (see punning strategy // in
        //             // owlapi)
        //             probabilityDensityFunction = ProbabilityDensityFunction::getInstance(
        //                 mOrganizationModelAsk, qualification);
        //             // if(!probabilityDensityFunction)
        //             // {
        //             //     throw
        //             //     std::invalid_argument("moreorg::metrics::Redundancy::computeMetrics
        //             //     probability density function was not set!");
        //             // }
        //         }
        //         catch (...)
        //         {
        //             probabilityDensityFunction = mDefaultProbabilityDensityFunction;
        //         }

        //         auto it = std::find_if(
        //             modelAssignmentsMap.begin(), modelAssignmentsMap.end(),
        //             [&qualification](
        //                 const std::pair<ModelBound, ModelBound::List> &pair)
        //             {
        //                 return modelBoundPair.first.model == qualification;
        //             });
        //         if (it == modelAssignmentsMap.end())
        //         {
        //             throw std::runtime_error(
        //                 "moreorg::metrics::Redundancy::getSharedUseMetricModelsList:"
        //                 " could not find requirement in Assignments map");
        //         }
        //         else
        //         {
        //             ProbabilityOfFailure survivability(restriction, probabilityDensityFunction, fullModelRedundancy, it->second);
        //             models.push_back(survivability);
        //         }

        //         // TODO replace logic above to read distribution function + parameters from
        //         // OM -> then init Distribution function and create PoF object
        //     }

        //     // Best model fit: redundancy
        //     bool updated = false;
        //     do
        //     {
        //         updated = false;
        //         // Sort based on probability of survival -- try to maximize redundancy
        //         std::sort(
        //             models.begin(), models.end(),
        //             [&t0, &t1](const ProbabilityOfFailure &a,
        //                        const ProbabilityOfFailure &b)
        //             {
        //                 return a.getProbabilityOfSurvivalConditionalWithRedundancy(t0, t1) <
        //                        b.getProbabilityOfSurvivalConditionalWithRedundancy(t0, t1);
        //             });

        //         ModelBound::List::iterator rit = modelBoundRemaining.begin();
        //         for (; rit != modelBoundRemaining.end(); ++rit)
        //         {
        //             ModelBound &remaining = *rit;
        //             bool hasPossibleMatch = false;

        //             // Try to fit remaining resources
        //             for (ProbabilityOfFailure &survivability : models)
        //             {
        //                 // Check if model can be used to strengthen the survivability
        //                 if (survivability.getQualification() == remaining.model ||
        //                     mOrganizationModelAsk.ontology().isSubClassOf(
        //                         remaining.model, survivability.getQualification()))
        //                 {
        //                     hasPossibleMatch = true;
        //                     try
        //                     {
        //                         remaining.decrement();
        //                         survivability.addModelBoundAssignment(remaining);
        //                         survivability.increment();
        //                         updated = true;
        //                     }
        //                     catch (...)
        //                     {
        //                         modelBoundRemaining.erase(rit);
        //                         --rit;
        //                         break;
        //                     }
        //                 }
        //             }
        //             if (!hasPossibleMatch)
        //             {
        //                 modelBoundRemaining.erase(rit);
        //                 --rit;
        //             }
        //         }
        //     } while (updated);

        //     return models;
        // }

        double Redundancy::parallel(const std::vector<double> &probabilities)
        {
            double probability = 1.0;
            for (const double &p : probabilities)
            {
                probability *= (1 - p);
            }

            return 1 - probability;
        }

        double Redundancy::serial(const std::vector<double> &probabilities)
        {
            double probability = 1.0;
            for (const double &p : probabilities)
            {
                probability *= p;
            }
            return probability;
        }

    } // end namespace metrics
} // end namespace moreorg
