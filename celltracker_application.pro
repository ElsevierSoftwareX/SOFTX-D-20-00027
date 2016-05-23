TEMPLATE = app

TARGET = Celltracker
QT += qml quick xml
QMAKE_INCDIR += src/

# clang supports these warnings
#QMAKE_CXXFLAGS_DEBUG += -O0 -g -std=c++11 -Wall -Wextra -pedantic -Wdeprecated -Wimplicit-fallthrough -Wmissing-noreturn -Wunused-exception-parameter -Wunreachable-code-return -Wunreachable-code-break -Wswitch-enum -Wcovered-switch-default -Wdocumentation -Wextra-semi
# these warings are supported by clang and g++
QMAKE_CXXFLAGS_DEBUG += -O0 -g -std=c++11 -Wall -Wextra -pedantic -Wdeprecated -Wmissing-noreturn -Wunreachable-code -Wswitch-enum
QMAKE_CXXFLAGS_RELEASE += -O0 -g -std=c++11 -Wall -Wextra -pedantic

LIBS += -lhdf5 -lhdf5_cpp

version.target = src/version.h
version.depends = FORCE
version.commands = cd $$PWD; ./version.sh > src/version.h

macx
{
    CONFIG += c++11
    INCLUDEPATH += /opt/local/include
    LIBS += -L/opt/local/lib
}

PRE_TARGETDEPS += src/version.h
QMAKE_EXTRA_TARGETS += version

SOURCES += main.cpp \
    src/project.cpp \
    src/exceptions/ctexception.cpp \
    src/exceptions/ctimportexception.cpp \
    src/exceptions/ctformatexception.cpp \
    src/exceptions/ctdataexception.cpp \
    src/exceptions/ctmissingelementexception.cpp \
    src/examples/examplewriteallimages.cpp \
    src/examples/exampleloadprojecthdf5.cpp \
    src/examples/examplewriteoutlinetognuplot.cpp \
    src/examples/exampleloadprojectxml.cpp \
    src/examples/exampleaddtrackdivision.cpp \
    src/examples/exampleaddannotation.cpp \
    src/examples/exampleobject.cpp \
    src/examples/exampleidprovider.cpp \
    src/provider/messagerelay.cpp \
    src/examples/examplesignalslot.cpp \
    src/examples/examplewritehdf5.cpp \
    src/examples/examplewriteprojectstdout.cpp \
    src/exceptions/ctexportexception.cpp \
    src/provider/dataprovider.cpp \
    src/provider/ctsettings.cpp \
    src/provider/guistate.cpp \
    src/provider/guicontroller.cpp \
    src/exceptions/ctunimplementedexception.cpp \
    src/provider/imageprovider.cpp \
    src/io/export.cpp \
    src/io/exporthdf5.cpp \
    src/io/import.cpp \
    src/io/importhdf5.cpp \
    src/io/importxml.cpp \
    src/io/hdf5_aux.cpp \
    src/base/autotracklet.cpp \
    src/base/channel.cpp \
    src/base/frame.cpp \
    src/base/info.cpp \
    src/base/movie.cpp \
    src/base/object.cpp \
    src/base/slice.cpp \
    src/tracked/annotateable.cpp \
    src/tracked/annotation.cpp \
    src/tracked/genealogy.cpp \
    src/tracked/trackevent.cpp \
    src/tracked/trackeventdead.cpp \
    src/tracked/trackeventdivision.cpp \
    src/tracked/trackeventendofmovie.cpp \
    src/tracked/trackeventlost.cpp \
    src/tracked/trackeventmerge.cpp \
    src/tracked/trackeventunmerge.cpp \
    src/tracked/tracklet.cpp \
    src/provider/idprovider.cpp \
    src/exceptions/ctdependencyexception.cpp \
    src/graphics/merge.cpp \
    src/graphics/separate.cpp \
    src/examples/examplesaveallcombinations.cpp \
    src/examples/exampleloadmultipletimes.cpp \
    src/examples/exampletestmemoryleak.cpp

RESOURCES += qml.qrc

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    src/project.h \
    src/exceptions/ctexception.h \
    src/exceptions/ctimportexception.h \
    src/exceptions/ctformatexception.h \
    src/exceptions/ctdataexception.h \
    src/exceptions/ctmissingelementexception.h \
    src/examples/examples.h \
    src/provider/messagerelay.h \
    src/exceptions/ctexportexception.h \
    src/provider/dataprovider.h \
    src/provider/ctsettings.h \
    src/provider/guistate.h \
    src/provider/guicontroller.h \
    src/exceptions/ctunimplementedexception.h \
    src/provider/imageprovider.h \
    src/io/export.h \
    src/io/exporthdf5.h \
    src/io/import.h \
    src/io/importhdf5.h \
    src/io/importxml.h \
    src/io/hdf5_aux.h \
    src/base/autotracklet.h \
    src/base/channel.h \
    src/base/frame.h \
    src/base/info.h \
    src/base/movie.h \
    src/base/object.h \
    src/base/slice.h \
    src/tracked/annotateable.h \
    src/tracked/annotation.h \
    src/tracked/genealogy.h \
    src/provider/idprovider.h \
    src/tracked/trackevent.h \
    src/tracked/trackeventdead.h \
    src/tracked/trackeventdivision.h \
    src/tracked/trackeventendofmovie.h \
    src/tracked/trackeventlost.h \
    src/tracked/trackeventmerge.h \
    src/tracked/trackeventunmerge.h \
    src/tracked/tracklet.h \
    src/exceptions/ctdependencyexception.h \
    src/graphics/merge.h \
    src/graphics/separate.h \
    src/version.h

DISTFILES += \
    README.md
