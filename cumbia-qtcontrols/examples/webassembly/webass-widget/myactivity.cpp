#include "myactivity.h"

MyActivity::MyActivity()
{
    m_cnt = 0;
}

bool MyActivity::matches(const CuData &token) const
{
    return false;
}

void MyActivity::init()
{

}

void MyActivity::execute()
{
    m_cnt++;
    publishResult(CuData("value", m_cnt));
}

void MyActivity::onExit()
{

}
