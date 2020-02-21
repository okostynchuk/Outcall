#-------------------------------------------------
#
# Project created by QtCreator 2015-03-16T10:53:56
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += sql
QT       += widgets

win32:{
    VERSION = 1.0.0
    QMAKE_TARGET_COMPANY = Vybor
    QMAKE_TARGET_PRODUCT = OutCALL
}

TARGET = OutCALL
TEMPLATE = app

SOURCES += main.cpp\
    DebugInfoDialog.cpp \
    AddExtensionDialog.cpp \
    AsteriskManager.cpp \
    CallHistoryDialog.cpp \
    Global.cpp \
    OutCALL.cpp \
    PlaceCallDialog.cpp \
    SettingsDialog.cpp \
    PopupWindow.cpp \
    Notifier.cpp \
    ContactsDialog.cpp \
    AddContactDialog.cpp \
    PopupHelloWindow.cpp \
    AddOrgContactDialog.cpp \
    ViewContactDialog.cpp \
    ViewOrgContactDialog.cpp \
    EditContactDialog.cpp \
    EditOrgContactDialog.cpp \
    AddNoteDialog.cpp \
    ChooseNumber.cpp \
    DatabasesConnectDialog.cpp

HEADERS  += \
    DebugInfoDialog.h \
    AsteriskManager.h \
    CallHistoryDialog.h \
    Global.h \
    OutCALL.h \
    PlaceCallDialog.h \
    AddExtensionDialog.h \
    SettingsDialog.h \
    PopupWindow.h \
    Notifier.h \
    ContactsDialog.h \
    AddContactDialog.h \
    PopupHelloWindow.h \
    AddOrgContactDialog.h \
    ViewContactDialog.h \
    ViewOrgContactDialog.h \
    EditContactDialog.h \
    EditOrgContactDialog.h \
    AddNoteDialog.h \
    ChooseNumber.h \
    DatabasesConnectDialog.h

FORMS    += \
    AddExtensionDialog.ui \
    CallHistoryDialog.ui \
    DebugInfoDialog.ui \
    PlaceCallDialog.ui \
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
    AddNoteDialog.ui \
    ChooseNumber.ui \
    DatabasesConnectDialog.ui

RESOURCES += \
    images.qrc \
    langs.qrc

TRANSLATIONS = ukrainian.ts russian.ts english.ts

win32: RC_ICONS = $$PWD/images/outcall-logo.ico
