QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    billpay.cpp \
    dashboard.cpp \
    database.cpp \
    deposit.cpp \
    fundtransfer.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    profile.cpp \
    register.cpp

HEADERS += \
    billpay.h \
    dashboard.h \
    database.h \
    deposit.h \
    fundtransfer.h \
    login.h \
    mainwindow.h \
    profile.h \
    register.h

FORMS += \
    billpay.ui \
    dashboard.ui \
    deposit.ui \
    fundtransfer.ui \
    login.ui \
    mainwindow.ui \
    profile.ui \
    register.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource.qrc
