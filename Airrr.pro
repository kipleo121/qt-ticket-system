QT += core gui
QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AdminFlightManager.cpp \
    AdminLoginDialog.cpp \
    Air_inquiryForm.cpp \
    LoginForm.cpp \
    MainForm.cpp \
    MyOrders.cpp \
    PersonalInfoWidget.cpp \
    RegisterForm.cpp \
    dbhelper.cpp \
    filterdialog.cpp \
    main.cpp

HEADERS += \
    AdminFlightManager.h \
    AdminLoginDialog.h \
    Air_inquiryForm.h \
    LoginForm.h \
    MainForm.h \
    MyOrders.h \
    PersonalInfoWidget.h \
    RegisterForm.h \
    dbhelper.h \
    filterdialog.h

FORMS += \
    AdminFlightManager.ui \
    AdminLoginDialog.ui \
    Air_inquiryForm.ui \
    LoginForm.ui \
    MainForm.ui \
    MyOrders.ui \
    PersonalInfoWidget.ui \
    RegisterForm.ui \
    filterdialog.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc





