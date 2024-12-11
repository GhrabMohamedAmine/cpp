QT       += core gui charts serialport
QT       += core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../connecton/Souhir/Souhir/Souhir/Atelier_Connexion (9)/Atelier_Connexion (9)/Atelier_Connexion/client.cpp \
    ../connecton/Souhir/Souhir/Souhir/Atelier_Connexion (9)/Atelier_Connexion (9)/Atelier_Connexion/mainwindow.cpp \
    ../connecton/Souhir/Souhir/Souhir/Atelier_Connexion (9)/Atelier_Connexion (9)/Atelier_Connexion/qrcodegen.cpp \
    ../connecton/Souhir/Souhir/Souhir/Atelier_Connexion (9)/Atelier_Connexion (9)/Atelier_Connexion/rfidmonitor.cpp \
    connection.cpp \
    event.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ../connecton/Souhir/Souhir/Souhir/Atelier_Connexion (9)/Atelier_Connexion (9)/Atelier_Connexion/client.h \
    ../connecton/Souhir/Souhir/Souhir/Atelier_Connexion (9)/Atelier_Connexion (9)/Atelier_Connexion/mainwindow.h \
    ../connecton/Souhir/Souhir/Souhir/Atelier_Connexion (9)/Atelier_Connexion (9)/Atelier_Connexion/qrcodegen.hpp \
    ../connecton/Souhir/Souhir/Souhir/Atelier_Connexion (9)/Atelier_Connexion (9)/Atelier_Connexion/rfidmonitor.h \
    connection.h \
    event.h \
    mainwindow.h

FORMS += \
    ../connecton/Souhir/Souhir/Souhir/Atelier_Connexion (9)/Atelier_Connexion (9)/Atelier_Connexion/mainwindow.ui \
    ../connecton/Souhir/Souhir/Souhir/Atelier_Connexion (9)/Atelier_Connexion (9)/Atelier_Connexion/rfidmonitor.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
