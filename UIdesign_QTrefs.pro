QT       += core gui sql core5compat widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    MainWidget.cpp \
    TitleBar.cpp \
    main.cpp \
    subFunc/Funcwidget.cpp \
    subFunc/GenDB_Widget.cpp \
    subFunc/Model/ACTreeModel.cpp \
    subFunc/Model/BrowserHistoryModel.cpp \
    subFunc/Model/FileHandler.cpp \
    subFunc/Model/KeyWordDbModel.cpp \
    subFunc/Model/SecretFileModel.cpp \
    subFunc/Model/SysInfoModel.cpp \
    subFunc/SecretScanWidget.cpp \
    subFunc/SysInfoWidget.cpp \
    subFunc/TraceCheckWidget.cpp

HEADERS += \
    MainWidget.h \
    TitleBar.h \
    subFunc/Funcwidget.h \
    subFunc/GenDB_Widget.h \
    subFunc/Model/ACTreeModel.h \
    subFunc/Model/BrowserHistoryModel.h \
    subFunc/Model/FileHandler.h \
    subFunc/Model/KeyWordDbModel.h \
    subFunc/Model/SecretFileModel.h \
    subFunc/Model/SysInfoModel.h \
    subFunc/SecretScanWidget.h \
    subFunc/SysInfoWidget.h \
    subFunc/TraceCheckWidget.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    QSS.qrc \
    WordLic.qrc \
    image.qrc

DISTFILES += \
    source/Image/Maximum.png \
    source/Image/Minimum.png \
    source/Image/exit.png \
    source/Image/hide.png \
    source/Image/icon.ico \
    source/Image/icon.png \
    source/lic/Aspose.Total.Product.Family.lic \
    source/qss/FuncWidget.css \
    source/qss/GenDB_Widget.css \
    source/qss/MainTitleBar.css \
    source/qss/MainWidget.css \
    source/qss/SecretScanWidget.css \
    source/qss/SysInfoWidget.css \
    source/qss/TraceCheckWidget.css
