/****************************************************************************
** Meta object code from reading C++ file 'CreateRoomDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/CreateRoomDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CreateRoomDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CreateRoomDialog_t {
    QByteArrayData data[11];
    char stringdata0[130];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CreateRoomDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CreateRoomDialog_t qt_meta_stringdata_CreateRoomDialog = {
    {
QT_MOC_LITERAL(0, 0, 16), // "CreateRoomDialog"
QT_MOC_LITERAL(1, 17, 15), // "onCreateClicked"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 15), // "onCancelClicked"
QT_MOC_LITERAL(4, 50, 20), // "onCreateRoomResponse"
QT_MOC_LITERAL(5, 71, 10), // "StatusCode"
QT_MOC_LITERAL(6, 82, 4), // "code"
QT_MOC_LITERAL(7, 87, 8), // "RoomInfo"
QT_MOC_LITERAL(8, 96, 9), // "room_info"
QT_MOC_LITERAL(9, 106, 17), // "onConnectionError"
QT_MOC_LITERAL(10, 124, 5) // "error"

    },
    "CreateRoomDialog\0onCreateClicked\0\0"
    "onCancelClicked\0onCreateRoomResponse\0"
    "StatusCode\0code\0RoomInfo\0room_info\0"
    "onConnectionError\0error"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CreateRoomDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x08 /* Private */,
       3,    0,   35,    2, 0x08 /* Private */,
       4,    2,   36,    2, 0x08 /* Private */,
       9,    1,   41,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5, 0x80000000 | 7,    6,    8,
    QMetaType::Void, QMetaType::QString,   10,

       0        // eod
};

void CreateRoomDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CreateRoomDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onCreateClicked(); break;
        case 1: _t->onCancelClicked(); break;
        case 2: _t->onCreateRoomResponse((*reinterpret_cast< StatusCode(*)>(_a[1])),(*reinterpret_cast< const RoomInfo(*)>(_a[2]))); break;
        case 3: _t->onConnectionError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CreateRoomDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CreateRoomDialog.data,
    qt_meta_data_CreateRoomDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CreateRoomDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CreateRoomDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CreateRoomDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int CreateRoomDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
