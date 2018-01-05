#ifndef PROPERTYREADER_H
#define PROPERTYREADER_H

#include <cudatalistener.h>
#include <vector>
#include <string>

class CumbiaTango;

class PropertyReader : public CuDataListener
{
public:
    PropertyReader();

    void get(const char* id, const std::vector<std::string>& props);

    // CuDataListener interface
public:
    void onUpdate(const CuData &data);

private:
    CumbiaTango* m_ct;
};

#endif // PROPERTYREADER_H
