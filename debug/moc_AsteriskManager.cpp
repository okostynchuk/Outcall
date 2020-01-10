/****************************************************************************
** Meta object code from reading C++ file 'AsteriskManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../AsteriskManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AsteriskManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AsteriskManager_t {
    QByteArrayData data[20];
    char stringdata0[223];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AsteriskManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AsteriskManager_t qt_meta_stringdata_AsteriskManager = {
    {
QT_MOC_LITERAL(0, 0, 15), // "AsteriskManager"
QT_MOC_LITERAL(1, 16, 15), // "messageReceived"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 7), // "message"
QT_MOC_LITERAL(4, 41, 19), // "authenticationState"
QT_MOC_LITERAL(5, 61, 5), // "state"
QT_MOC_LITERAL(6, 67, 13), // "callDeteceted"
QT_MOC_LITERAL(7, 81, 4), // "call"
QT_MOC_LITERAL(8, 86, 9), // "CallState"
QT_MOC_LITERAL(9, 96, 12), // "callReceived"
QT_MOC_LITERAL(10, 109, 5), // "error"
QT_MOC_LITERAL(11, 115, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(12, 144, 11), // "socketError"
QT_MOC_LITERAL(13, 156, 3), // "msg"
QT_MOC_LITERAL(14, 160, 12), // "stateChanged"
QT_MOC_LITERAL(15, 173, 13), // "AsteriskState"
QT_MOC_LITERAL(16, 187, 7), // "onError"
QT_MOC_LITERAL(17, 195, 4), // "read"
QT_MOC_LITERAL(18, 200, 5), // "login"
QT_MOC_LITERAL(19, 206, 16) // "onSettingsChange"

    },
    "AsteriskManager\0messageReceived\0\0"
    "message\0authenticationState\0state\0"
    "callDeteceted\0call\0CallState\0callReceived\0"
    "error\0QAbstractSocket::SocketError\0"
    "socketError\0msg\0stateChanged\0AsteriskState\0"
    "onError\0read\0login\0onSettingsChange"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AsteriskManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       4,    1,   67,    2, 0x06 /* Public */,
       6,    2,   70,    2, 0x06 /* Public */,
       9,    1,   75,    2, 0x06 /* Public */,
      10,    2,   78,    2, 0x06 /* Public */,
      14,    1,   83,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      16,    1,   86,    2, 0x09 /* Protected */,
      17,    0,   89,    2, 0x09 /* Protected */,
      18,    0,   90,    2, 0x09 /* Protected */,
      19,    0,   91,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::QVariantMap, 0x80000000 | 8,    7,    5,
    QMetaType::Void, QMetaType::QVariantMap,    2,
    QMetaType::Void, 0x80000000 | 11, QMetaType::QString,   12,   13,
    QMetaType::Void, 0x80000000 | 15,    5,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void AsteriskManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AsteriskManager *_t = static_cast<AsteriskManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->messageReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->authenticationState((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->callDeteceted((*reinterpret_cast< const QMap<QString,QVariant>(*)>(_a[1])),(*reinterpret_cast< CallState(*)>(_a[2]))); break;
        case 3: _t->callReceived((*reinterpret_cast< const QMap<QString,QVariant>(*)>(_a[1]))); break;
        case 4: _t->error((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 5: _t->stateChanged((*reinterpret_cast< AsteriskState(*)>(_a[1]))); break;
        case 6: _t->onError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 7: _t->read(); break;
        case 8: _t->login(); break;
        case 9: _t->onSettingsChange(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AsteriskManager::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AsteriskManager::messageReceived)) {
                *result = 0;
            }
        }
        {
            typedef void (AsteriskManager::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AsteriskManager::authenticationState)) {
                *result = 1;
            }
        }
        {
            typedef void (AsteriskManager::*_t)(const QMap<QString,QVariant> & , CallState );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AsteriskManager::callDeteceted)) {
                *result = 2;
            }
        }
        {
            typedef void (AsteriskManager::*_t)(const QMap<QString,QVariant> & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AsteriskManager::callReceived)) {
                *result = 3;
            }
        }
        {
            typedef void (AsteriskManager::*_t)(QAbstractSocket::SocketError , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AsteriskManager::error)) {
                *result = 4;
            }
        }
        {
            typedef void (AsteriskManager::*_t)(AsteriskState );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AsteriskManager::stateChanged)) {
                *result = 5;
            }
        }
    }
}

const QMetaObject AsteriskManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_AsteriskManager.data,
      qt_meta_data_AsteriskManager,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *AsteriskManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AsteriskManager::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_AsteriskManager.stringdata0))
        return static_cast<void*>(const_cast< AsteriskManager*>(this));
    return QObject::qt_metacast(_clname);
}

int AsteriskManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void AsteriskManager::messageReceived(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AsteriskManager::authenticationState(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AsteriskManager::callDeteceted(const QMap<QString,QVariant> & _t1, CallState _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void AsteriskManager::callReceived(const QMap<QString,QVariant> & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void AsteriskManager::error(QAbstractSocket::SocketError _t1, const QString & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void AsteriskManager::stateChanged(AsteriskState _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
