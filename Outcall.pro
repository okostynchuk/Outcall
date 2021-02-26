QT += core
QT += gui
QT += sql
QT += network
QT += widgets
QT += multimedia

include($$PWD/QSimpleUpdater/QSimpleUpdater.pri)

VERSION = 1.0.5

QMAKE_TARGET_PRODUCT = Outcall
QMAKE_TARGET_COMPANY = Team Nonames

win32 {
   RC_ICONS = $$PWD/src/images/outcall-logo.ico
}
win32-g++ {
   QMAKE_CXXFLAGS += -Werror
}

TARGET = Outcall
TEMPLATE = app

CONFIG += c++11

VPATH += $$PWD/src

SOURCES += \
    main.cpp \
    AddReminderDialog.cpp \
    DebugInfoDialog.cpp \
    AddExtensionDialog.cpp \
    AsteriskManager.cpp \
    CallHistoryDialog.cpp \
    Global.cpp \
    Outcall.cpp \
    PlaceCallDialog.cpp \
    PopupNotification.cpp \
    QSqlQueryModelReminders.cpp \
    SettingsDialog.cpp \
    PopupWindow.cpp \
    ContactsDialog.cpp \
    AddContactDialog.cpp \
    PopupHelloWindow.cpp \
    AddOrgContactDialog.cpp \
    ViewContactDialog.cpp \
    ViewOrgContactDialog.cpp \
    EditContactDialog.cpp \
    EditOrgContactDialog.cpp \
    ChooseNumber.cpp \
    DatabasesConnectDialog.cpp \
    AddOrgToPerson.cpp \
    AddPhoneNumberToContactDialog.cpp \
    AddPersonToOrg.cpp \
    RemindersDialog.cpp \
    RemindersThreadManager.cpp \
    NotesDialog.cpp \
    EditReminderDialog.cpp \
    PopupReminder.cpp \
    ChooseEmployee.cpp \
    InternalContactsDialog.cpp \
    QCustomWidget.cpp \
    Player.cpp

HEADERS += \
    AddReminderDialog.h \
    DebugInfoDialog.h \
    AsteriskManager.h \
    CallHistoryDialog.h \
    Global.h \
    Outcall.h \
    PlaceCallDialog.h \
    AddExtensionDialog.h \
    PopupNotification.h \
    QSqlQueryModelReminders.h \
    SettingsDialog.h \
    PopupWindow.h \
    ContactsDialog.h \
    AddContactDialog.h \
    PopupHelloWindow.h \
    AddOrgContactDialog.h \
    ViewContactDialog.h \
    ViewOrgContactDialog.h \
    EditContactDialog.h \
    EditOrgContactDialog.h \
    ChooseNumber.h \
    DatabasesConnectDialog.h \
    AddOrgToPerson.h \
    AddPhoneNumberToContactDialog.h \
    AddPersonToOrg.h \
    RemindersDialog.h \
    RemindersThreadManager.h \
    NotesDialog.h \
    EditReminderDialog.h \
    PopupReminder.h \
    ChooseEmployee.h \
    InternalContactsDialog.h \
    QCustomWidget.h \
    Player.h

FORMS += \
    AddExtensionDialog.ui \
    AddReminderDialog.ui \
    CallHistoryDialog.ui \
    DebugInfoDialog.ui \
    PlaceCallDialog.ui \
    PopupNotification.ui \
    SettingsDialog.ui \
    PopupWindow.ui \
    ContactsDialog.ui \
    AddContactDialog.ui \
    PopupHelloWindow.ui \
    AddOrgContactDialog.ui \
    ViewContactDialog.ui \
    ViewOrgContactDialog.ui \
    EditContactDialog.ui \
    EditOrgContactDialog.ui \
    ChooseNumber.ui \
    DatabasesConnectDialog.ui \
    AddOrgToPerson.ui \
    AddPhoneNumberToContactDialog.ui \
    AddPersonToOrg.ui \
    RemindersDialog.ui \
    NotesDialog.ui \
    EditReminderDialog.ui \
    PopupReminder.ui \
    ChooseEmployee.ui \
    InternalContactsDialog.ui \
    Player.ui

RESOURCES += \
    src/images.qrc \
    src/translations.qrc \
