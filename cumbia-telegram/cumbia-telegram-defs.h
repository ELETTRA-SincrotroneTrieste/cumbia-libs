#ifndef CUMBIATELEGRAMDEFS_H
#define CUMBIATELEGRAMDEFS_H

#define LOCALSERVER_NAME "/tmp/cumbia-telegram-bot-ctrl.local"

class ControlMsg {
public:
    enum Type { Invalid = -1, Authorized, AuthRevoked,  MaxType = 64 };
};

#endif // CUMBIATELEGRAMDEFS_H
