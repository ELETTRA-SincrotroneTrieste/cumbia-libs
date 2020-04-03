#include "stdthread_widget.h"
#include <thread>
#include <mutex>
#include <chrono>

#include <QGridLayout>
#include <QLabel>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *lo = new QGridLayout(this);
    QLabel *la = new QLabel(this);
    lo->addWidget(la, 0, 0, 1, 1);
    th = new Thread(this);
    resize(200, 150);
}

Widget::~Widget()
{
    printf("Widget::~Widget: waiting for thread..."); fflush(stdout);
    th->exit();
    printf("done\n");
}

void Widget::onUpdate(const std::string &txt)
{
    findChild<QLabel *>()->setText(txt.c_str());
}


Thread::Thread(ThreadListener *l)
{
    lis = l;
    cnt = 0;
    _exit = false;
    th = new std::thread(&Thread::run, this);
}

void Thread::run()
{
    while(!_exit) {
        std::unique_lock<std::mutex> condvar_lock(m_mut);
        cnt++;
        printf("incrementing value: %d\n", cnt);
        lis->onUpdate(std::to_string(cnt));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Thread::exit()
{
    _exit = true;
    th->join();
}
