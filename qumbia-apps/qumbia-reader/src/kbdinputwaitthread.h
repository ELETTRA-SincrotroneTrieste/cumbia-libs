#ifndef KBDINPUTWAITTHREAD_H
#define KBDINPUTWAITTHREAD_H

#include <QThread>
#include <termios.h>

class KbdInputWaitThread : public QThread
{
    Q_OBJECT
public:
    KbdInputWaitThread(QObject* parent);

    ~KbdInputWaitThread();

    // QThread interface
protected:
    void run();

private:
    struct termios m_default_tio;
};

#endif // KBDINPUTWAITTHREAD_H
