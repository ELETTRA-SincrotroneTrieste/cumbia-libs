#include "stdthread_widget.h"
#include <thread>
#include <mutex>
#include <chrono>

#include <QGridLayout>
#include <QLabel>

Label::Label(QWidget *parent) : QLabel(parent)
{

}

void Label::onUpdate(int val)
{
    printf("calling label set text\n");
    QLabel::setText(QString::number(val));
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *lo = new QGridLayout(this);
    Label *la = new Label(this);
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

void Widget::onUpdate(int val)
{
    findChild<Label *>()->onUpdate(val);
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
        lis->onUpdate(cnt);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Thread::exit()
{
    _exit = true;
    th->join();
}

