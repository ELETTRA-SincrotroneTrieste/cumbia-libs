#ifndef CUHTTPAUTHCOOKIESTORE_H
#define CUHTTPAUTHCOOKIESTORE_H

#include <QByteArray>

class CuHttpAuthCookieStorePrivate;

class CuHttpAuthCookieStore
{
public:
    CuHttpAuthCookieStore();
    ~CuHttpAuthCookieStore();

    bool save(const QByteArray& cookie);
    QByteArray get() const;
    bool appHasCookie() const;

    QString errmsg() const;

private:
    CuHttpAuthCookieStorePrivate *d;

    QByteArray m_make_app_hash() const;
};

#endif // CUHTTPAUTHCOOKIESTORE_H
