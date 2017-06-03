TEMPLATE = app

TARGET = Celltracker
QT += qml quick svg xml
QMAKE_INCDIR += src/

# clang supports these warnings
#QMAKE_CXXFLAGS_DEBUG += -O0 -g -std=c++11 -Wall -Wextra -pedantic -Wdeprecated -Wimplicit-fallthrough -Wmissing-noreturn -Wunused-exception-parameter -Wunreachable-code-return -Wunreachable-code-break -Wswitch-enum -Wcovered-switch-default -Wdocumentation -Wextra-semi
# these warings are supported by clang and g++

QMAKE_CXXFLAGS_DEBUG += -O0 -g -std=c++11

!clang:debug {
        QMAKE_CXXFLAGS_DEBUG += -Wall
        QMAKE_CXXFLAGS_DEBUG += -Wextra
        QMAKE_CXXFLAGS_DEBUG += -pedantic
        QMAKE_CXXFLAGS_DEBUG += -Wdeprecated
        QMAKE_CXXFLAGS_DEBUG += -Wmissing-noreturn
        QMAKE_CXXFLAGS_DEBUG += -Wunreachable-code
        QMAKE_CXXFLAGS_DEBUG += -Wswitch-enum
} else:clang:debug {
        # FIXED:
        QMAKE_CXXFLAGS_DEBUG += -Wunreachable-code-break
        QMAKE_CXXFLAGS_DEBUG += -Wcovered-switch-default
        QMAKE_CXXFLAGS_DEBUG += -Wdouble-promotion
        QMAKE_CXXFLAGS_DEBUG += -Wmissing-variable-declarations
        QMAKE_CXXFLAGS_DEBUG += -Wmissing-prototypes
        QMAKE_CXXFLAGS_DEBUG += -Wold-style-cast
        QMAKE_CXXFLAGS_DEBUG += -Wshorten-64-to-32
        QMAKE_CXXFLAGS_DEBUG += -Wdocumentation
        QMAKE_CXXFLAGS_DEBUG += -Wundefined-func-template
        QMAKE_CXXFLAGS_DEBUG += -Wshadow
        QMAKE_CXXFLAGS_DEBUG += -Wshadow-field-in-constructor
        QMAKE_CXXFLAGS_DEBUG += -Wfloat-conversion
        QMAKE_CXXFLAGS_DEBUG += -Wfloat-equal

        # No C++98 compat, padded is also annoying
        QMAKE_CXXFLAGS_DEBUG += -Wno-c++98-compat
        QMAKE_CXXFLAGS_DEBUG += -Wno-c++98-compat-pedantic
        QMAKE_CXXFLAGS_DEBUG += -Wno-c++98-compat-local-type-template-args
        QMAKE_CXXFLAGS_DEBUG += -Wno-padded

        # Ignored due to Qt generating a file qrc_qml.cpp, that triggers these warnings
        QMAKE_CXXFLAGS_DEBUG += -Wno-exit-time-destructors
        QMAKE_CXXFLAGS_DEBUG += -Wno-global-constructors

        # MOC-files generate this warning by casting things to void **
        QMAKE_CXXFLAGS_DEBUG += -Wno-undefined-reinterpret-cast

        # Triggers too often on valid code
        QMAKE_CXXFLAGS_DEBUG += -Wno-disabled-macro-expansion

        # Maybe someday…
        QMAKE_CXXFLAGS_DEBUG += -Wno-weak-vtables
        QMAKE_CXXFLAGS_DEBUG += -Wno-conversion
        QMAKE_CXXFLAGS_DEBUG += -Wno-sign-conversion
}

QMAKE_CXXFLAGS_RELEASE += -O2 -g -std=c++11 -Wall -Wextra -pedantic

LIBS += -lhdf5_cpp -lhdf5

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
    src/provider/messagerelay.cpp \
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
    src/tracked/tracklet.cpp \
    src/provider/idprovider.cpp \
    src/exceptions/ctdependencyexception.cpp \
    src/graphics/merge.cpp \
    src/graphics/separate.cpp \
    src/io/modifyhdf5.cpp \
    src/graphics/base.cpp \
    src/graphics/floodfill.cpp \
    src/provider/timetracker.cpp

# examples
SOURCES += src/examples/examplewriteallimages.cpp \
    src/examples/exampleloadprojecthdf5.cpp \
    src/examples/examplewriteoutlinetognuplot.cpp \
    src/examples/exampleloadprojectxml.cpp \
    src/examples/exampleaddtrackdivision.cpp \
    src/examples/exampleaddannotation.cpp \
    src/examples/exampleobject.cpp \
    src/examples/exampleidprovider.cpp \
    src/examples/examplesignalslot.cpp \
    src/examples/examplewritehdf5.cpp \
    src/examples/examplewriteprojectstdout.cpp \
    src/examples/examplesaveallcombinations.cpp \
    src/examples/exampleloadmultipletimes.cpp \
    src/examples/exampletestmemoryleak.cpp \
    src/examples/exampletestfloodfill.cpp

HEADERS += \
    src/examples/examples.h \

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
    src/tracked/tracklet.h \
    src/exceptions/ctdependencyexception.h \
    src/graphics/merge.h \
    src/graphics/separate.h \
    src/version.h \
    src/io/modify.h \
    src/io/modifyhdf5.h \
    src/graphics/base.h \
    src/graphics/floodfill.h \
    src/provider/timetracker.h \
    src/tracked/trackeventdead.hpp \
    src/tracked/trackeventdivision.hpp \
    src/tracked/trackeventendofmovie.hpp \
    src/tracked/trackeventlost.hpp \
    src/tracked/trackeventmerge.hpp \
    src/tracked/trackeventunmerge.hpp

DISTFILES += \
    README.md
