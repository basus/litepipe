/****************************************************************************
** Meta object code from reading C++ file 'litepipe_client.h'
**
** Created: Wed May 5 20:46:24 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "litepipe_client.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'litepipe_client.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_litepipe_client[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      29,   17,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      71,   16,   16,   16, 0x08,
      80,   16,   16,   16, 0x08,
      92,   16,   16,   16, 0x08,
     101,   16,   16,   16, 0x08,
     118,   16,   16,   16, 0x08,
     135,   16,   16,   16, 0x08,
     157,   17,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_litepipe_client[] = {
    "litepipe_client\0\0status,data\0"
    "handleCommunicationEventSignal(int,void*)\0"
    "goBack()\0goForward()\0reload()\0"
    "infoActive(bool)\0httpActive(bool)\0"
    "connectToServer(bool)\0"
    "handleCommunicationEventSlot(int,void*)\0"
};

const QMetaObject litepipe_client::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_litepipe_client,
      qt_meta_data_litepipe_client, 0 }
};

const QMetaObject *litepipe_client::metaObject() const
{
    return &staticMetaObject;
}

void *litepipe_client::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_litepipe_client))
        return static_cast<void*>(const_cast< litepipe_client*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int litepipe_client::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: handleCommunicationEventSignal((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< void*(*)>(_a[2]))); break;
        case 1: goBack(); break;
        case 2: goForward(); break;
        case 3: reload(); break;
        case 4: infoActive((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: httpActive((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: connectToServer((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: handleCommunicationEventSlot((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< void*(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void litepipe_client::handleCommunicationEventSignal(int _t1, void * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
