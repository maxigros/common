/****************************************************************************
** Meta object code from reading C++ file 'qfieldview.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../skirl_painter_src/qfieldview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qfieldview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QFieldView[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   12,   11,   11, 0x05,
      72,   43,   11,   11, 0x05,
     104,   11,   11,   11, 0x05,
     119,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     135,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QFieldView[] = {
    "QFieldView\0\0r_ch,t_ch\0pointPicked(int,int)\0"
    "range,tr_frame,fr_min,fr_max\0"
    "pointPicked(double,int,int,int)\0"
    "scaleChanged()\0paintFinished()\0"
    "onThreadFinished()\0"
};

void QFieldView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QFieldView *_t = static_cast<QFieldView *>(_o);
        switch (_id) {
        case 0: _t->pointPicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->pointPicked((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 2: _t->scaleChanged(); break;
        case 3: _t->paintFinished(); break;
        case 4: _t->onThreadFinished(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QFieldView::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QFieldView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QFieldView,
      qt_meta_data_QFieldView, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QFieldView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QFieldView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QFieldView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QFieldView))
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
    }
    return _id;
}

// SIGNAL 0
void QFieldView::pointPicked(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QFieldView::pointPicked(double _t1, int _t2, int _t3, int _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QFieldView::scaleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void QFieldView::paintFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
