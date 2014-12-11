#ifndef OWLAPI_DB_SOPRANO_DB_HPP
#define OWLAPI_DB_SOPRANO_DB_HPP

#include <string>
#include <Soprano/Soprano>
#include <owl_om/owlapi/db/FileBackend.hpp>

namespace owlapi {
namespace db {

/**
 * Soprano based model reader
 * \see http://soprano.sourceforge.net/apidox/stable/index.html
 *
 */
class SopranoDB : public FileBackend
{
public:
    SopranoDB(const std::string& filename, const std::string& baseUri = "");

    static Soprano::Model* fromFile(const std::string& filename, const std::string& baseUri);

    /**
     * Query the database
     */
    query::Results query(const std::string& query, const query::Bindings& bindings) const;

private:
    Soprano::Model* mRDFModel;
};

} // end namespace db
} // end namespace owlapi
#endif // OWLAPI_DB_SOPRANO_DB_HPP
