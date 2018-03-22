#ifndef CPPINSTANTIATIONEXPAND_H
#define CPPINSTANTIATIONEXPAND_H

#include "fileprocessor_a.h"

class CppInstantiationExpand : public FileCmd
{
public:
    CppInstantiationExpand(const QString& filenam);

    // FileCmd interface
public:
    virtual QString process(const QString &input);
    virtual QString name();
};

#endif // CPPINSTANTIATIONEXPAND_H
