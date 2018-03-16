#ifndef MAIN2CU_H
#define MAIN2CU_H

#include <QString>
#include <QFile>
#include <QMap>

class ConversionDefs;

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

private:
    QString m_filenam;

    QString m_errMsg;
    bool m_error, m_w_inHeap;
    QString m_mainw;
};

#endif // MAIN2CU_H
