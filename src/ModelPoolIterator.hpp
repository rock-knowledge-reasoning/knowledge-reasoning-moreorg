#ifndef ORGANIZATION_MODEL_MODEL_POOL_ITERATOR_HPP
#define ORGANIZATION_MODEL_MODEL_POOL_ITERATOR_HPP

#include "ModelPool.hpp"

namespace moreorg {

class ModelPoolIterator
{
public:
    ModelPoolIterator(const ModelPool& start,
            const ModelPool& end,
            const ModelPool& stepSize);

    bool next();
    const ModelPool& current() const { return mCurrent; }

protected:
    /**
     * Return false if model cannot be incremented any further
     */
    bool increment(const owlapi::model::IRI& model);
    void reset(const owlapi::model::IRI& model);

private:
    ModelPool mStart;
    ModelPool mCurrent;
    ModelPool mEnd;
    ModelPool mStepSize;

    bool mInitialized;
};

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_MODEL_POOL_ITERATOR_HPP

