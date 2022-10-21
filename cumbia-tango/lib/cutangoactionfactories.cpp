#include "cutangoactionfactories.h"
#include "cutreader.h"
#include "cutwriter.h"
#include "cutconfiguration.h"
#include "cutadb.h"
#include <cumacros.h>
#include <tango/tango.h>

/*! \brief set the options on the reader factory in order to configure the actual reader
 *  within create.
 *
 * @param o a CuData compiled with the following options:
 * \li "period" an integer, in milliseconds, for the polling period. Converted with: CuVariant::toInt
 * \li "refresh_mode": an integer defining the Tango refresh mode. Converted with: CuVariant::toInt
 *
 * \par note
 * Please use the CuTangoOptBuilder class rather than filling in the options manually.
 *
 * @see CuTangoOptBuilder
 */
void CuTangoReaderFactory::setOptions(const CuData &o) {
    options = o;
}

void CuTangoReaderFactory::setTag(const CuData &t) {
    tag = t;
}

CuTangoReaderFactory::~CuTangoReaderFactory() { }

/** \brief creates and returns a CuTReader, (that implements the CuTangoActionI interface)
 *
 * @param s a string with the name of the source
 * @param ct a pointer to CumbiaTango
 * @return a CuTReader, that implements the CuTangoActionI interface
 *
 * The source and the CumbiaTango reference are passed to the CuTReader
 * If options have been set, normally through CuTangoReaderFactory::create,
 * they are used to configure the CuTReader.
 *
 * \par options
 * The recognised options are:
 * \li "period" an integer, in milliseconds, for the polling period. Converted with: CuVariant::toInt
 * \li "refresh_mode": an integer defining the Tango refresh mode. Converted with: CuVariant::toInt
 *
 * \par note
 * Please use the CuTangoOptBuilder class rather than filling in the options manually.
 *
 * @see CuTangoOptBuilder
 */
CuTangoActionI *CuTangoReaderFactory::create(const std::string &s, CumbiaTango *ct) const {
    CuTReader* reader = new CuTReader(s, ct, options, tag);
    return reader;
}

CuTangoActionI::Type CuTangoReaderFactory::getType() const {
    return CuTangoActionI::Reader;
}

CuTangoWriterFactory::~CuTangoWriterFactory() { }

void CuTangoWriterFactory::setOptions(const CuData &o) {
    options = o;
}

void CuTangoWriterFactory::setTag(const CuData &t) {
    m_tag = t;
}

void CuTangoWriterFactory::setWriteValue(const CuVariant &write_val) {
    m_write_val = write_val;
}

void CuTangoWriterFactory::setConfiguration(const CuData &configuration) {
    m_configuration = configuration;
}

CuTangoActionI *CuTangoWriterFactory::create(const std::string &s, CumbiaTango *ct) const {
    CuTWriter *w = new CuTWriter(s, ct, m_configuration, options, m_tag);
    w->setWriteValue(m_write_val);
    return w;
}

CuTangoActionI::Type CuTangoWriterFactory::getType() const {
    return CuTangoActionI::Writer;
}

// Configuration base class
//
CuTConfFactoryBase::~CuTConfFactoryBase() {

}

void CuTConfFactoryBase::setOptions(const CuData &options) {
    opts = options;
}

void CuTConfFactoryBase::setTag(const CuData &tag)  {
    dtag = tag;
}

CuData CuTConfFactoryBase::options() const {
    return opts;
}

CuData  CuTConfFactoryBase::tag() const {
    return dtag;
}

// Configuration: Reader
CuTangoActionI *CuTReaderConfFactory::create(const std::string &s, CumbiaTango *ct) const {
    return new CuTConfiguration(s, ct, CuTangoActionI::ReaderConfig, opts, dtag, nullptr);
}

CuTangoActionI::Type CuTReaderConfFactory::getType() const {
    return CuTangoActionI::ReaderConfig;
}

// Configuration: Writer
CuTangoActionI *CuTWriterConfFactory::create(const string &s, CumbiaTango *ct) const {
    CuTConfiguration *w = new CuTConfiguration(s, ct, CuTangoActionI::WriterConfig, opts, dtag, nullptr);
    if(opts.containsKey("fetch_props"))
        w->setDesiredAttributeProperties(opts["fetch_props"].toStringVector());
    return w;
}

CuTangoActionI::Type CuTWriterConfFactory::getType() const {
    return CuTangoActionI::WriterConfig;
}

class CuTaDbFactoryPrivate {
public:
    CuData o, tag;
};

CuTaDbFactory::CuTaDbFactory() {
    d = new CuTaDbFactoryPrivate;
}

CuTaDbFactory::~CuTaDbFactory() {
    delete d;
}

CuTangoActionI *CuTaDbFactory::create(const string &s, CumbiaTango *ct) const {
    return new CuTaDb(s, ct, d->o, d->tag);
}

CuTangoActionI::Type CuTaDbFactory::getType() const {
    return CuTangoActionI::TaDb;
}

void CuTaDbFactory::setOptions(const CuData &o) {
    d->o = o;
}

void CuTaDbFactory::setTag(const CuData &t) {
    d->tag = t;
}

CuData CuTaDbFactory::options() const {
    return d->o;
}
