#ifndef CUHISTORICALDBPLUGIN_I_H
#define CUHISTORICALDBPLUGIN_I_H

#include <vector>
#include <string>
#include <cucontrolsfactories_i.h>

class Cumbia;

class CuHdbPlugin_I {
public:
    virtual ~CuHdbPlugin_I() {}

    virtual Cumbia* getCumbia() const = 0;

    virtual std::vector<std::string> getSrcPatterns() const = 0;

    virtual void setDbProfile(const QString &profile_name) = 0;

    virtual CuControlsReaderFactoryI *getReaderFactory() const = 0;

};

#define CuHdbPlugin_I_iid "eu.elettra.qutils.CuHdbPlugin_I"

Q_DECLARE_INTERFACE(CuHdbPlugin_I, CuHdbPlugin_I_iid)


#endif // CUHISTORICALDBPLUGIN_I_H
