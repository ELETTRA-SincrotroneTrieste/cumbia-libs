#include "cuformula.h"
#include <cumacros.h>
#include <pthread.h>

CumbiaFormula::CumbiaFormula()
{

}

CumbiaFormula::~CumbiaFormula()
{
    pdelete("~CumbiaFormula %p\n", this);
}


int CumbiaFormula::getType() const
{
    return CumbiaFormulasType;
}
