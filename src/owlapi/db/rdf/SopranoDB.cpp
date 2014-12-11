#include "SopranoDB.hpp"
#include <base/Logging.hpp>
#include <fstream>

namespace owlapi {
namespace db {

SopranoDB::SopranoDB(const std::string& filename, const std::string& baseUri)
    : FileBackend(filename, baseUri)
    , mRDFModel(0)
{
    mRDFModel = fromFile(mFilename, mBaseUri);
}

Soprano::Model* SopranoDB::fromFile(const std::string& filename, const std::string& baseUri)
{
    const Soprano::Parser* p = Soprano::PluginManager::instance()->discoverParserForSerialization( Soprano::SerializationRdfXml );
    QUrl qBaseUri( QString::fromStdString(baseUri) );

    {
        std::ifstream infile(filename.c_str());
        if(! infile.good() )
        {
            std::string msg = "SopranoDB: file does not exist: '" + filename + "\n";
            throw std::runtime_error(msg);
        }
    }


    Soprano::StatementIterator it = p->parseFile(QString::fromStdString(filename), qBaseUri, Soprano::SerializationRdfXml);

    Soprano::Model* sopranoModel = Soprano::createModel();
    QList<Soprano::Statement> allStatements = it.allStatements();

    Q_FOREACH( Soprano::Statement s, allStatements)
    {
        sopranoModel->addStatement(s);
    }

    return sopranoModel;
}

query::Results SopranoDB::query(const std::string& query, const query::Bindings& bindings) const
{
    query::Results queryResults;

    Soprano::QueryResultIterator qit = mRDFModel->executeQuery(QString(query.c_str()), Soprano::Query::QueryLanguageSparql);
    while( qit.next())
    {
        query::Bindings::const_iterator bit = bindings.begin();

        query::Row row;
        for(; bit != bindings.end(); ++bit)
        {
            query::Variable binding = *bit;
            std::string boundValue = qit.binding(QString(binding.getName().c_str())).toString().toStdString();
            row[binding] = owlapi::model::IRI::create( boundValue );
        }

        queryResults.rows.push_back(row);
    }

    return queryResults;
}






//    //PREFIX rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#>
//    //PREFIX owl: <http://www.w3.org/2002/07/owl#>
//    //PREFIX xsd: <http://www.w3.org/2001/XMLSchema#>
//    //PREFIX rdfs: <http://www.w3.org/2000/01/rdf-schema#>
//    //SELECT ?subject ?object
//    //  WHERE { ?subject rdfs:subClassOf ?object }
//
//    using namespace owlapi;
//    std::string prefix = "PREFIX rdf:<" + vocabulary::RDF::BaseUri() +">\n";
//    prefix += "PREFIX owl: <" + vocabulary::OWL::BaseUri() + ">\n";
//    prefix += "PREFIX rdfs: <" + vocabulary::RDFS::BaseUri() + ">\n";
//
//    // Loading the model specification
//    Registry& registry = RDFModelFactory::getRegistry();
//    {
//        std::string query = prefix + " select ?r where { ?r rdf:type owl:Class . }";
//        Soprano::QueryResultIterator qit = sopranoModel->executeQuery(QString(query.c_str()), Soprano::Query::QueryLanguageSparql);
//        while( qit.next())
//        {
//            std::string classUri = qit.binding("r").toString().toStdString();
//            registry.registerClass( classUri, owl_om::Class::Ptr( new owl_om::Class( classUri ) ) );
//        }
//    }
//
//    // Update subclass relationships
//    {
//        std::string query = prefix + " select ?s ?o where { ?s rdfs:subClassOf ?o .}";
//        Soprano::QueryResultIterator qit = sopranoModel->executeQuery(QString(query.c_str()), Soprano::Query::QueryLanguageSparql);
//
//        while( qit.next())
//        {
//            std::string subject = qit.binding("s").toString().toStdString();
//            std::string object = qit.binding("o").toString().toStdString();
//
//            LOG_DEBUG_S << "Subject " << subject << " subclassOf " << object;
//
//            Class::Ptr subjectKlass = registry.getClass(subject);
//            Class::Ptr objectKlass = registry.getClass(object);
//            subjectKlass->addSuperClass(objectKlass);
//        }
//    }
//
//    {
//        std::string query = prefix + " select ?r where { ?r rdf:type owl:ObjectProperty . }";
//        Soprano::QueryResultIterator qit = sopranoModel->executeQuery(QString(query.c_str()), Soprano::Query::QueryLanguageSparql);
//        while( qit.next())
//        {
//            LOG_DEBUG_S << qit.binding("r").toString().toStdString();
//            std::string propertyUri = qit.binding("r").toString().toStdString();
//            registry.registerProperty( propertyUri, owl_om::Property::Ptr( new owl_om::ObjectProperty( propertyUri ) ) );
//        }
//    }
//
//    {
//        std::string query = prefix + " select ?r where { ?r rdf:type owl:DatatypeProperty . }";
//        Soprano::QueryResultIterator qit = sopranoModel->executeQuery(QString(query.c_str()), Soprano::Query::QueryLanguageSparql);
//        while( qit.next())
//        {
//            LOG_DEBUG_S << qit.binding("r").toString().toStdString();
//            std::string propertyUri = qit.binding("r").toString().toStdString();
//            registry.registerProperty( propertyUri, owl_om::Property::Ptr( new owl_om::DatatypeProperty( propertyUri ) ) );
//        }
//    }
//
//
//    // Describing the model instance
//    Model model;
//    {
//        std::string query = prefix + " select ?r where { ?r rdf:type owl:NamedIndividual . }";
//        std::vector<std::string> individuals;
//        Soprano::QueryResultIterator qit = sopranoModel->executeQuery(QString(query.c_str()), Soprano::Query::QueryLanguageSparql);
//        while( qit.next())
//        {
//            individuals.push_back( qit.binding("r").toString().toStdString() );
//        }
//
//        std::vector<std::string>::const_iterator cit = individuals.begin();
//        for(; cit != individuals.end(); ++cit)
//        {
//            owl_om::vocabulary::SplitUri splitUri = owl_om::vocabulary::Utils::extractBaseUri(*cit);
//            std::string prefixIndividual = prefix + " PREFIX custom: <" + splitUri.baseUri +">\n";
//            std::string iQuery = prefixIndividual + " select ?o where { custom:" + splitUri.name + " rdf:type ?o . }";
//
//            LOG_DEBUG_S << "iQuery: " << iQuery;
//
//            Soprano::QueryResultIterator iit = sopranoModel->executeQuery(QString(iQuery.c_str()), Soprano::Query::QueryLanguageSparql);
//            while( iit.next())
//            {
//                std::string uri = iit.binding("o").toString().toStdString();
//                if(uri != owl_om::vocabulary::OWL::NamedIndividual())
//                {
//                    OrganizationElement::Ptr element = RDFModelFactory::getInstanceOf( splitUri.fullName(), uri);
//                    model.add(element);
//                }
//            }
//        }
//    }
//
//    LOG_WARN_S << registry.toString();
//
//
//    //Q_FOREACH( Soprano::Statement s, allStatements)
//    //{
//    //    Soprano::Node subject = s.subject();
//    //    Soprano::Node predicate = s.predicate();
//    //    Soprano::Node object = s.object();
//
//    //    printf("s: %s p: %s o: %s\n", subject.toString().toStdString().c_str(),
//    //            predicate.toString().toStdString().c_str(),
//    //            object.toString().toStdString().c_str());
//
//
//    //    try {
//    //        OrganizationElement::Ptr element = RDFModelFactory::getInstanceOf(subject.toString().toStdString().c_str());
//
//    //        // How to identify individual elements i.e. id?!
//
//    //        //model.add(element);
//    //        LOG_WARN("SopranoDB: adding vertex for %s", subject.toString().toStdString().c_str());
//    //    } catch(const std::runtime_error& e)
//    //    {
//    //        LOG_DEBUG("SopranoDB: error on adding vertex %s", e.what());
//    //    }
//
//    //}
//    return model;
//}

} // end namespace db
} // end namespace owlapi
