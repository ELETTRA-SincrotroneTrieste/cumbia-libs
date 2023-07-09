#include "cudatachecker.h"
#include <stdio.h>

CuDataChecker::CuDataChecker()
{

}

bool CuDataChecker::check() {
    printf("CuDataChecker.check \n");
    return true;
}

bool CuDataChecker::update()
{
    printf("CuDataChecker.update \n");
    return true;
}
