#ifndef CUMBIAFORMULAS_H
#define CUMBIAFORMULAS_H

#include <cumbia.h>

class CumbiaFormula : public Cumbia
{
public:
    enum Type { CumbiaFormulasType = 206 };

    CumbiaFormula();

    virtual ~CumbiaFormula();

    // Cumbia interface
public:
    int getType() const;
};

#endif // CUMBIAFORMULAS_H
