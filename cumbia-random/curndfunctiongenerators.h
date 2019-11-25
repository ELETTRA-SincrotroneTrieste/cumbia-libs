#ifndef CURNDFUNCTIONGENERATORS_H
#define CURNDFUNCTIONGENERATORS_H

#include <curndfunctiongena.h>
#include <QString>

class CuRndRandomFunctionGen : public CuRndFunctionGenA
{
public:
    // CuRndFunctionGenI interface
public:

    void configure(const CuData& options);

    void generate(CuData &res);

    Type getType() const;
};


class CuRndSinFunctionGen : public CuRndFunctionGenA
{
public:
    // CuRndFunctionGenI interface
public:
    void configure(const CuData& options);
    void generate(CuData &res);

    Type getType() const;

    CuData options;
};


class CuRndJsFunctionGen : public CuRndFunctionGenA
{
public:
    CuRndJsFunctionGen(const QString& filenam);

    // CuRndFunctionGenI interface
public:
    void configure(const CuData& options);
    void generate(CuData &res);

    Type getType() const;

private:
    QString m_jsfilenam;
};


#endif // CURNDFUNCTIONGENERATORS_H
