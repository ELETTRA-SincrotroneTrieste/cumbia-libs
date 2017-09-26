/****************************************************************************
** Meta object code from reading C++ file 'ttablebooleditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../plugins/forms/ttablebooleditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ttablebooleditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_TTableBoolEditor_t {
    QByteArrayData data[10];
    char stringdata0[112];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TTableBoolEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TTableBoolEditor_t qt_meta_stringdata_TTableBoolEditor = {
    {
QT_MOC_LITERAL(0, 0, 16), // "TTableBoolEditor"
QT_MOC_LITERAL(1, 17, 15), // "propertyChanged"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 8), // "propName"
QT_MOC_LITERAL(4, 43, 3), // "val"
QT_MOC_LITERAL(5, 47, 6), // "accept"
QT_MOC_LITERAL(6, 54, 15), // "updateColorTrue"
QT_MOC_LITERAL(7, 70, 16), // "updateColorFalse"
QT_MOC_LITERAL(8, 87, 11), // "updateFlags"
QT_MOC_LITERAL(9, 99, 12) // "updateLabels"

    },
    "TTableBoolEditor\0propertyChanged\0\0"
    "propName\0val\0accept\0updateColorTrue\0"
    "updateColorFalse\0updateFlags\0updateLabels"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TTableBoolEditor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   49,    2, 0x09 /* Protected */,
       6,    0,   50,    2, 0x08 /* Private */,
       7,    0,   51,    2, 0x08 /* Private */,
       8,    0,   52,    2, 0x08 /* Private */,
       9,    0,   53,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,    3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void TTableBoolEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TTableBoolEditor *_t = static_cast<TTableBoolEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->propertyChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2]))); break;
        case 1: _t->accept(); break;
        case 2: _t->updateColorTrue(); break;
        case 3: _t->updateColorFalse(); break;
        case 4: _t->updateFlags(); break;
        case 5: _t->updateLabels(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (TTableBoolEditor::*_t)(const QString & , const QVariant & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TTableBoolEditor::propertyChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject TTableBoolEditor::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_TTableBoolEditor.data,
      qt_meta_data_TTableBoolEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *TTableBoolEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TTableBoolEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_TTableBoolEditor.stringdata0))
        return static_cast<void*>(const_cast< TTableBoolEditor*>(this));
    return QDialog::qt_metacast(_clname);
}

int TTableBoolEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void TTableBoolEditor::propertyChanged(const QString & _t1, const QVariant & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
