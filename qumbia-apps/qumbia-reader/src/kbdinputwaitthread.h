#ifndef KBDINPUTWAITTHREAD_H
#define KBDINPUTWAITTHREAD_H

#include <QThread>

class KbdInputWaitThread : public QThread
{
    Q_OBJECT
public:
    KbdInputWaitThread(QObject* parent);

    // QThread interface
protected:
    void run();
};

#endif // KBDINPUTWAITTHREAD_H
