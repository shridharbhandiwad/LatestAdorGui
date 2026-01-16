/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../MainWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[17];
    char stringdata0[264];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 13), // "updateDisplay"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 20), // "readPendingDatagrams"
QT_MOC_LITERAL(4, 47, 21), // "onSimulateDataToggled"
QT_MOC_LITERAL(5, 69, 14), // "onRangeChanged"
QT_MOC_LITERAL(6, 84, 5), // "range"
QT_MOC_LITERAL(7, 90, 17), // "onMinRangeChanged"
QT_MOC_LITERAL(8, 108, 4), // "text"
QT_MOC_LITERAL(9, 113, 14), // "onChirpChanged"
QT_MOC_LITERAL(10, 128, 18), // "onBandwidthChanged"
QT_MOC_LITERAL(11, 147, 17), // "onMinSpeedChanged"
QT_MOC_LITERAL(12, 165, 17), // "onMaxSpeedChanged"
QT_MOC_LITERAL(13, 183, 23), // "onChirpsPerFrameChanged"
QT_MOC_LITERAL(14, 207, 24), // "onSamplesPerChirpChanged"
QT_MOC_LITERAL(15, 232, 15), // "onApplySettings"
QT_MOC_LITERAL(16, 248, 15) // "onResetSettings"

    },
    "MainWindow\0updateDisplay\0\0"
    "readPendingDatagrams\0onSimulateDataToggled\0"
    "onRangeChanged\0range\0onMinRangeChanged\0"
    "text\0onChirpChanged\0onBandwidthChanged\0"
    "onMinSpeedChanged\0onMaxSpeedChanged\0"
    "onChirpsPerFrameChanged\0"
    "onSamplesPerChirpChanged\0onApplySettings\0"
    "onResetSettings"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x08 /* Private */,
       3,    0,   80,    2, 0x08 /* Private */,
       4,    0,   81,    2, 0x08 /* Private */,
       5,    1,   82,    2, 0x08 /* Private */,
       7,    1,   85,    2, 0x08 /* Private */,
       9,    1,   88,    2, 0x08 /* Private */,
      10,    1,   91,    2, 0x08 /* Private */,
      11,    1,   94,    2, 0x08 /* Private */,
      12,    1,   97,    2, 0x08 /* Private */,
      13,    1,  100,    2, 0x08 /* Private */,
      14,    1,  103,    2, 0x08 /* Private */,
      15,    0,  106,    2, 0x08 /* Private */,
      16,    0,  107,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateDisplay(); break;
        case 1: _t->readPendingDatagrams(); break;
        case 2: _t->onSimulateDataToggled(); break;
        case 3: _t->onRangeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->onMinRangeChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->onChirpChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->onBandwidthChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->onMinSpeedChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->onMaxSpeedChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->onChirpsPerFrameChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->onSamplesPerChirpChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->onApplySettings(); break;
        case 12: _t->onResetSettings(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
