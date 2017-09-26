#ifndef QUWIDGETUPDATESTRATEGYINTERFACE_H
#define QUWIDGETUPDATESTRATEGYINTERFACE_H

class QWidget;
class CuData;

class QuWidgetUpdateStrategyI
{
public:
    virtual ~QuWidgetUpdateStrategyI() {}

    virtual void update(const CuData& data, QWidget *widget) = 0;
};

#endif // QUWIDGETUPDATESTRATEGYINTERFACE_H
