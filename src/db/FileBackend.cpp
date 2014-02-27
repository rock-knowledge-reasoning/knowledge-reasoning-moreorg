#include "FileBackend.hpp"

namespace owl_om {
namespace db {

FileBackend::FileBackend(const std::string& filename, const std::string& baseUri)
    : mFilename(filename)
    , mBaseUri(baseUri)
{}
} // end namespace db
} // end namespace owl_om
