#include "curndfunctiongena.h"

CuRndFunctionGenA::CuRndFunctionGenA()
{
    data = new CuRndFData;
}

CuRndFunctionGenA::~CuRndFunctionGenA() {
    delete data;
}

void CuRndFunctionGenA::extractConf(const CuData &res) const
{
    data->min = data->max = 0.0;
    if(res["size"].isValid())
        res["size"].to<int>(data->size);
    else
        data->size = 1;

    // min max
    if(res.containsKey(CuDType::Min) && res.containsKey(CuDType::Max)) {  // res.containsKey("min"), res.containsKey("max")
        res[CuDType::Min].to<double>(data->min);  // res["min"]
        res[CuDType::Max].to<double>(data->max);  // res["max"]
    }
    if(data->min == data->max) {
        data->min = 0;
        data->max = 1000;
    }
}
