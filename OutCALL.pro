QT += core
QT += gui
QT += network
QT += sql
QT += widgets
QT += multimedia
QT += winextras
QT += axcontainer

include ($$PWD/QSimpleUpdater/QSimpleUpdater.pri)

win32:
{
    VERSION = 3.0.2

    QMAKE_TARGET_COMPANY = Team Nonames
    QMAKE_TARGET_PRODUCT = OutCALL

    RC_ICONS = $$PWD/src/images/outcall-logo.ico
}

TARGET = OutCALL
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
    OutCALL.cpp \
    PlaceCallDialog.cpp \
    PlayAudioDialog.cpp \
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
    RemindersThread.cpp \
    NotesDialog.cpp \
    EditReminderDialog.cpp \
    PopupReminder.cpp \
    ChooseEmployee.cpp \
    InternalContactsDialog.cpp

HEADERS += \
    AddReminderDialog.h \
    DebugInfoDialog.h \
    AsteriskManager.h \
    CallHistoryDialog.h \
    Global.h \
    OutCALL.h \
    PlaceCallDialog.h \
    AddExtensionDialog.h \
    PlayAudioDialog.h \
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
    RemindersThread.h \
    NotesDialog.h \
    EditReminderDialog.h \
    PopupReminder.h \
    ChooseEmployee.h \
    InternalContactsDialog.h

FORMS += \
    AddExtensionDialog.ui \
    AddReminderDialog.ui \
    CallHistoryDialog.ui \
    DebugInfoDialog.ui \
    PlaceCallDialog.ui \
    PlayAudioDialog.ui \
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
    InternalContactsDialog.ui

RESOURCES += \
    images.qrc \
    translations.qrc \
