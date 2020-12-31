#include "cuhttpactionfactories.h"
#include "cuhttpwritehelper.h"
#include "cuhttpactionconf.h"
#include "cumbiahttp.h"
#include "cuhttpchannelreceiver.h"

#include <QNetworkAccessManager>

class CuHttpAuthManager;
class CuHTTPClient;

class CuHTTPReaderFactoryPrivate {
public:
    CuData options;
    bool single_shot;
};


void CuHTTPActionReaderFactory::mergeOptions(const CuData &o) {
    d->options.merge(o);
}

CuHTTPActionReaderFactory::CuHTTPActionReaderFactory(bool single_shot) {
    d  = new CuHTTPReaderFactoryPrivate;
    d->single_shot = single_shot;
}

CuHTTPActionReaderFactory::~CuHTTPActionReaderFactory() {
    delete d;
}

std::string CuHTTPActionReaderFactory::getMethod() const {
    return d->single_shot ? "read" : "s";
}

CuHTTPActionFactoryI *CuHTTPActionReaderFactory::clone() const {
    CuHTTPActionReaderFactory *f = new CuHTTPActionReaderFactory(false);
    f->d->options = d->options;
    return f;
}

CuData CuHTTPActionReaderFactory::options() const {
    return d->options;
}

void CuHTTPActionWriterFactory::setConfiguration(const CuData &conf) {
    o.merge(conf);
}

void CuHTTPActionWriterFactory::setWriteValue(const CuVariant &write_val) {
    o.merge(CuData("write_val", write_val));
}

string CuHTTPActionWriterFactory::getMethod() const {
    return "write";
}

CuHTTPActionFactoryI *CuHTTPActionWriterFactory::clone() const {
    CuHTTPActionWriterFactory *wf = new CuHTTPActionWriterFactory();
    wf->o = this->o;
    return wf;
}

CuData CuHTTPActionWriterFactory::options() const {
    return o;
}

CuHTTPActionWriterFactory::~CuHTTPActionWriterFactory() { }


void CuHTTPActionConfFactory::setOptions(const CuData &o) {
    m_o = o;
}

void CuHTTPActionConfFactory::mergeOptions(const CuData &o) {
    m_o.merge(o);
}

CuHTTPActionConfFactory::~CuHTTPActionConfFactory() {

}

std::string CuHTTPActionConfFactory::getMethod() const {
    return "conf";
}

CuHTTPActionFactoryI *CuHTTPActionConfFactory::clone() const
{
    CuHTTPActionConfFactory *f = new CuHTTPActionConfFactory;
    f->m_o = this->m_o;
    return f;
}

CuData CuHTTPActionConfFactory::options() const {
    return m_o;
}


void CuHTTPActionEditFactory::setOptions(const CuData &o) {
    m_ed_o = o;
}

CuHTTPActionEditFactory::~CuHTTPActionEditFactory() {

}

std::string CuHTTPActionEditFactory::getMethod() const {
    return "edit";
}

CuHTTPActionFactoryI *CuHTTPActionEditFactory::clone() const
{
    CuHTTPActionEditFactory *f = new CuHTTPActionEditFactory;
    f->m_ed_o = this->m_ed_o;
    return f;
}

CuData CuHTTPActionEditFactory::options() const {
    return m_ed_o;
}
