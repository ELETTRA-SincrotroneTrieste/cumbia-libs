#ifndef VOLATILEOPERATIONS_H
#define VOLATILEOPERATIONS_H

#include <volatileoperation.h>
#include <QMultiMap>
#include <QObject>

/**
 * @brief The VolatileOperations class stores a map of VolatileOperation objects associated to a chat_id
 *
 * A volatile operation has a very short lifetime: it is created by the BotServer upon receiving a certain
 * message and lives until the next  or at most the next few messages.
 * This means that the next message only can use data from the volatile
 * operation that will be destroyed soon.
 * Volatile operations prevent operations from living in memory for and undefined time and for multiple
 * chats. Memory used by the cumbia-telegram server would otherwise grow in time.
 *
 * An example is Tango get_device_exported which is a search by string. The result is stored in memory by
 * a volatile operation and can be used only until the next message (for example the operation can store
 * links to get the Tango attribute list for each device but, as soon as one of the next messages does not contain
 * one of those links, the operation is consumed and the results will not be available again).
 *
 */
class VolatileOperations : public QObject
{
    Q_OBJECT
public:
    VolatileOperations();

    ~VolatileOperations();

    void addOperation(int chat_id, VolatileOperation *op);

    void replaceOperation(int chat_id, VolatileOperation* op);

    void consume(int chat_id, TBotMsgDecoder::Type t);

    VolatileOperation *get(int chat_id, int type) const;

private:
    QMultiMap<int, VolatileOperation *> m_op_map;

private slots:
    void cleanOld();
};

#endif // VOLATILEOPERATIONS_H
