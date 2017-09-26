#ifndef CUCONTROLSWRITERFACTORYI_H
#define CUCONTROLSWRITERFACTORYI_H

class CuControlsWriterA;
class CuControlsReaderA;
class Cumbia;
class CuDataListener;

class CuControlsWriterFactoryI
{
public:
    virtual ~CuControlsWriterFactoryI() {}

    virtual CuControlsWriterA *create(Cumbia *, CuDataListener *) const = 0;

    virtual CuControlsWriterFactoryI* clone() const = 0;
};

class CuControlsReaderFactoryI
{
public:
    virtual ~CuControlsReaderFactoryI() {}

    virtual CuControlsReaderA *create(Cumbia *, CuDataListener *) const = 0;

    virtual CuControlsReaderFactoryI* clone() const = 0;
};


#endif // CUCONTROLSWRITERFACTORYI_H
