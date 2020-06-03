#ifndef CODEEXTRACTORS_H
#define CODEEXTRACTORS_H

#include "cumbiacodeinjectcmd.h"

class ExtractorHelper
{
public:
    QList<Section> extractCumbiaPoolSections(const QString &s);
    QString newline_wrap(const QString& s);
};

class MainCppCodeExtractor : public CodeExtractorA
{
    // CodeExtractor interface
public:
    virtual CodeExtractorA::Type type() const;
    virtual QList<Section> extract(CodeExtractorA::Type t);
};

class MainWidgetCppCodeExtractor : public CodeExtractorA
{

    // CodeExtractorA interface
public:
    virtual CodeExtractorA::Type type() const;
    virtual QList<Section> extract(CodeExtractorA::Type t);
};

class MainWidgetHCodeExtractor : public CodeExtractorA
{
    // CodeExtractorA interface
public:
    virtual CodeExtractorA::Type type() const;
    virtual QList<Section> extract(CodeExtractorA::Type t);
};



#endif // CODEEXTRACTORS_H
