/****************************************************************************
** Meta object code from reading C++ file 'qfieldview.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../skirl_painter_src/qfieldview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qfieldview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QFieldView_t {
    QByteArrayData data[12];
    char stringdata0[107];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QFieldView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QFieldView_t qt_meta_stringdata_QFieldView = {
    {
QT_MOC_LITERAL(0, 0, 10), // "QFieldView"
QT_MOC_LITERAL(1, 11, 11), // "pointPicked"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 4), // "r_ch"
QT_MOC_LITERAL(4, 29, 4), // "t_ch"
QT_MOC_LITERAL(5, 34, 5), // "range"
QT_MOC_LITERAL(6, 40, 8), // "tr_frame"
QT_MOC_LITERAL(7, 49, 6), // "fr_min"
QT_MOC_LITERAL(8, 56, 6), // "fr_max"
QT_MOC_LITERAL(9, 63, 12), // "scaleChanged"
QT_MOC_LITERAL(10, 76, 13), // "paintFinished"
QT_MOC_LITERAL(11, 90, 16) // "onThreadFinished"

    },
    "QFieldView\0pointPicked\0\0r_ch\0t_ch\0"
    "range\0tr_frame\0fr_min\0fr_max\0scaleChanged\0"
    "paintFinished\0onThreadFinished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QFieldView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x06 /* Public */,
       1,    4,   44,    2, 0x06 /* Public */,
       9,    0,   53,    2, 0x06 /* Public */,
      10,    0,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,   55,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::Double, QMetaType::Int, QMetaType::Int, QMetaType::Int,    5,    6,    7,    8,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void QFieldView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QFieldView *_t = static_cast<QFieldView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->pointPicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->pointPicked((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 2: _t->scaleChanged(); break;
        case 3: _t->paintFinished(); break;
        case 4: _t->onThreadFinished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QFieldView::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QFieldView::pointPicked)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (QFieldView::*_t)(double , int , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QFieldView::pointPicked)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (QFieldView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QFieldView::scaleChanged)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (QFieldView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QFieldView::paintFinished)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject QFieldView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QFieldView.data,
      qt_meta_data_QFieldView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QFieldView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QFieldView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QFieldView.stringdata0))
        return static_cast<void*>(const_cast< QFieldView*>(this));
    return QWidget::qt_metacast(_clname);
}

int QFieldView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void QFieldView::pointPicked(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QFieldView::pointPicked(double _t1, int _t2, int _t3, int _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QFieldView::scaleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void QFieldView::paintFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
