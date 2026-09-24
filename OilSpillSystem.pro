QT += core gui widgets charts sql

RESOURCES += resources.qrc

CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    oilspillwindow.cpp \
    database/databasemanager.cpp

HEADERS += \
    mainwindow.h \
    oilspillwindow.h \
    database/databasemanager.h \
    models/environmentdata.h \
    models/aircraftdata.h \
    models/operationresult.h

TARGET = OilSpillSystem

TEMPLATE = app
