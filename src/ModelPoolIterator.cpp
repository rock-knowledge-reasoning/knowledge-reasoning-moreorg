#include "ModelPoolIterator.hpp"
#include "Algebra.hpp"

namespace moreorg {

ModelPoolIterator::ModelPoolIterator(const ModelPool& start,
                                     const ModelPool& end,
                                     const ModelPool& stepSize)
    : mStart(start)
    , mCurrent(start)
    , mEnd(end)
    , mStepSize(stepSize)
    , mInitialized(false)
{
}

bool ModelPoolIterator::next()
{
    ModelPoolDelta delta = Algebra::delta(mCurrent, mEnd);
    if(delta.isNegative())
    {
        return false;
    }
    if(!mInitialized)
    {
        mInitialized = true;
        return true;
    }

    for(const ModelPool::value_type& v : mCurrent)
    {
        bool success = increment(v.first);
        if(success)
        {
            return true;
        } else
        {
            reset(v.first);
        }
    }
    return false;
}

bool ModelPoolIterator::increment(const owlapi::model::IRI& model)
{
    bool exhausted = true;
    size_t& currentValue = mCurrent[model];
    size_t maxValue = mEnd[model];
    // increment when maximum of this model has not yet been reached
    if(currentValue < maxValue)
    {
        // account for stepsize
        ModelPool::iterator sit = mStepSize.find(model);
        if(sit != mStepSize.end())
        {
            currentValue += sit->second;
        } else
        {
            ++currentValue;
        }
        currentValue = std::min(currentValue, maxValue);
        exhausted = false;
    }

    return !exhausted;
}

void ModelPoolIterator::reset(const owlapi::model::IRI& model)
{
    mCurrent[model] = mStart[model];
}

} // namespace moreorg
