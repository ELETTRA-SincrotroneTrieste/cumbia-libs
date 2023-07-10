#ifndef CUDATACHECKER_H
#define CUDATACHECKER_H
#include <QMap>
#include <QRegularExpression>
#include <QString>
#include <QList>

class CuDataChecker
{
public:
    CuDataChecker(bool debug);
    int check();
    int update();

    QMap<QString, QString> subs; // substitutions
    QString msg;

private:
    int m_process(bool rw);

    /*
     * class CuDType {
    public:
        enum Key {
            Time_us,  ///< timestamp microsecs ("timestamp_us")
            Time_ms,   ///< timestamp millis ("timestamp_ms")
            Value, ///< the value, (was "value")
            Src,  ///< source name (was "src")
    */

    QRegularExpression m_keys_re; // \s*([A-Za-z0-9_]+),\s*///<.*"(.*)".*
    QRegularExpression m_comment_re;
    QList<QRegularExpression> m_key_patterns;
    bool m_debug;
};

#endif // CUDATACHECKER_H
