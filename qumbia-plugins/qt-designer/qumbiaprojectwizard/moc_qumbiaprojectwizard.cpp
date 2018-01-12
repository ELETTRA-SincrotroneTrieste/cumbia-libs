/****************************************************************************
** Meta object code from reading C++ file 'qumbiaprojectwizard.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qumbiaprojectwizard.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qumbiaprojectwizard.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QumbiaProjectWizard_t {
    QByteArrayData data[12];
    char stringdata0[131];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QumbiaProjectWizard_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QumbiaProjectWizard_t qt_meta_stringdata_QumbiaProjectWizard = {
    {
QT_MOC_LITERAL(0, 0, 19), // "QumbiaProjectWizard"
QT_MOC_LITERAL(1, 20, 4), // "init"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 6), // "create"
QT_MOC_LITERAL(4, 33, 9), // "checkText"
QT_MOC_LITERAL(5, 43, 14), // "selectLocation"
QT_MOC_LITERAL(6, 58, 11), // "addProperty"
QT_MOC_LITERAL(7, 70, 5), // "parts"
QT_MOC_LITERAL(8, 76, 14), // "removeProperty"
QT_MOC_LITERAL(9, 91, 18), // "projectNameChanged"
QT_MOC_LITERAL(10, 110, 10), // "setFactory"
QT_MOC_LITERAL(11, 121, 9) // "rbchecked"

    },
    "QumbiaProjectWizard\0init\0\0create\0"
    "checkText\0selectLocation\0addProperty\0"
    "parts\0removeProperty\0projectNameChanged\0"
    "setFactory\0rbchecked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QumbiaProjectWizard[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x08 /* Private */,
       3,    0,   60,    2, 0x08 /* Private */,
       4,    1,   61,    2, 0x08 /* Private */,
       5,    0,   64,    2, 0x08 /* Private */,
       6,    1,   65,    2, 0x08 /* Private */,
       6,    0,   68,    2, 0x28 /* Private | MethodCloned */,
       8,    0,   69,    2, 0x08 /* Private */,
       9,    1,   70,    2, 0x08 /* Private */,
      10,    1,   73,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QStringList,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Bool,   11,

       0        // eod
};

void QumbiaProjectWizard::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QumbiaProjectWizard *_t = static_cast<QumbiaProjectWizard *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->init(); break;
        case 1: _t->create(); break;
        case 2: _t->checkText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->selectLocation(); break;
        case 4: _t->addProperty((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 5: _t->addProperty(); break;
        case 6: _t->removeProperty(); break;
        case 7: _t->projectNameChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->setFactory((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject QumbiaProjectWizard::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QumbiaProjectWizard.data,
      qt_meta_data_QumbiaProjectWizard,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *QumbiaProjectWizard::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QumbiaProjectWizard::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QumbiaProjectWizard.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int QumbiaProjectWizard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
