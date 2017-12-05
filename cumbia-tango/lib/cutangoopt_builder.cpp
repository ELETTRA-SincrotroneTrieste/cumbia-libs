#include "cutangoreadoptions.h"


CuTangoReadOptions::CuTangoReadOptions()
{
    period = 1000;
    mode = CuTReader::ChangeEventRefresh;
}

CuTangoReadOptions::CuTangoReadOptions(int per, CuTReader::RefreshMode mod)
{
    period = per;
    mode = mod;
}
