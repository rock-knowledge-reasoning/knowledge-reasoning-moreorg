#ifndef ORGANIZATION_MODEL_SHARED_PTR_HPP
#define ORGANIZATION_MODEL_SHARED_PTR_HPP

#if __cplusplus <= 199711L
#define USE_BOOST_SHARED_PTR
#endif

#ifdef USE_BOOST_SHARED_PTR
#include <boost/function.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#else
#include <functional>
#include <memory>
#endif

namespace moreorg {
#ifdef USE_BOOST_SHARED_PTR
using ::boost::bind;
using ::boost::dynamic_pointer_cast;
using ::boost::function1;
using ::boost::make_shared;
using ::boost::shared_ptr;
using ::boost::static_pointer_cast;
namespace placeholder = ::boost;
#else
using ::std::bind;
using ::std::dynamic_pointer_cast;
using ::std::function;
using ::std::make_shared;
using ::std::shared_ptr;
using ::std::static_pointer_cast;
namespace placeholder = ::std::placeholders;
template <class T, class U> using function1 = ::std::function<T(U)>;
#endif
} // namespace moreorg

#endif // ORGANIZATION_MODEL_SHARED_PTR_HPP
