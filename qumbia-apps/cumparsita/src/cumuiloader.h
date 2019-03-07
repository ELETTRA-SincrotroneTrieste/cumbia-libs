#ifndef CUMUILOADER_H
#define CUMUILOADER_H

#include <QUiLoader>

class CumUiLoader : public QUiLoader
{
    Q_OBJECT
public:
    CumUiLoader(QObject *parent = nullptr);

    virtual ~CumUiLoader();

    // QUiLoader interface
public:
    QWidget *createWidget(const QString &className, QWidget *parent, const QString &name);
};

#endif // CUMUILOADER_H
