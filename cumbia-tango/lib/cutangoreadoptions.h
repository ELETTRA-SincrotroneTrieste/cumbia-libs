#ifndef CUTANGOREADOPTIONS_H
#define CUTANGOREADOPTIONS_H

#include <cutreader.h>

class CuTangoReadOptions
{
public:
    CuTangoReadOptions();

    CuTangoReadOptions(int per, CuTReader::RefreshMode mod);

    int period;
    CuTReader::RefreshMode mode;
};

#endif // CUTREADOPTIONS_H
