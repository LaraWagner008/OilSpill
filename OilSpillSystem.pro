QT += core gui widgets charts sql

RESOURCES += resources.qrc

CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    oilspillwindow.cpp \
    database/databasemanager.cpp \
    calculations/uavcalculator.cpp \
    calculations/helicoptercalculator.cpp \
    calculations/airplanecalculator.cpp \
    calculations/operationcalculator.cpp

HEADERS += \
    mainwindow.h \
    oilspillwindow.h \
    database/databasemanager.h \
    models/environmentdata.h \
    models/aircraftdata.h \
    models/operationresult.h \
    calculations/uavcalculator.h \
    calculations/helicoptercalculator.h \
    calculations/airplanecalculator.h \
    calculations/operationcalculator.h

TARGET = OilSpillSystem

TEMPLATE = app
