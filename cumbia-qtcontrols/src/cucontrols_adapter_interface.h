#ifndef CUCONTROLSADAPTERINTERFACE_H
#define CUCONTROLSADAPTERINTERFACE_H

class QWidget;
class CuData;

class CuControlsAdapterInterface
{
public:
    CuControlsAdapterInterface();

    virtual ~CuControlsAdapterInterface() {}

    virtual void onUpdate(QWidget *w, const CuData& d) = 0;
};

#endif // TANGOADAPTERINTERFACE_H
