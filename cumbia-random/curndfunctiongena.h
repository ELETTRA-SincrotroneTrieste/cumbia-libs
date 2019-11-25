#ifndef CURNDFUNCTIONGENA_H
#define CURNDFUNCTIONGENA_H

#include <cudata.h>

class CuRndFData {
public:
    CuRndFData() {
        min = 0.0; // default min
        max = 1000.0; // default max
        period = 1000; // 1 second
        size = 1; // scalar
    }

    double min, max;
    int period;
    int size;
};

class CuRndFunctionGenA
{
public:
    enum Type { RandomF, SinF, FromJSFileF };

    CuRndFunctionGenA();

    virtual ~CuRndFunctionGenA();

    virtual Type getType() const = 0;

    virtual void configure(const CuData& options) = 0;

    virtual void generate(CuData& res) = 0;

    void extractConf(const CuData& res) const;

protected:
    CuRndFData *data;
};

#endif // CURNDFUNCTIONGENI_H

