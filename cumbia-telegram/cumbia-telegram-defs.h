#ifndef CUMBIATELEGRAMDEFS_H
#define CUMBIATELEGRAMDEFS_H

#define LOCALSERVER_NAME "/tmp/cumbia-telegram-bot-ctrl.local"

#define CTRLTYPENAMELEN 32

class ControlMsg {
public:
    enum Type { Invalid = 0, Authorized, AuthRevoked, Statistics, MaxType = 64 };

    const char types[MaxType][CTRLTYPENAMELEN] = { "Invalid", "Authorized", "AuthRevoked", "Statistics" };
};

#endif // CUMBIATELEGRAMDEFS_H
