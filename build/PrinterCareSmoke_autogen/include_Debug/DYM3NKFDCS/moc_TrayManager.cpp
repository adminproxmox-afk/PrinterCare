/****************************************************************************
** Meta object code from reading C++ file 'TrayManager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../include/managers/TrayManager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TrayManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN11printercare8managers11TrayManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto printercare::managers::TrayManager::qt_create_metaobjectdata<qt_meta_tag_ZN11printercare8managers11TrayManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "printercare::managers::TrayManager",
        "openRequested",
        "",
        "markTestPrintRequested",
        "markHeadCleaningRequested",
        "historyRequested",
        "aboutRequested",
        "showRequested",
        "settingsRequested",
        "exitRequested",
        "notificationClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'openRequested'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'markTestPrintRequested'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'markHeadCleaningRequested'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'historyRequested'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'aboutRequested'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'showRequested'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'settingsRequested'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'exitRequested'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'notificationClicked'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<TrayManager, qt_meta_tag_ZN11printercare8managers11TrayManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject printercare::managers::TrayManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11printercare8managers11TrayManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11printercare8managers11TrayManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11printercare8managers11TrayManagerE_t>.metaTypes,
    nullptr
} };

void printercare::managers::TrayManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<TrayManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->openRequested(); break;
        case 1: _t->markTestPrintRequested(); break;
        case 2: _t->markHeadCleaningRequested(); break;
        case 3: _t->historyRequested(); break;
        case 4: _t->aboutRequested(); break;
        case 5: _t->showRequested(); break;
        case 6: _t->settingsRequested(); break;
        case 7: _t->exitRequested(); break;
        case 8: _t->notificationClicked(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (TrayManager::*)()>(_a, &TrayManager::openRequested, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (TrayManager::*)()>(_a, &TrayManager::markTestPrintRequested, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (TrayManager::*)()>(_a, &TrayManager::markHeadCleaningRequested, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (TrayManager::*)()>(_a, &TrayManager::historyRequested, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (TrayManager::*)()>(_a, &TrayManager::aboutRequested, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (TrayManager::*)()>(_a, &TrayManager::showRequested, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (TrayManager::*)()>(_a, &TrayManager::settingsRequested, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (TrayManager::*)()>(_a, &TrayManager::exitRequested, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (TrayManager::*)()>(_a, &TrayManager::notificationClicked, 8))
            return;
    }
}

const QMetaObject *printercare::managers::TrayManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *printercare::managers::TrayManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11printercare8managers11TrayManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int printercare::managers::TrayManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void printercare::managers::TrayManager::openRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void printercare::managers::TrayManager::markTestPrintRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void printercare::managers::TrayManager::markHeadCleaningRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void printercare::managers::TrayManager::historyRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void printercare::managers::TrayManager::aboutRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void printercare::managers::TrayManager::showRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void printercare::managers::TrayManager::settingsRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void printercare::managers::TrayManager::exitRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void printercare::managers::TrayManager::notificationClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}
QT_WARNING_POP
