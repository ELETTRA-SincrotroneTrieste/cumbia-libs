#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <string>
#include <thread>
#include <mutex>

#include <QLabel>

class ThreadListener {
public:
    virtual void onUpdate(int val) = 0;
};

class Thread {
public:
    Thread(ThreadListener *l);
    ThreadListener *lis;

    void run();

    void exit();

    int cnt;
    bool _exit;

    std::mutex m_mut;
    std::thread * th;
};

class Label : public QLabel {
    Q_OBJECT
public: Label(QWidget *parent);

public slots:
    void onUpdate(int val);
};

class Widget : public QWidget, public ThreadListener
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    Thread *th;

    // ThreadListener interface
public:
    void onUpdate(int val);
};
#endif // WIDGET_H
