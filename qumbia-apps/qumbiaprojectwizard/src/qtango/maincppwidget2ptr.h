#ifndef MAINCPPWIDGET2PTR_H
#define MAINCPPWIDGET2PTR_H

#include "fileprocessor_a.h"

class MainCppWidget2Ptr : public FileCmd
{
public:
    MainCppWidget2Ptr(const QString& filename);

    // FileCmd interface
public:
    virtual QString process(const QString &input);
    virtual QString name();
};

#endif // MAINCPPWIDGET2PTR_H
