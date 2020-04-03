#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <string>
#include <thread>
#include <mutex>

class ThreadListener {
public:
    virtual void onUpdate(const std::string& txt) = 0;
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

class Widget : public QWidget, public ThreadListener
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    Thread *th;

    // ThreadListener interface
public:
    void onUpdate(const std::string &txt);
};
#endif // WIDGET_H
