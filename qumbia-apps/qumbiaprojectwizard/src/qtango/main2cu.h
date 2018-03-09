#ifndef MAIN2CU_H
#define MAIN2CU_H

#include <QString>
#include <QFile>
#include <QMap>

class FindReplace;

class Main2Cu
{
public:
    Main2Cu(const QString& fname);

    Main2Cu();

    ~Main2Cu();

    void setFileName(const QString& fname);

    bool findMainWidget(const QString& classnam);

    QMap<QString, QString> parseProps();

    bool error() const;

    QString errorMessage() const;

    QString mainWidgetVar() const;

    bool mainWidgetInHeap() const;

    bool convert(const QString& mainwidget, const QString& mainwidgetvar, FindReplace &cdefs);

    QString converted();

private:
    QString m_filenam;

    QString m_errMsg;
    bool m_error, m_w_inHeap;
    QString m_mainw, m_converted;
};

#endif // MAIN2CU_H
