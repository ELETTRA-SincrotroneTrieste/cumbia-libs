#include "curndactioni.h"

CuRNDActionI::CuRNDActionI()
{

}


void CuRNDActionI::onProgress(int step, int total, const CuData &data)
{
}

void CuRNDActionI::onResult(const CuData &data)
{
}

void CuRNDActionI::onResult(const std::vector<CuData> &datalist)
{
}

CuData CuRNDActionI::getToken() const
{
}
