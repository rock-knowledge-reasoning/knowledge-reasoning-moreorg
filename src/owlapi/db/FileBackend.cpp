#include "FileBackend.hpp"

namespace owlapi {
namespace db {

FileBackend::FileBackend(const std::string& filename, const std::string& baseUri)
    : mFilename(filename)
    , mBaseUri(baseUri)
{}
} // end namespace db
} // end namespace owlapi
