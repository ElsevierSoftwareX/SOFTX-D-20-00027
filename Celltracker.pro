TEMPLATE = app

QT += qml quick xml

QMAKE_CXXFLAGS += -O0 -g -std=c++11 -Wall -Wextra -pedantic
LIBS += -lhdf5 -lhdf5_cpp
MAKEFLAGS += -j4

SOURCES += main.cpp \
    src/base_data/movie.cpp \
    src/base_data/frame.cpp \
    src/base_data/slice.cpp \
    src/base_data/channel.cpp \
    src/base_data/object.cpp \
    src/auto_tracklets/autotracklet.cpp \
    src/corrected_data/genealogy.cpp \
    src/corrected_data/trackelement.cpp \
    src/corrected_data/annotation.cpp \
    src/corrected_data/tracklet.cpp \
    src/corrected_data/trackevent.cpp \
    src/corrected_data/trackeventdivision.cpp \
    src/corrected_data/trackeventmerge.cpp \
    src/corrected_data/trackeventunmerge.cpp \
    src/corrected_data/trackeventlost.cpp \
    src/corrected_data/trackeventdead.cpp \
    src/corrected_data/trackletregular.cpp \
    src/corrected_data/trackletmerged.cpp \
    src/corrected_data/trackletsplit.cpp \
    src/import/import.cpp \
    src/import/importxml.cpp \
    src/project.cpp \
    src/import/importhdf5.cpp \
    src/base_data/info.cpp \
    imageprovider.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    src/base_data/movie.h \
    src/base_data/frame.h \
    src/base_data/slice.h \
    src/base_data/channel.h \
    src/base_data/object.h \
    src/auto_tracklets/autotracklet.h \
    src/corrected_data/genealogy.h \
    src/corrected_data/trackelement.h \
    src/corrected_data/annotation.h \
    src/corrected_data/tracklet.h \
    src/corrected_data/trackevent.h \
    src/corrected_data/trackeventdivision.h \
    src/corrected_data/trackeventmerge.h \
    src/corrected_data/trackeventunmerge.h \
    src/corrected_data/trackeventlost.h \
    src/corrected_data/trackeventdead.h \
    src/corrected_data/trackletregular.h \
    src/corrected_data/trackletmerged.h \
    src/corrected_data/trackletsplit.h \
    src/import/import.h \
    src/import/importxml.h \
    src/project.h \
    src/import/importhdf5.h \
    src/base_data/info.h \
    imageprovider.h
