#include "cuwsactioni.h"

CuWSActionI::CuWSActionI()
{

}


void CuWSActionI::onProgress(int step, int total, const CuData &data)
{
}

void CuWSActionI::onResult(const CuData &data)
{
}

void CuWSActionI::onResult(const std::vector<CuData> &datalist)
{
}

CuData CuWSActionI::getToken() const
{
}
