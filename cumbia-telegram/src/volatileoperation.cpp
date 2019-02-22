#include "volatileoperation.h"

VolatileOperation::VolatileOperation()
{
    d_life_cnt = 1;
    m_datetime = QDateTime::currentDateTime();
}

VolatileOperation::~VolatileOperation()
{

}

/**
 * @brief VolatileOperation::lifeCount number of lives remaining for the volatile operation
 * @return an integer representing the life count of the operation
 *
 * When lifeCount returns less than zero, operation will be removed and destroyed
 * by VolatileOperations
 */
int VolatileOperation::lifeCount() const
{
    return d_life_cnt;
}

/**
 * @brief VolatileOperation::ttl time to live of a volatile operation.
 *        Default 10 minutes
 * @return 600 seconds;
 */
int VolatileOperation::ttl() const {
    return 60 * 10;
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
