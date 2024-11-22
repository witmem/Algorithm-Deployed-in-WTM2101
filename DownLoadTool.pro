QT       += core gui printsupport
QT       += serialport
VERSION = 0.0.1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
DEFINES += QT_MESSAGELOGCONTEXT
CONFIG += c++17
TARGET = 2302DownLoadTool
TEMPLATE = app

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CH347I2CInterfaceWorker.cpp \
    CH347SpiInterfaceWorker.cpp \
    DownLoadMainWindow.cpp \
    FileDealManger.cpp \
    PassObjectFileDialog.cpp \
    UARTserialportWorker.cpp \
    main.cpp

HEADERS += \
    CH347I2CInterfaceWorker.h \
    CH347SpiInterfaceWorker.h \
    DataDefine.h \
    DownLoadMainWindow.h \
    FileDealManger.h \
    PassObjectFileDialog.h \
    UARTserialportWorker.h \
    lib/CH347DLL.H \
    lib/WinTypes.h \
    lib/ftd2xx.h \
    lib/libmpsse_i2c.h \
    lib/libmpsse_spi.h

FORMS += \
    DownLoadMainWindow.ui \
    PassObjectFileDialog.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx|win32: LIBS += -L$$PWD/lib/ -llibmpsse -lftd2xx -lCH347DLLA64

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

RESOURCES += \
    Resource.qrc

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.
