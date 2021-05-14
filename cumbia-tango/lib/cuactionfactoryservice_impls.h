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

    virtual CuTangoActionI* registerAction(const std::string& src, const CuTangoActionFactoryI& f, CumbiaTango *ct);
    virtual CuTangoActionI *find(const std::string &name, CuTangoActionI::Type at);
    virtual size_t count() const;
    virtual void unregisterAction(const std::string& src, CuTangoActionI::Type at);
    virtual void cleanup();

protected:
    CuActionFactoryServiceImplBasePrivate *d;
};

/*!
 * \brief The CuActionFactoryServiceImpl class is the simple *non thread safe* implementation for most cases
 * where actions are registered and unregistered from the same thread (i.e. simple applications)
 */
class CuActionFactoryServiceImpl : public CuActionFactoryServiceImpl_Base {
public:

    CuActionFactoryServiceImpl();
    virtual ~CuActionFactoryServiceImpl();

    virtual CuTangoActionI* registerAction(const std::string& src, const CuTangoActionFactoryI& f, CumbiaTango *ct);
    virtual CuTangoActionI *find(const std::string &name, CuTangoActionI::Type at);
    virtual size_t count() const;
    virtual void unregisterAction(const std::string& src, CuTangoActionI::Type at);
    virtual void cleanup();

private:
    CuActionFactoryServiceImplPrivate *d;
};

/*!
 * \brief The CuActionFactoryServiceImpl_TS class is the thread safe version of CuActionFactoryServiceImpl
 *      for cases registering and unregistering actions from different threads (i.e. complex applications)
 */
class CuActionFactoryServiceImpl_TS : public CuActionFactoryServiceImpl_Base {
public:
    CuActionFactoryServiceImpl_TS();
    virtual ~CuActionFactoryServiceImpl_TS();

    virtual CuTangoActionI* registerAction(const std::string& src, const CuTangoActionFactoryI& f, CumbiaTango *ct);
    virtual CuTangoActionI *find(const std::string &name, CuTangoActionI::Type at);
    virtual size_t count() const;
    virtual void unregisterAction(const std::string& src, CuTangoActionI::Type at);
    virtual void cleanup();

private:
    CuActionFactoryServiceImpl_R_Private *d;
};



#endif // CUACTIONFACTORYSERVICE_IMPLS_H
