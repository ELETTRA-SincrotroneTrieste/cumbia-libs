/****************************************************************************
** Meta object code from reading C++ file 'create-delete.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/create-delete.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'create-delete.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CreateDelete_t {
    QByteArrayData data[7];
    char stringdata0[74];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CreateDelete_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CreateDelete_t qt_meta_stringdata_CreateDelete = {
    {
QT_MOC_LITERAL(0, 0, 12), // "CreateDelete"
QT_MOC_LITERAL(1, 13, 10), // "addReaders"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 9), // "addReader"
QT_MOC_LITERAL(4, 35, 13), // "disconnectSrc"
QT_MOC_LITERAL(5, 49, 12), // "deleteReader"
QT_MOC_LITERAL(6, 62, 11) // "deleteMulti"

    },
    "CreateDelete\0addReaders\0\0addReader\0"
    "disconnectSrc\0deleteReader\0deleteMulti"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CreateDelete[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x08 /* Private */,
       3,    0,   40,    2, 0x08 /* Private */,
       4,    0,   41,    2, 0x08 /* Private */,
       5,    0,   42,    2, 0x08 /* Private */,
       6,    0,   43,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CreateDelete::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CreateDelete *_t = static_cast<CreateDelete *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->addReaders(); break;
        case 1: _t->addReader(); break;
        case 2: _t->disconnectSrc(); break;
        case 3: _t->deleteReader(); break;
        case 4: _t->deleteMulti(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject CreateDelete::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CreateDelete.data,
      qt_meta_data_CreateDelete,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CreateDelete::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CreateDelete::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CreateDelete.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CreateDelete::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
