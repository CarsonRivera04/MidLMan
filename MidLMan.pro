QT       += \
    core gui \
    multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    MidiFile.cpp \
    Binasc.cpp \
    MidiEvent.cpp \
    MidiEventList.cpp

HEADERS += \
    mainwindow.h \
    MidiFile.h \
    Binasc.h \
    MidiEvent.h \
    MidiEventList.h

FORMS += \
    mainwindow.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


macx: LIBS += -L$$PWD/../midifile/lib/ -lmidifile

INCLUDEPATH += $$PWD/../midifile/lib
DEPENDPATH += $$PWD/../midifile/lib

macx: PRE_TARGETDEPS += $$PWD/../midifile/lib/libmidifile.a




