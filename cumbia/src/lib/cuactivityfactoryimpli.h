#ifndef CUACTIVITYFACTORYIMPLI_H
#define CUACTIVITYFACTORYIMPLI_H

class CuActivity;

class CuActivityFactoryImplI
{
public:

    virtual CuActivity *createActivity() const = 0;
};

#endif // QUACTIVITYFACTORYIMPLI_H
