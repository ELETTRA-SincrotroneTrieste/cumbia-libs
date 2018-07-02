#ifndef QML_GLOBAL_H
#define QML_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QML_LIBRARY)
#  define QMLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QMLSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QML_GLOBAL_H
