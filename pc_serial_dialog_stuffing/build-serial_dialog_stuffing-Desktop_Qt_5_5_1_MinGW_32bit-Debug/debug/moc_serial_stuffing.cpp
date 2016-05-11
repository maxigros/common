/****************************************************************************
** Meta object code from reading C++ file 'serial_stuffing.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../serial_dialog_stuffing/serial_stuffing.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serial_stuffing.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_cp9b_lib_t {
    QByteArrayData data[10];
    char stringdata0[72];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_cp9b_lib_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_cp9b_lib_t qt_meta_stringdata_cp9b_lib = {
    {
QT_MOC_LITERAL(0, 0, 8), // "cp9b_lib"
QT_MOC_LITERAL(1, 9, 11), // "send_to_gui"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 3), // "key"
QT_MOC_LITERAL(4, 26, 3), // "msg"
QT_MOC_LITERAL(5, 30, 5), // "char*"
QT_MOC_LITERAL(6, 36, 4), // "data"
QT_MOC_LITERAL(7, 41, 9), // "data_size"
QT_MOC_LITERAL(8, 51, 8), // "read_COM"
QT_MOC_LITERAL(9, 60, 11) // "tim_timeout"

    },
    "cp9b_lib\0send_to_gui\0\0key\0msg\0char*\0"
    "data\0data_size\0read_COM\0tim_timeout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_cp9b_lib[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    4,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    0,   38,    2, 0x08 /* Private */,
       9,    0,   39,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::QString, 0x80000000 | 5, QMetaType::Int,    3,    4,    6,    7,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void cp9b_lib::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        cp9b_lib *_t = static_cast<cp9b_lib *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->send_to_gui((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< char*(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 1: _t->read_COM(); break;
        case 2: _t->tim_timeout(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (cp9b_lib::*_t)(int , QString , char * , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&cp9b_lib::send_to_gui)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject cp9b_lib::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_cp9b_lib.data,
      qt_meta_data_cp9b_lib,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *cp9b_lib::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *cp9b_lib::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_cp9b_lib.stringdata0))
        return static_cast<void*>(const_cast< cp9b_lib*>(this));
    return QObject::qt_metacast(_clname);
}

int cp9b_lib::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void cp9b_lib::send_to_gui(int _t1, QString _t2, char * _t3, int _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
