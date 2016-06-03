/****************************************************************************
** Meta object code from reading C++ file 'viewwindow.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../skirl_painter_src/viewwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'viewwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ViewWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      42,   36,   11,   11, 0x0a,
      84,   11,   11,   11, 0x08,
     115,  105,   11,   11, 0x08,
     138,   11,   11,   11, 0x08,
     184,  155,   11,   11, 0x08,
     224,  221,   11,   11, 0x08,
     256,  221,   11,   11, 0x08,
     287,   11,   11,   11, 0x08,
     308,   11,   11,   11, 0x08,
     339,   11,   11,   11, 0x08,
     368,   11,   11,   11, 0x08,
     398,   11,   11,   11, 0x08,
     428,   11,   11,   11, 0x08,
     460,   11,   11,   11, 0x08,
     488,   11,   11,   11, 0x08,
     519,   11,   11,   11, 0x08,
     550,   11,   11,   11, 0x08,
     580,   11,   11,   11, 0x08,
     605,   11,   11,   11, 0x08,
     635,   11,   11,   11, 0x08,
     666,   11,   11,   11, 0x08,
     700,  694,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ViewWindow[] = {
    "ViewWindow\0\0view_params_requested()\0"
    "index\0on_mode_comboBox_currentIndexChanged(int)\0"
    "onSpinBoxesUpdated()\0r_ch,t_ch\0"
    "onPointPicked(int,int)\0onScaleChanged()\0"
    "range,fr_trace,fr_min,fr_max\0"
    "onIkoPointPicked(double,int,int,int)\0"
    "pt\0onRangeGraphPointPicked(QPoint)\0"
    "onTimeGraphPointPicked(QPoint)\0"
    "onHelpWindowClosed()\0"
    "on_resetScale_button_clicked()\0"
    "on_zoom_toolButton_clicked()\0"
    "on_trace_toolButton_clicked()\0"
    "on_halfScale_button_clicked()\0"
    "on_doubleScale_button_clicked()\0"
    "on_pan_toolButton_clicked()\0"
    "on_viewParams_button_clicked()\0"
    "on_gTrace_toolButton_clicked()\0"
    "on_gZoom_toolButton_clicked()\0"
    "on_help_button_clicked()\0"
    "on_clear_toolButton_clicked()\0"
    "on_openFile_action_triggered()\0"
    "on_clear_action_triggered()\0value\0"
    "on_thr_slider_valueChanged(int)\0"
};

void ViewWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ViewWindow *_t = static_cast<ViewWindow *>(_o);
        switch (_id) {
        case 0: _t->view_params_requested(); break;
        case 1: _t->on_mode_comboBox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->onSpinBoxesUpdated(); break;
        case 3: _t->onPointPicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->onScaleChanged(); break;
        case 5: _t->onIkoPointPicked((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 6: _t->onRangeGraphPointPicked((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 7: _t->onTimeGraphPointPicked((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 8: _t->onHelpWindowClosed(); break;
        case 9: _t->on_resetScale_button_clicked(); break;
        case 10: _t->on_zoom_toolButton_clicked(); break;
        case 11: _t->on_trace_toolButton_clicked(); break;
        case 12: _t->on_halfScale_button_clicked(); break;
        case 13: _t->on_doubleScale_button_clicked(); break;
        case 14: _t->on_pan_toolButton_clicked(); break;
        case 15: _t->on_viewParams_button_clicked(); break;
        case 16: _t->on_gTrace_toolButton_clicked(); break;
        case 17: _t->on_gZoom_toolButton_clicked(); break;
        case 18: _t->on_help_button_clicked(); break;
        case 19: _t->on_clear_toolButton_clicked(); break;
        case 20: _t->on_openFile_action_triggered(); break;
        case 21: _t->on_clear_action_triggered(); break;
        case 22: _t->on_thr_slider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ViewWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ViewWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_ViewWindow,
      qt_meta_data_ViewWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ViewWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ViewWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ViewWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ViewWindow))
        return static_cast<void*>(const_cast< ViewWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int ViewWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    }
    return _id;
}

// SIGNAL 0
void ViewWindow::view_params_requested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
