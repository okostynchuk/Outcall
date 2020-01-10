/****************************************************************************
** Meta object code from reading C++ file 'PlaceCallDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../PlaceCallDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlaceCallDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PlaceCallDialog_t {
    QByteArrayData data[15];
    char stringdata0[203];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PlaceCallDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PlaceCallDialog_t qt_meta_stringdata_PlaceCallDialog = {
    {
QT_MOC_LITERAL(0, 0, 15), // "PlaceCallDialog"
QT_MOC_LITERAL(1, 16, 12), // "onCallButton"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 14), // "onCancelButton"
QT_MOC_LITERAL(4, 45, 15), // "onChangeContact"
QT_MOC_LITERAL(5, 61, 4), // "name"
QT_MOC_LITERAL(6, 66, 20), // "onContactIndexChange"
QT_MOC_LITERAL(7, 87, 16), // "onContactsLoaded"
QT_MOC_LITERAL(8, 104, 16), // "QList<Contact*>&"
QT_MOC_LITERAL(9, 121, 8), // "contacts"
QT_MOC_LITERAL(10, 130, 16), // "onSettingsChange"
QT_MOC_LITERAL(11, 147, 19), // "onItemDoubleClicked"
QT_MOC_LITERAL(12, 167, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(13, 184, 4), // "item"
QT_MOC_LITERAL(14, 189, 13) // "onItemClicked"

    },
    "PlaceCallDialog\0onCallButton\0\0"
    "onCancelButton\0onChangeContact\0name\0"
    "onContactIndexChange\0onContactsLoaded\0"
    "QList<Contact*>&\0contacts\0onSettingsChange\0"
    "onItemDoubleClicked\0QTreeWidgetItem*\0"
    "item\0onItemClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlaceCallDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x09 /* Protected */,
       3,    0,   55,    2, 0x09 /* Protected */,
       4,    1,   56,    2, 0x09 /* Protected */,
       6,    1,   59,    2, 0x09 /* Protected */,
       7,    1,   62,    2, 0x09 /* Protected */,
      10,    0,   65,    2, 0x09 /* Protected */,
      11,    2,   66,    2, 0x09 /* Protected */,
      14,    2,   71,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12, QMetaType::Int,   13,    2,
    QMetaType::Void, 0x80000000 | 12, QMetaType::Int,   13,    2,

       0        // eod
};

void PlaceCallDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PlaceCallDialog *_t = static_cast<PlaceCallDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onCallButton(); break;
        case 1: _t->onCancelButton(); break;
        case 2: _t->onChangeContact((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->onContactIndexChange((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->onContactsLoaded((*reinterpret_cast< QList<Contact*>(*)>(_a[1]))); break;
        case 5: _t->onSettingsChange(); break;
        case 6: _t->onItemDoubleClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: _t->onItemClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject PlaceCallDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PlaceCallDialog.data,
      qt_meta_data_PlaceCallDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PlaceCallDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlaceCallDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PlaceCallDialog.stringdata0))
        return static_cast<void*>(const_cast< PlaceCallDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int PlaceCallDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
