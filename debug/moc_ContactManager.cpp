/****************************************************************************
** Meta object code from reading C++ file 'ContactManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../ContactManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ContactManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AddOutlookContactThread_t {
    QByteArrayData data[5];
    char stringdata0[62];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AddOutlookContactThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AddOutlookContactThread_t qt_meta_stringdata_AddOutlookContactThread = {
    {
QT_MOC_LITERAL(0, 0, 23), // "AddOutlookContactThread"
QT_MOC_LITERAL(1, 24, 27), // "addingContactThreadFinished"
QT_MOC_LITERAL(2, 52, 0), // ""
QT_MOC_LITERAL(3, 53, 3), // "bOK"
QT_MOC_LITERAL(4, 57, 4) // "info"

    },
    "AddOutlookContactThread\0"
    "addingContactThreadFinished\0\0bOK\0info"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AddOutlookContactThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    3,    4,

       0        // eod
};

void AddOutlookContactThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AddOutlookContactThread *_t = static_cast<AddOutlookContactThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->addingContactThreadFinished((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AddOutlookContactThread::*_t)(bool , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AddOutlookContactThread::addingContactThreadFinished)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject AddOutlookContactThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_AddOutlookContactThread.data,
      qt_meta_data_AddOutlookContactThread,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *AddOutlookContactThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AddOutlookContactThread::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_AddOutlookContactThread.stringdata0))
        return static_cast<void*>(const_cast< AddOutlookContactThread*>(this));
    return QThread::qt_metacast(_clname);
}

int AddOutlookContactThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void AddOutlookContactThread::addingContactThreadFinished(bool _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_ViewOutlookContactThread_t {
    QByteArrayData data[5];
    char stringdata0[64];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ViewOutlookContactThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ViewOutlookContactThread_t qt_meta_stringdata_ViewOutlookContactThread = {
    {
QT_MOC_LITERAL(0, 0, 24), // "ViewOutlookContactThread"
QT_MOC_LITERAL(1, 25, 28), // "viewingContactThreadFinished"
QT_MOC_LITERAL(2, 54, 0), // ""
QT_MOC_LITERAL(3, 55, 3), // "bOK"
QT_MOC_LITERAL(4, 59, 4) // "info"

    },
    "ViewOutlookContactThread\0"
    "viewingContactThreadFinished\0\0bOK\0"
    "info"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ViewOutlookContactThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    3,    4,

       0        // eod
};

void ViewOutlookContactThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ViewOutlookContactThread *_t = static_cast<ViewOutlookContactThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->viewingContactThreadFinished((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ViewOutlookContactThread::*_t)(bool , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewOutlookContactThread::viewingContactThreadFinished)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject ViewOutlookContactThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_ViewOutlookContactThread.data,
      qt_meta_data_ViewOutlookContactThread,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ViewOutlookContactThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ViewOutlookContactThread::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ViewOutlookContactThread.stringdata0))
        return static_cast<void*>(const_cast< ViewOutlookContactThread*>(this));
    return QThread::qt_metacast(_clname);
}

int ViewOutlookContactThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void ViewOutlookContactThread::viewingContactThreadFinished(bool _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_ContactManager_t {
    QByteArrayData data[13];
    char stringdata0[188];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ContactManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ContactManager_t qt_meta_stringdata_ContactManager = {
    {
QT_MOC_LITERAL(0, 0, 14), // "ContactManager"
QT_MOC_LITERAL(1, 15, 14), // "contactsLoaded"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 16), // "QList<Contact*>&"
QT_MOC_LITERAL(4, 48, 8), // "contacts"
QT_MOC_LITERAL(5, 57, 7), // "syncing"
QT_MOC_LITERAL(6, 65, 6), // "status"
QT_MOC_LITERAL(7, 72, 29), // "loadingContactsThreadFinished"
QT_MOC_LITERAL(8, 102, 3), // "bOK"
QT_MOC_LITERAL(9, 106, 4), // "info"
QT_MOC_LITERAL(10, 111, 25), // "loadContactsThreadStarted"
QT_MOC_LITERAL(11, 137, 27), // "addingContactThreadFinished"
QT_MOC_LITERAL(12, 165, 22) // "onRefreshContactsTimer"

    },
    "ContactManager\0contactsLoaded\0\0"
    "QList<Contact*>&\0contacts\0syncing\0"
    "status\0loadingContactsThreadFinished\0"
    "bOK\0info\0loadContactsThreadStarted\0"
    "addingContactThreadFinished\0"
    "onRefreshContactsTimer"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ContactManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       5,    1,   47,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    2,   50,    2, 0x08 /* Private */,
      10,    0,   55,    2, 0x08 /* Private */,
      11,    2,   56,    2, 0x08 /* Private */,
      12,    0,   61,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Bool,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    8,    9,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    8,    9,
    QMetaType::Void,

       0        // eod
};

void ContactManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ContactManager *_t = static_cast<ContactManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->contactsLoaded((*reinterpret_cast< QList<Contact*>(*)>(_a[1]))); break;
        case 1: _t->syncing((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->loadingContactsThreadFinished((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 3: _t->loadContactsThreadStarted(); break;
        case 4: _t->addingContactThreadFinished((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 5: _t->onRefreshContactsTimer(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ContactManager::*_t)(QList<Contact*> & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ContactManager::contactsLoaded)) {
                *result = 0;
            }
        }
        {
            typedef void (ContactManager::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ContactManager::syncing)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject ContactManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ContactManager.data,
      qt_meta_data_ContactManager,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ContactManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ContactManager::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ContactManager.stringdata0))
        return static_cast<void*>(const_cast< ContactManager*>(this));
    return QObject::qt_metacast(_clname);
}

int ContactManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void ContactManager::contactsLoaded(QList<Contact*> & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ContactManager::syncing(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
