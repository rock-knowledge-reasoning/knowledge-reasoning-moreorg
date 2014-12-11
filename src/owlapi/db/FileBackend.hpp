#ifndef OWLAPI_DB_FILE_BACKEND_HPP
#define OWLAPI_DB_FILE_BACKEND_HPP

#include <owl_om/owlapi/db/rdf/SparqlInterface.hpp>

namespace owlapi {
namespace db {

class FileBackend : public query::SparqlInterface
{
public:
    FileBackend(const std::string& filename, const std::string& baseUri);

    virtual ~FileBackend() {}

    virtual query::Results query(const std::string& query, const query::Bindings& bindings) { throw std::runtime_error("owl_om::FileBackend: query interface not implemented"); }

protected:
    std::string mFilename;
    std::string mBaseUri;
};

} // end namespace db
} // end namespace owlapi
#endif // OWLAPI_DB_FILE_BACKEND_HPP
