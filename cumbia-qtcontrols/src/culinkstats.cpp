#include "culinkstats.h"

CuLinkStats::CuLinkStats(int errorHistoryBufSiz)
{
    op_cnt = m_err_cnt = 0;
    m_err_history_size = errorHistoryBufSiz;
}

void CuLinkStats::addOperation()
{
    if(op_cnt == 0)
        m_connectionDateTime = QDateTime::currentDateTime();
    op_cnt++;
}

int CuLinkStats::opCnt() const
{
    return op_cnt;
}

/** \brief Returns the error count.
 *
 * Every time addError is invoked, the error counter is incremented
 *
 * @see addError
 */
int CuLinkStats::errorCnt() const
{
    return m_err_cnt;
}

/** \brief Add a date/time generated on the fly to the errorHistory circular buffer
 *
 * \li Adds a date/time generated on the fly to the errorHistory circular buffer (timestamp is
 *     not taken by the underlying link implementation)
 * \li increments the error counter
 *
 */
void CuLinkStats::addError(const std::string &message)
{
    m_err_cnt++;
    last_error_msg = message;
    while(m_error_history.size() >= m_err_history_size)
        m_error_history.removeFirst();
    m_error_history.append(QDateTime::currentDateTime());
}

/** \brief Change the error history buffer size
 *
 * @param s the new buffer size. At the (s + 1)-th error event,
 *        the oldest value is removed from the circular buffer.
 */
void CuLinkStats::setErrorHistorySize(int s) { m_err_history_size = s; }

/** \brief Returns the error history buffer size
 *
 */
int CuLinkStats::errorHistorySize() const { return m_err_history_size; }

/** \brief Returns the error history circular buffer.
 *
 * @see setErrorHistorySize
 * @see errorHistorySize
 *
 * Returns a circular buffer long <em>errorHistorySize</em>. It stores a set
 * of QDateTime values accounting for the date and time when the last
 * errors occurred.
 */
QList<QDateTime> CuLinkStats::errorHistory() const { return m_error_history; }

QDateTime CuLinkStats::connectionDateTime() const
{
    return m_connectionDateTime;
}
