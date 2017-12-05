#ifndef CUCONTEXTWIDGETI_H
#define CUCONTEXTWIDGETI_H

class CuContext;

class CuContextWidgetI
{
public:
    virtual ~CuContextWidgetI() {}

    virtual CuContext *getContext() const = 0;
};

#endif // CUCONTEXTWIDGETI_H
