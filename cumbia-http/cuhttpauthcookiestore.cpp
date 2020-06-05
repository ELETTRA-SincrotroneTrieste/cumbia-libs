#include "cuhttpauthcookiestore.h"

#include <QTextStream>
#include <QDir>
#include <QFile>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <cumacros.h>

class CuHttpAuthCookieStorePrivate {
public:
    QString errmsg;
};

CuHttpAuthCookieStore::CuHttpAuthCookieStore()
{
    d = new CuHttpAuthCookieStorePrivate;
}

CuHttpAuthCookieStore::~CuHttpAuthCookieStore()
{
    delete d;
}

bool CuHttpAuthCookieStore::save(const QByteArray &cookie) {
    d->errmsg.clear();
    QString cookiedir = QDir::homePath() + "/.cache/cumbia-http";
    QDir dir(cookiedir);
    bool ok = dir.exists();
    if(!ok)
        ok = dir.mkpath(cookiedir);
    if(ok) {
        QFile f(cookiedir + "/" + m_make_app_hash());
        ok = f.open(QIODevice::WriteOnly);
        if(!ok)
            d->errmsg = QString("CuHttpAuthCookieStore::save: failed to open file \"%1\": %2").arg(f.fileName()).arg(f.errorString());
        else {
            QTextStream out(&f);
            out << cookie;
            f.close();
        }
    }
    else {
        d->errmsg = QString("CuHttpAuthCookieStore::save: failed to create directory \"%1\"").arg(cookiedir);
    }

    return ok;
}

QByteArray CuHttpAuthCookieStore::get() const {
    QString cookiedir = QDir::homePath() + "/.cache/cumbia-http";
    QFile f(cookiedir + "/" + m_make_app_hash());
    if(f.open(QIODevice::ReadOnly)) {
        return f.readAll();
    }
    return QByteArray();
}

bool CuHttpAuthCookieStore::appHasCookie() const {
    return get().length() > 0;
}

QString CuHttpAuthCookieStore::errmsg() const {
    return  d->errmsg;
}

QByteArray CuHttpAuthCookieStore::m_make_app_hash() const
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(qApp->arguments().first().toUtf8());
    foreach(const QString& a, qApp->arguments())
        hash.addData(a.toUtf8());
    return hash.result().toHex();
}
