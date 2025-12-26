/****************************************************************************
** Meta object code from reading C++ file 'NetworkManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/network/NetworkManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_NetworkManager_t {
    QByteArrayData data[63];
    char stringdata0[825];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NetworkManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NetworkManager_t qt_meta_stringdata_NetworkManager = {
    {
QT_MOC_LITERAL(0, 0, 14), // "NetworkManager"
QT_MOC_LITERAL(1, 15, 9), // "connected"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 12), // "disconnected"
QT_MOC_LITERAL(4, 39, 15), // "connectionError"
QT_MOC_LITERAL(5, 55, 5), // "error"
QT_MOC_LITERAL(6, 61, 16), // "registerResponse"
QT_MOC_LITERAL(7, 78, 10), // "StatusCode"
QT_MOC_LITERAL(8, 89, 4), // "code"
QT_MOC_LITERAL(9, 94, 13), // "loginResponse"
QT_MOC_LITERAL(10, 108, 8), // "uint32_t"
QT_MOC_LITERAL(11, 117, 6), // "userId"
QT_MOC_LITERAL(12, 124, 11), // "displayName"
QT_MOC_LITERAL(13, 136, 18), // "createRoomResponse"
QT_MOC_LITERAL(14, 155, 8), // "RoomInfo"
QT_MOC_LITERAL(15, 164, 8), // "roomInfo"
QT_MOC_LITERAL(16, 173, 17), // "listRoomsResponse"
QT_MOC_LITERAL(17, 191, 8), // "uint16_t"
QT_MOC_LITERAL(18, 200, 9), // "roomCount"
QT_MOC_LITERAL(19, 210, 17), // "QVector<RoomInfo>"
QT_MOC_LITERAL(20, 228, 5), // "rooms"
QT_MOC_LITERAL(21, 234, 16), // "joinRoomResponse"
QT_MOC_LITERAL(22, 251, 7), // "uint8_t"
QT_MOC_LITERAL(23, 259, 11), // "playerCount"
QT_MOC_LITERAL(24, 271, 19), // "QVector<PlayerInfo>"
QT_MOC_LITERAL(25, 291, 7), // "players"
QT_MOC_LITERAL(26, 299, 10), // "hostUserId"
QT_MOC_LITERAL(27, 310, 17), // "playerJoinedNotif"
QT_MOC_LITERAL(28, 328, 10), // "PlayerInfo"
QT_MOC_LITERAL(29, 339, 6), // "player"
QT_MOC_LITERAL(30, 346, 15), // "playerLeftNotif"
QT_MOC_LITERAL(31, 362, 16), // "readyStatusNotif"
QT_MOC_LITERAL(32, 379, 5), // "ready"
QT_MOC_LITERAL(33, 385, 14), // "gameStartNotif"
QT_MOC_LITERAL(34, 400, 13), // "questionNotif"
QT_MOC_LITERAL(35, 414, 10), // "questionId"
QT_MOC_LITERAL(36, 425, 7), // "content"
QT_MOC_LITERAL(37, 433, 7), // "options"
QT_MOC_LITERAL(38, 441, 12), // "timeLimitSec"
QT_MOC_LITERAL(39, 454, 16), // "roundResultNotif"
QT_MOC_LITERAL(40, 471, 13), // "correctOption"
QT_MOC_LITERAL(41, 485, 26), // "QVector<PlayerRoundResult>"
QT_MOC_LITERAL(42, 512, 7), // "results"
QT_MOC_LITERAL(43, 520, 21), // "playerEliminatedNotif"
QT_MOC_LITERAL(44, 542, 13), // "gameOverNotif"
QT_MOC_LITERAL(45, 556, 12), // "rankingCount"
QT_MOC_LITERAL(46, 569, 26), // "QVector<PlayerFinalResult>"
QT_MOC_LITERAL(47, 596, 8), // "rankings"
QT_MOC_LITERAL(48, 605, 15), // "gamePausedNotif"
QT_MOC_LITERAL(49, 621, 16), // "gameResumedNotif"
QT_MOC_LITERAL(50, 638, 19), // "gameTerminatedNotif"
QT_MOC_LITERAL(51, 658, 17), // "TerminationReason"
QT_MOC_LITERAL(52, 676, 6), // "reason"
QT_MOC_LITERAL(53, 683, 13), // "statsResponse"
QT_MOC_LITERAL(54, 697, 17), // "UserStatsResponse"
QT_MOC_LITERAL(55, 715, 5), // "stats"
QT_MOC_LITERAL(56, 721, 13), // "errorResponse"
QT_MOC_LITERAL(57, 735, 7), // "message"
QT_MOC_LITERAL(58, 743, 11), // "onConnected"
QT_MOC_LITERAL(59, 755, 14), // "onDisconnected"
QT_MOC_LITERAL(60, 770, 11), // "onReadyRead"
QT_MOC_LITERAL(61, 782, 13), // "onSocketError"
QT_MOC_LITERAL(62, 796, 28) // "QAbstractSocket::SocketError"

    },
    "NetworkManager\0connected\0\0disconnected\0"
    "connectionError\0error\0registerResponse\0"
    "StatusCode\0code\0loginResponse\0uint32_t\0"
    "userId\0displayName\0createRoomResponse\0"
    "RoomInfo\0roomInfo\0listRoomsResponse\0"
    "uint16_t\0roomCount\0QVector<RoomInfo>\0"
    "rooms\0joinRoomResponse\0uint8_t\0"
    "playerCount\0QVector<PlayerInfo>\0players\0"
    "hostUserId\0playerJoinedNotif\0PlayerInfo\0"
    "player\0playerLeftNotif\0readyStatusNotif\0"
    "ready\0gameStartNotif\0questionNotif\0"
    "questionId\0content\0options\0timeLimitSec\0"
    "roundResultNotif\0correctOption\0"
    "QVector<PlayerRoundResult>\0results\0"
    "playerEliminatedNotif\0gameOverNotif\0"
    "rankingCount\0QVector<PlayerFinalResult>\0"
    "rankings\0gamePausedNotif\0gameResumedNotif\0"
    "gameTerminatedNotif\0TerminationReason\0"
    "reason\0statsResponse\0UserStatsResponse\0"
    "stats\0errorResponse\0message\0onConnected\0"
    "onDisconnected\0onReadyRead\0onSocketError\0"
    "QAbstractSocket::SocketError"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NetworkManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      21,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  139,    2, 0x06 /* Public */,
       3,    0,  140,    2, 0x06 /* Public */,
       4,    1,  141,    2, 0x06 /* Public */,
       6,    1,  144,    2, 0x06 /* Public */,
       9,    3,  147,    2, 0x06 /* Public */,
      13,    2,  154,    2, 0x06 /* Public */,
      16,    2,  159,    2, 0x06 /* Public */,
      21,    5,  164,    2, 0x06 /* Public */,
      27,    1,  175,    2, 0x06 /* Public */,
      30,    1,  178,    2, 0x06 /* Public */,
      31,    2,  181,    2, 0x06 /* Public */,
      33,    0,  186,    2, 0x06 /* Public */,
      34,    4,  187,    2, 0x06 /* Public */,
      39,    3,  196,    2, 0x06 /* Public */,
      43,    1,  203,    2, 0x06 /* Public */,
      44,    2,  206,    2, 0x06 /* Public */,
      48,    0,  211,    2, 0x06 /* Public */,
      49,    0,  212,    2, 0x06 /* Public */,
      50,    1,  213,    2, 0x06 /* Public */,
      53,    1,  216,    2, 0x06 /* Public */,
      56,    2,  219,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      58,    0,  224,    2, 0x08 /* Private */,
      59,    0,  225,    2, 0x08 /* Private */,
      60,    0,  226,    2, 0x08 /* Private */,
      61,    1,  227,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 10, QMetaType::QString,    8,   11,   12,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 14,    8,   15,
    QMetaType::Void, 0x80000000 | 17, 0x80000000 | 19,   18,   20,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 14, 0x80000000 | 22, 0x80000000 | 24, 0x80000000 | 10,    8,   15,   23,   25,   26,
    QMetaType::Void, 0x80000000 | 28,   29,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 10, QMetaType::Bool,   11,   32,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10, QMetaType::QString, QMetaType::QStringList, 0x80000000 | 10,   35,   36,   37,   38,
    QMetaType::Void, 0x80000000 | 22, 0x80000000 | 22, 0x80000000 | 41,   40,   23,   42,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 22, 0x80000000 | 46,   45,   47,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 51,   52,
    QMetaType::Void, 0x80000000 | 54,   55,
    QMetaType::Void, 0x80000000 | 7, QMetaType::QString,    8,   57,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 62,    5,

       0        // eod
};

void NetworkManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NetworkManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->connectionError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->registerResponse((*reinterpret_cast< StatusCode(*)>(_a[1]))); break;
        case 4: _t->loginResponse((*reinterpret_cast< StatusCode(*)>(_a[1])),(*reinterpret_cast< uint32_t(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 5: _t->createRoomResponse((*reinterpret_cast< StatusCode(*)>(_a[1])),(*reinterpret_cast< const RoomInfo(*)>(_a[2]))); break;
        case 6: _t->listRoomsResponse((*reinterpret_cast< uint16_t(*)>(_a[1])),(*reinterpret_cast< const QVector<RoomInfo>(*)>(_a[2]))); break;
        case 7: _t->joinRoomResponse((*reinterpret_cast< StatusCode(*)>(_a[1])),(*reinterpret_cast< const RoomInfo(*)>(_a[2])),(*reinterpret_cast< uint8_t(*)>(_a[3])),(*reinterpret_cast< const QVector<PlayerInfo>(*)>(_a[4])),(*reinterpret_cast< uint32_t(*)>(_a[5]))); break;
        case 8: _t->playerJoinedNotif((*reinterpret_cast< const PlayerInfo(*)>(_a[1]))); break;
        case 9: _t->playerLeftNotif((*reinterpret_cast< uint32_t(*)>(_a[1]))); break;
        case 10: _t->readyStatusNotif((*reinterpret_cast< uint32_t(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 11: _t->gameStartNotif(); break;
        case 12: _t->questionNotif((*reinterpret_cast< uint32_t(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QStringList(*)>(_a[3])),(*reinterpret_cast< uint32_t(*)>(_a[4]))); break;
        case 13: _t->roundResultNotif((*reinterpret_cast< uint8_t(*)>(_a[1])),(*reinterpret_cast< uint8_t(*)>(_a[2])),(*reinterpret_cast< const QVector<PlayerRoundResult>(*)>(_a[3]))); break;
        case 14: _t->playerEliminatedNotif((*reinterpret_cast< uint32_t(*)>(_a[1]))); break;
        case 15: _t->gameOverNotif((*reinterpret_cast< uint8_t(*)>(_a[1])),(*reinterpret_cast< const QVector<PlayerFinalResult>(*)>(_a[2]))); break;
        case 16: _t->gamePausedNotif(); break;
        case 17: _t->gameResumedNotif(); break;
        case 18: _t->gameTerminatedNotif((*reinterpret_cast< TerminationReason(*)>(_a[1]))); break;
        case 19: _t->statsResponse((*reinterpret_cast< const UserStatsResponse(*)>(_a[1]))); break;
        case 20: _t->errorResponse((*reinterpret_cast< StatusCode(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 21: _t->onConnected(); break;
        case 22: _t->onDisconnected(); break;
        case 23: _t->onReadyRead(); break;
        case 24: _t->onSocketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 24:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (NetworkManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::connected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::disconnected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::connectionError)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(StatusCode );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::registerResponse)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(StatusCode , uint32_t , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::loginResponse)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(StatusCode , const RoomInfo & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::createRoomResponse)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(uint16_t , const QVector<RoomInfo> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::listRoomsResponse)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(StatusCode , const RoomInfo & , uint8_t , const QVector<PlayerInfo> & , uint32_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::joinRoomResponse)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(const PlayerInfo & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::playerJoinedNotif)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(uint32_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::playerLeftNotif)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(uint32_t , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::readyStatusNotif)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::gameStartNotif)) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(uint32_t , const QString & , const QStringList & , uint32_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::questionNotif)) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(uint8_t , uint8_t , const QVector<PlayerRoundResult> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::roundResultNotif)) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(uint32_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::playerEliminatedNotif)) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(uint8_t , const QVector<PlayerFinalResult> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::gameOverNotif)) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::gamePausedNotif)) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::gameResumedNotif)) {
                *result = 17;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(TerminationReason );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::gameTerminatedNotif)) {
                *result = 18;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(const UserStatsResponse & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::statsResponse)) {
                *result = 19;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(StatusCode , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkManager::errorResponse)) {
                *result = 20;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject NetworkManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_NetworkManager.data,
    qt_meta_data_NetworkManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *NetworkManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NetworkManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int NetworkManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 25)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 25;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 25)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 25;
    }
    return _id;
}

