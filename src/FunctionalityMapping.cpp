#include "FunctionalityMapping.hpp"
#include <sstream>
#include <algorithm>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include "Algebra.hpp"

namespace moreorg {

FunctionalityMapping::FunctionalityMapping()
{}

FunctionalityMapping::FunctionalityMapping(const ModelPool& modelPool,
        const owlapi::model::IRIList& functionalities,
        const ModelPool& functionalSaturationBound)
    : mModelPool(modelPool)
    , mFunctionalities(functionalities)
    , mFunctionalSaturationBound(functionalSaturationBound)
{
    owlapi::model::IRIList::const_iterator cit = mFunctionalities.begin();
    for(; cit != mFunctionalities.end(); ++cit)
    {
        // initialize the set of functionalities
        mFunction2Pool[*cit] = ModelPool::Set();
    }
}

const ModelPool::Set& FunctionalityMapping::getModelPools(const owlapi::model::IRI& iri) const
{
    Function2PoolMap::const_iterator cit = mFunction2Pool.find(iri);
    if(cit != mFunction2Pool.end())
    {
        return cit->second;
    } else {
        throw std::invalid_argument("moreorg::FunctionalityMapping::getModelPools: could not find"
                " model pools with function: " + iri.toString());
    }
}

owlapi::model::IRIList FunctionalityMapping::getFunctionalities(const ModelPool& pool) const
{
    owlapi::model::IRIList functions;
    for(const Function2PoolMap::value_type& p : mFunction2Pool)
    {
        bool supported = false;
        for(const ModelPool& fPool : p.second)
        {
            ModelPoolDelta delta = Algebra::substract(fPool, pool);
            if(Algebra::isSubset(fPool, pool))
            {
                supported = true;
                break;
            }
        }
        if(supported)
        {
            functions.push_back(p.first);
        }
    }
    return functions;
}

void FunctionalityMapping::add(const ModelPool& modelPool, const owlapi::model::IRI& function)
{
    if(!modelPool.empty())
    {
        mFunction2Pool[function].insert(modelPool);
        mSupportedFunctionalities.insert(function);
        mActiveModelPools.insert(modelPool);
    }
}

void FunctionalityMapping::add(const ModelPool& modelPool, const owlapi::model::IRIList& functionModels)
{
    using namespace owlapi::model;
    IRIList::const_iterator cit = functionModels.begin();
    for(; cit != functionModels.end(); ++cit)
    {
        IRI functionModel = *cit;
        add(modelPool, functionModel);
    }
}

FunctionalityMapping FunctionalityMapping::fromFile(const std::string& filename)
{
    using namespace owlapi::model;
    FunctionalityMapping functionalityMapping;

    std::ifstream mappingFile(filename);
    if(mappingFile.is_open())
    {
        std::string line;
        std::string delimiter = " ";

        ModelPool modelPool;
        while(getline(mappingFile, line))
        {
            if(line.find("#") == 0)
            {
                continue;
            }
            if(line.find("---") == 0)
            {
                break;
            }

            boost::char_separator<char> sep(" ");
            boost::tokenizer< boost::char_separator<char> > tokens(line, sep);
            std::vector<std::string> columns(tokens.begin(), tokens.end());
            if(columns.size() == 2)
            {
                IRI modelName(columns.at(0));
                size_t modelCount = boost::lexical_cast<size_t>(columns.at(1));

                modelPool[modelName] = modelCount;
            } else if(columns.size() == 1)
            {
                IRI functionalityModel = IRI(columns.at(0));
                functionalityMapping.mFunctionalities.push_back(functionalityModel);
                Function2PoolMap::const_iterator cit = functionalityMapping.mFunction2Pool.find(functionalityModel);
                if(cit == functionalityMapping.mFunction2Pool.end())
                {
                    functionalityMapping.mFunction2Pool[functionalityModel] = ModelPool::Set();
                }
            } else
            {
                if(functionalityMapping.mModelPool.empty())
                {
                    functionalityMapping.mModelPool = modelPool;
                    modelPool = ModelPool();
                } else
                {
                    functionalityMapping.mFunctionalSaturationBound = modelPool;
                    modelPool = ModelPool();
                }
            }
        }

        IRI functionalityModel;
        modelPool = ModelPool();
        while(getline(mappingFile, line))
        {
            if(line.find("#") == 0)
            {
                continue;
            }

            boost::char_separator<char> sep(" ");
            boost::tokenizer< boost::char_separator<char> > tokens(line, sep);
            std::vector<std::string> columns(tokens.begin(), tokens.end());
            if(columns.size() == 2)
            {
                IRI modelName(columns.at(0));
                size_t modelCount = boost::lexical_cast<size_t>(columns.at(1));

                modelPool[modelName] = modelCount;
            } else if(columns.size() == 1)
            {
                functionalityModel = IRI(columns.at(0));
            } else
            {
                functionalityMapping.add(modelPool, functionalityModel);
                modelPool = ModelPool();
            }
        }
    }
    return functionalityMapping;
}

void FunctionalityMapping::save(const std::string& filename) const
{
    using namespace owlapi::model;

    std::ofstream mappingFile(filename);
    if(mappingFile.is_open())
    {
        mappingFile << "# BEGIN HEADER" << std::endl;
        mappingFile << "# AVAILABLE RESOURCES" << std::endl;
        for(const auto& p : mModelPool)
        {
            mappingFile << p.first.toString() << " " << p.second << std::endl;
        }
        mappingFile << std::endl;
        mappingFile << "# AVAILABLE FUNCTIONALITIES" << std::endl;
        for(const IRI& f : mFunctionalities)
        {
            mappingFile << f.toString() << std::endl;
        }
        mappingFile << std::endl;
        mappingFile << "# SATURATION BOUND" << std::endl;
        for(const auto& p : mFunctionalSaturationBound)
        {
            mappingFile << p.first.toString() << " " << p.second << std::endl;
        }
        mappingFile << std::endl;
        mappingFile << "---" << std::endl;

        mappingFile << "# END HEADER" << std::endl;
        mappingFile << "# BEGIN MAPPING" << std::endl;
        for(const std::pair<IRI, ModelPool::Set>& m : mFunction2Pool)
        {
            if(m.second.empty())
            {
                continue;
            }

            mappingFile << "# FUNCTIONALITY" << std::endl;
            mappingFile << m.first.toString() << std::endl;
            mappingFile << "# AGENTS (MODEL POOLS)" << std::endl;
            for(const ModelPool& pool : m.second)
            {
                for(const std::pair<IRI,size_t>& resource : pool)
                {
                    mappingFile << resource.first.toString() << " " << resource.second << std::endl;
                }
                mappingFile << std::endl;
            }
            mappingFile << std::endl;
        }
        mappingFile << "# END MAPPING" << std::endl;
    }
}


std::string FunctionalityMapping::toString(uint32_t indent) const
{
    std::stringstream ss;
    std::string hspace(indent,' ');
    ss << hspace << "FunctionalityMapping:" << std::endl;
    ss << mModelPool.toString(indent) << std::endl;
    ss << hspace << "FunctionalSaturationBound -- ";
    ss << mFunctionalSaturationBound.toString(indent) << std::endl;

    {
        Function2PoolMap::const_iterator cit = mFunction2Pool.begin();
        ss << hspace << "Function --> Pool:" << std::endl;
        for(; cit != mFunction2Pool.end(); ++cit)
        {
            ss << hspace << "   - function: " << cit->first.toString() << std::endl;
            ss << hspace << "     pools: " << std::endl;

            const ModelPool::Set& modelPoolSet = cit->second;
            ss << ModelPool::toString(modelPoolSet, indent + 8) << std::endl;
        }
    }

    return ss.str();
}

} // end namespace moreorg
