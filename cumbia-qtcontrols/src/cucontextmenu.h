#ifndef INFOCONTEXTMENUFILTER_H
#define INFOCONTEXTMENUFILTER_H

#include <QObject>

class InfoContextMenuFilter : public QObject
{
    Q_OBJECT
public:
    explicit InfoContextMenuFilter(QObject *parent = nullptr);

signals:

public slots:
};

#endif // INFOCONTEXTMENUFILTER_H