// SIGNAL 0
void NetworkManager::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void NetworkManager::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void NetworkManager::connectionError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NetworkManager::registerResponse(StatusCode _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void NetworkManager::loginResponse(StatusCode _t1, uint32_t _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void NetworkManager::createRoomResponse(StatusCode _t1, const RoomInfo & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void NetworkManager::listRoomsResponse(uint16_t _t1, const QVector<RoomInfo> & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void NetworkManager::joinRoomResponse(StatusCode _t1, const RoomInfo & _t2, uint8_t _t3, const QVector<PlayerInfo> & _t4, uint32_t _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void NetworkManager::playerJoinedNotif(const PlayerInfo & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void NetworkManager::playerLeftNotif(uint32_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void NetworkManager::readyStatusNotif(uint32_t _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void NetworkManager::gameStartNotif()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void NetworkManager::questionNotif(uint32_t _t1, const QString & _t2, const QStringList & _t3, uint32_t _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void NetworkManager::roundResultNotif(uint8_t _t1, uint8_t _t2, const QVector<PlayerRoundResult> & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void NetworkManager::playerEliminatedNotif(uint32_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void NetworkManager::gameOverNotif(uint8_t _t1, const QVector<PlayerFinalResult> & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void NetworkManager::gamePausedNotif()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void NetworkManager::gameResumedNotif()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}

// SIGNAL 18
void NetworkManager::gameTerminatedNotif(TerminationReason _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}

// SIGNAL 19
void NetworkManager::statsResponse(const UserStatsResponse & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 19, _a);
}

// SIGNAL 20
void NetworkManager::errorResponse(StatusCode _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
