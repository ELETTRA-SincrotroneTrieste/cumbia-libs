#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QThread>
#include <QLabel>

class MyLabel : public QLabel {
public:
    MyLabel(QWidget *parent);
};

class Thread : public QThread {
    Q_OBJECT
public:
    Thread(QObject *parent );

    void stop();

signals:
    void onUpdate(const QString& text);

protected:

    void run();

private:
    int m_cnt;
    bool m_stop;
};

class QthWidget : public QWidget
{
    Q_OBJECT

public:
    QthWidget(QWidget *parent = nullptr);
    ~QthWidget();

protected slots:
    void onTimeout();

signals:
    void timerTxtReady(const QString& text);

private:
    int m_cnt;
};
#endif // WIDGET_H
