/****************************************************************************
** Meta object code from reading C++ file 'LobbyWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/LobbyWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LobbyWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_LobbyWindow_t {
    QByteArrayData data[29];
    char stringdata0[359];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LobbyWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LobbyWindow_t qt_meta_stringdata_LobbyWindow = {
    {
QT_MOC_LITERAL(0, 0, 11), // "LobbyWindow"
QT_MOC_LITERAL(1, 12, 16), // "onRefreshClicked"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 19), // "onCreateRoomClicked"
QT_MOC_LITERAL(4, 50, 14), // "onStatsClicked"
QT_MOC_LITERAL(5, 65, 15), // "onLogoutClicked"
QT_MOC_LITERAL(6, 81, 22), // "onRoomTableItemClicked"
QT_MOC_LITERAL(7, 104, 3), // "row"
QT_MOC_LITERAL(8, 108, 6), // "column"
QT_MOC_LITERAL(9, 115, 19), // "onListRoomsResponse"
QT_MOC_LITERAL(10, 135, 8), // "uint16_t"
QT_MOC_LITERAL(11, 144, 5), // "count"
QT_MOC_LITERAL(12, 150, 17), // "QVector<RoomInfo>"
QT_MOC_LITERAL(13, 168, 5), // "rooms"
QT_MOC_LITERAL(14, 174, 20), // "onCreateRoomResponse"
QT_MOC_LITERAL(15, 195, 10), // "StatusCode"
QT_MOC_LITERAL(16, 206, 4), // "code"
QT_MOC_LITERAL(17, 211, 8), // "RoomInfo"
QT_MOC_LITERAL(18, 220, 9), // "room_info"
QT_MOC_LITERAL(19, 230, 18), // "onJoinRoomResponse"
QT_MOC_LITERAL(20, 249, 7), // "uint8_t"
QT_MOC_LITERAL(21, 257, 12), // "player_count"
QT_MOC_LITERAL(22, 270, 19), // "QVector<PlayerInfo>"
QT_MOC_LITERAL(23, 290, 7), // "players"
QT_MOC_LITERAL(24, 298, 8), // "uint32_t"
QT_MOC_LITERAL(25, 307, 12), // "host_user_id"
QT_MOC_LITERAL(26, 320, 17), // "onConnectionError"
QT_MOC_LITERAL(27, 338, 5), // "error"
QT_MOC_LITERAL(28, 344, 14) // "onRefreshTimer"

    },
    "LobbyWindow\0onRefreshClicked\0\0"
    "onCreateRoomClicked\0onStatsClicked\0"
    "onLogoutClicked\0onRoomTableItemClicked\0"
    "row\0column\0onListRoomsResponse\0uint16_t\0"
    "count\0QVector<RoomInfo>\0rooms\0"
    "onCreateRoomResponse\0StatusCode\0code\0"
    "RoomInfo\0room_info\0onJoinRoomResponse\0"
    "uint8_t\0player_count\0QVector<PlayerInfo>\0"
    "players\0uint32_t\0host_user_id\0"
    "onConnectionError\0error\0onRefreshTimer"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LobbyWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x08 /* Private */,
       3,    0,   65,    2, 0x08 /* Private */,
       4,    0,   66,    2, 0x08 /* Private */,
       5,    0,   67,    2, 0x08 /* Private */,
       6,    2,   68,    2, 0x08 /* Private */,
       9,    2,   73,    2, 0x08 /* Private */,
      14,    2,   78,    2, 0x08 /* Private */,
      19,    5,   83,    2, 0x08 /* Private */,
      26,    1,   94,    2, 0x08 /* Private */,
      28,    0,   97,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    7,    8,
    QMetaType::Void, 0x80000000 | 10, 0x80000000 | 12,   11,   13,
    QMetaType::Void, 0x80000000 | 15, 0x80000000 | 17,   16,   18,
    QMetaType::Void, 0x80000000 | 15, 0x80000000 | 17, 0x80000000 | 20, 0x80000000 | 22, 0x80000000 | 24,   16,   18,   21,   23,   25,
    QMetaType::Void, QMetaType::QString,   27,
    QMetaType::Void,

       0        // eod
};

void LobbyWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LobbyWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onRefreshClicked(); break;
        case 1: _t->onCreateRoomClicked(); break;
        case 2: _t->onStatsClicked(); break;
        case 3: _t->onLogoutClicked(); break;
        case 4: _t->onRoomTableItemClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->onListRoomsResponse((*reinterpret_cast< uint16_t(*)>(_a[1])),(*reinterpret_cast< const QVector<RoomInfo>(*)>(_a[2]))); break;
        case 6: _t->onCreateRoomResponse((*reinterpret_cast< StatusCode(*)>(_a[1])),(*reinterpret_cast< const RoomInfo(*)>(_a[2]))); break;
        case 7: _t->onJoinRoomResponse((*reinterpret_cast< StatusCode(*)>(_a[1])),(*reinterpret_cast< const RoomInfo(*)>(_a[2])),(*reinterpret_cast< uint8_t(*)>(_a[3])),(*reinterpret_cast< const QVector<PlayerInfo>(*)>(_a[4])),(*reinterpret_cast< uint32_t(*)>(_a[5]))); break;
        case 8: _t->onConnectionError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->onRefreshTimer(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject LobbyWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_LobbyWindow.data,
    qt_meta_data_LobbyWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *LobbyWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LobbyWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LobbyWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int LobbyWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
