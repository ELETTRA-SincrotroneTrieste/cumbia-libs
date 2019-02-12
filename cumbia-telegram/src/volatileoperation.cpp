#include "volatileoperation.h"

VolatileOperation::VolatileOperation()
{
    d_life_cnt = 1;
    m_datetime = QDateTime::currentDateTime();
}

VolatileOperation::~VolatileOperation()
{

}

int VolatileOperation::lifeCount() const
{
    return d_life_cnt;
}

int VolatileOperation::ttl() const {
    printf("\e[1;35mreturning ttl of 19 for fuckin example!\e[0m\n");
    return 19;
}

QDateTime VolatileOperation::creationTime()
{
    return m_datetime;
}

QString VolatileOperation::message() const {
    return d_msg;
}

bool VolatileOperation::error() const {
    return d_error;
}
