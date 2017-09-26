#include "cuvariantprivate.h"

#include <stdio.h>

CuVariantPrivate::CuVariantPrivate()
{
    val = 0; // NULL
    mIsValid = false;
    mIsNull = true;
    mSize = 0;
}
