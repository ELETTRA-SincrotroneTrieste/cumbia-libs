#ifndef CUCONTEXTI_H
#define CUCONTEXTI_H

class CuContext;

class CuContextI
{
public:
    virtual ~CuContextI() {}

    virtual CuContext *getContext() const = 0;
};

#endif // CUCONTEXTI_H
