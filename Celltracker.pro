TEMPLATE = app

QT += qml quick xml
QMAKE_INCDIR += src/

QMAKE_CXXFLAGS_DEBUG += -O0 -g -std=c++11 -Wall -Wunused -Wextra -pedantic
QMAKE_CXXFLAGS_RELEASE += -O2 -std=c++11 -Wall -Wunused -Wextra -pedantic

LIBS += -lhdf5 -lhdf5_cpp

macx
{
    CONFIG += c++11
    INCLUDEPATH += /opt/local/include
    LIBS += -L/opt/local/lib
}

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
    src/import/import.cpp \
    src/import/importxml.cpp \
    src/project.cpp \
    src/import/importhdf5.cpp \
    src/base_data/info.cpp \
    src/exceptions/ctexception.cpp \
    src/exceptions/ctimportexception.cpp \
    src/exceptions/ctformatexception.cpp \
    src/exceptions/ctdataexception.cpp \
    src/provider/imageprovider.cpp \
    src/exceptions/ctmissingelementexception.cpp \
    src/corrected_data/annotateable.cpp \
    src/examples/examplewriteallimages.cpp \
    src/examples/exampleloadprojecthdf5.cpp \
    src/examples/examplewriteoutlinetognuplot.cpp \
    src/examples/exampleloadprojectxml.cpp \
    src/examples/exampleaddtrackdivision.cpp \
    src/examples/exampleaddannotation.cpp \
    src/examples/exampleobject.cpp \
    src/examples/exampleidprovider.cpp \
    src/corrected_data/idprovider.cpp \
    src/provider/messagerelay.cpp \
    src/examples/examplesignalslot.cpp \
    src/export/export.cpp \
    src/export/exporthdf5.cpp \
    src/examples/examplewritehdf5.cpp \
    src/examples/examplewriteprojectstdout.cpp \
    src/exceptions/ctexportexception.cpp \
    src/provider/dataprovider.cpp \
    src/provider/imageprovider2.cpp \
    src/provider/ctsettings.cpp \
    src/provider/guistate.cpp \
    src/provider/guicontroller.cpp \
    src/exceptions/ctunimplementedexception.cpp

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
    src/corrected_data/annotation.h \
    src/corrected_data/trackelement.h \
    src/corrected_data/tracklet.h \
    src/corrected_data/trackevent.h \
    src/corrected_data/trackeventdivision.h \
    src/corrected_data/trackeventmerge.h \
    src/corrected_data/trackeventunmerge.h \
    src/corrected_data/trackeventlost.h \
    src/corrected_data/trackeventdead.h \
    src/import/import.h \
    src/import/importxml.h \
    src/project.h \
    src/import/importhdf5.h \
    src/base_data/info.h \
    src/exceptions/ctexception.h \
    src/exceptions/ctimportexception.h \
    src/exceptions/ctformatexception.h \
    src/exceptions/ctdataexception.h \
    src/provider/imageprovider.h \
    src/exceptions/ctmissingelementexception.h \
    src/corrected_data/annotateable.h \
    src/examples/examples.h \
    src/corrected_data/idprovider.h \
    src/provider/messagerelay.h \
    src/export/export.h \
    src/export/exporthdf5.h \
    src/exceptions/ctexportexception.h \
    src/provider/dataprovider.h \
    src/provider/imageprovider2.h \
    src/provider/ctsettings.h \
    src/provider/guistate.h \
    src/provider/guicontroller.h \
    src/exceptions/ctunimplementedexception.h
