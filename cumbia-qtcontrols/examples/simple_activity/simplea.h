#ifndef SIMPLEA_H
#define SIMPLEA_H

#include <QWidget>
#include <cuthreadlistener.h>

class Cumbia;

class SimpleA : public QWidget, public CuThreadListener
{
    Q_OBJECT
public:
    explicit SimpleA(QWidget *parent = nullptr);

    ~SimpleA();

private slots:
    void start();

    // CuThreadListener interface
public:
    void onProgress(int step, int total, const CuData &data);
    void onResult(const CuData &data);
    CuData getToken() const;

private:
    Cumbia *m_cumbia;
    int m_cnt, m_thid;
};

#endif // SIMPLEA_H
