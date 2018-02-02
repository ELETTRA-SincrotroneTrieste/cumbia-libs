#ifndef LINKSTATS_H
#define LINKSTATS_H

#include <QList>
#include <QDateTime>
#include <string>

/** \brief Class used by cumbia-qtcontrols objects to store a minimal set of statistics
 *         about the link health.
 *
 * \par Available information
 * \li op_cnt a counter incremented at each operation (read/write/configuration)
 * \li errorCnt a method returning the total number of errors occurred
 * \li last_error_msg a std::string containing the <em>last error</em> message
 * \li errorHistory: a circular buffer whose size is determined at creation time (constructor)
 *     or changed at runtime through setErrorHistorySize. It stores QDateTime elements
 *     marking the date and time of the last errors occurred.
 *
 * To get a reference to the link statistics, call getLinkStats on the cumbia-qtcontrols object.
 *
 * This class provide minimal statistics about the connection status of the object. It is foreseen
 * that every implementation of either a reader or writer provides some basic information such as
 * error events. The date and time stored in the errorHistory is not taken from a timestamp from
 * the engine implementation. It's instead generated on the fly by addError.
 */
class CuLinkStats
{
public:
    CuLinkStats(int errorHistoryBufSiz = 10);

    void addOperation();

    int opCnt() const;

    int errorCnt() const;

    void addError(const std::string &message);

    void setErrorHistorySize(int s);

    int errorHistorySize() const;

    std::string last_error_msg;

    QList<QDateTime> errorHistory() const;

    QDateTime connectionDateTime() const;

private:
    QList<QDateTime> m_error_history;
    int m_err_history_size;
    int m_err_cnt;
    int op_cnt;   // count operations (read, write, ...)
    QDateTime m_connectionDateTime;
};

#endif // LINKSTATS_H
