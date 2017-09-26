/****************************************************************************
** Meta object code from reading C++ file 'estringlisteditor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../plugins/forms/estringlisteditor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'estringlisteditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_EStringListEditor_t {
    QByteArrayData data[15];
    char stringdata0[276];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EStringListEditor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EStringListEditor_t qt_meta_stringdata_EStringListEditor = {
    {
QT_MOC_LITERAL(0, 0, 17), // "EStringListEditor"
QT_MOC_LITERAL(1, 18, 10), // "stringList"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 6), // "accept"
QT_MOC_LITERAL(4, 37, 24), // "on_newItemButton_clicked"
QT_MOC_LITERAL(5, 62, 27), // "on_deleteItemButton_clicked"
QT_MOC_LITERAL(6, 90, 27), // "on_moveItemUpButton_clicked"
QT_MOC_LITERAL(7, 118, 29), // "on_moveItemDownButton_clicked"
QT_MOC_LITERAL(8, 148, 31), // "on_listWidget_currentRowChanged"
QT_MOC_LITERAL(9, 180, 10), // "currentRow"
QT_MOC_LITERAL(10, 191, 25), // "on_listWidget_itemChanged"
QT_MOC_LITERAL(11, 217, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(12, 234, 4), // "item"
QT_MOC_LITERAL(13, 239, 31), // "on_itemTextLineEdit_textChanged"
QT_MOC_LITERAL(14, 271, 4) // "text"

    },
    "EStringListEditor\0stringList\0\0accept\0"
    "on_newItemButton_clicked\0"
    "on_deleteItemButton_clicked\0"
    "on_moveItemUpButton_clicked\0"
    "on_moveItemDownButton_clicked\0"
    "on_listWidget_currentRowChanged\0"
    "currentRow\0on_listWidget_itemChanged\0"
    "QListWidgetItem*\0item\0"
    "on_itemTextLineEdit_textChanged\0text"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EStringListEditor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   62,    2, 0x09 /* Protected */,
       4,    0,   63,    2, 0x08 /* Private */,
       5,    0,   64,    2, 0x08 /* Private */,
       6,    0,   65,    2, 0x08 /* Private */,
       7,    0,   66,    2, 0x08 /* Private */,
       8,    1,   67,    2, 0x08 /* Private */,
      10,    1,   70,    2, 0x08 /* Private */,
      13,    1,   73,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QStringList,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, QMetaType::QString,   14,

       0        // eod
};

void EStringListEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EStringListEditor *_t = static_cast<EStringListEditor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->stringList((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 1: _t->accept(); break;
        case 2: _t->on_newItemButton_clicked(); break;
        case 3: _t->on_deleteItemButton_clicked(); break;
        case 4: _t->on_moveItemUpButton_clicked(); break;
        case 5: _t->on_moveItemDownButton_clicked(); break;
        case 6: _t->on_listWidget_currentRowChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->on_listWidget_itemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 8: _t->on_itemTextLineEdit_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (EStringListEditor::*_t)(const QStringList & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EStringListEditor::stringList)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject EStringListEditor::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_EStringListEditor.data,
      qt_meta_data_EStringListEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *EStringListEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EStringListEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_EStringListEditor.stringdata0))
        return static_cast<void*>(const_cast< EStringListEditor*>(this));
    return QDialog::qt_metacast(_clname);
}

int EStringListEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void EStringListEditor::stringList(const QStringList & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
