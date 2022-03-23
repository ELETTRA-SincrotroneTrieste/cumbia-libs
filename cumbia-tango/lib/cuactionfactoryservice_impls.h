#ifndef CUACTIONFACTORYSERVICE_IMPLS_H
#define CUACTIONFACTORYSERVICE_IMPLS_H

#include <cuservicei.h>
#include <cutangoactioni.h>
#include <cutangoactionfactoryi.h>

class CuActionFactoryServiceImplBasePrivate;
class CuActionFactoryServiceImplPrivate;
class CuActionFactoryServiceImpl_R_Private;

class CuActionFactoryServiceImpl_Base {
public:
    enum Type { CuActionFactoryServiceType = CuServices::User + 21 };
    CuActionFactoryServiceImpl_Base();
    virtual ~CuActionFactoryServiceImpl_Base();

    virtual CuTangoActionI* registerAction(const std::string& src, const CuTangoActionFactoryI& f, CumbiaTango *ct, bool *isnew);
    virtual CuTangoActionI *find(const std::string &name, CuTangoActionI::Type at);
    virtual size_t count() const;
    virtual void unregisterAction(const std::string& src, CuTangoActionI::Type at);
    virtual void cleanup();

protected:
    CuActionFactoryServiceImplBasePrivate *d;
};

/*!
 * \brief The CuActionFactoryServiceImpl class is the default implementation.
 *
 * Functions shall be called from the same thread.
 */
class CuActionFactoryServiceImpl : public CuActionFactoryServiceImpl_Base {
public:

    CuActionFactoryServiceImpl();
    virtual ~CuActionFactoryServiceImpl();

    virtual CuTangoActionI* registerAction(const std::string& src, const CuTangoActionFactoryI& f, CumbiaTango *ct, bool *isnew);
    virtual CuTangoActionI *find(const std::string &name, CuTangoActionI::Type at);
    virtual size_t count() const;
    virtual void unregisterAction(const std::string& src, CuTangoActionI::Type at);
    virtual void cleanup();

private:
    CuActionFactoryServiceImplPrivate *d;
};


#endif // CUACTIONFACTORYSERVICE_IMPLS_H
