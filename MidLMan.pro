QT       += \
    core gui \
    multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    sources/main.cpp \
    sources/mainwindow.cpp \
    midifile-library/MidiFile.cpp \
    midifile-library/Binasc.cpp \
    midifile-library/MidiEvent.cpp \
    midifile-library/MidiEventList.cpp

HEADERS += \
    sources/mainwindow.h \
    midifile-library/MidiFile.h \
    midifile-library/Binasc.h \
    midifile-library/MidiEvent.h \
    midifile-library/MidiEventList.h

FORMS += \
    sources/mainwindow.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./release/ -lmidifile
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./debug/ -lmidifile
else:unix: LIBS += -L$$PWD/./ -lmidifile

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.
