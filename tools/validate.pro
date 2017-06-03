TARGET = validate

QT += qml quick xml
QMAKE_INCDIR += ../src/

QMAKE_CXXFLAGS_DEBUG += -O0 -g -std=c++11 -Wall -Wextra -pedantic -Wdeprecated -Wmissing-noreturn -Wunreachable-code -Wswitch-enum
QMAKE_CXXFLAGS_RELEASE += -O0 -g -std=c++11 -Wall -Wextra -pedantic

LIBS += -lhdf5 -lhdf5_cpp

macx
{
    CONFIG += c++11
    INCLUDEPATH += /opt/local/include
    LIBS += -L/opt/local/lib
}

SOURCES += validate.cpp \
    ../src/project.cpp \
    ../src/exceptions/ctexception.cpp \
    ../src/exceptions/ctimportexception.cpp \
    ../src/exceptions/ctformatexception.cpp \
    ../src/exceptions/ctdataexception.cpp \
    ../src/exceptions/ctdependencyexception.cpp \
    ../src/exceptions/ctmissingelementexception.cpp \
    ../src/provider/messagerelay.cpp \
    ../src/exceptions/ctexportexception.cpp \
    ../src/provider/dataprovider.cpp \
    ../src/provider/ctsettings.cpp \
    ../src/provider/guistate.cpp \
    ../src/provider/guicontroller.cpp \
    ../src/exceptions/ctunimplementedexception.cpp \
    ../src/provider/imageprovider.cpp \
    ../src/io/export.cpp \
    ../src/io/exporthdf5.cpp \
    ../src/io/import.cpp \
    ../src/io/importhdf5.cpp \
    ../src/io/importxml.cpp \
    ../src/io/hdf5_aux.cpp \
    ../src/base/autotracklet.cpp \
    ../src/base/channel.cpp \
    ../src/base/frame.cpp \
    ../src/base/info.cpp \
    ../src/base/movie.cpp \
    ../src/base/object.cpp \
    ../src/base/slice.cpp \
    ../src/tracked/annotateable.cpp \
    ../src/tracked/annotation.cpp \
    ../src/tracked/genealogy.cpp \
    ../src/tracked/trackevent.cpp \
    ../src/tracked/tracklet.cpp \
    ../src/provider/idprovider.cpp \
    ../src/graphics/separate.cpp \
    ../src/graphics/merge.cpp \
    ../src/io/modifyhdf5.cpp \
    ../src/graphics/base.cpp \
    ../src/graphics/floodfill.cpp

#RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(../deployment.pri)

HEADERS += \
    ../src/project.h \
    ../src/exceptions/ctexception.h \
    ../src/exceptions/ctimportexception.h \
    ../src/exceptions/ctformatexception.h \
    ../src/exceptions/ctdataexception.h \
    ../src/exceptions/ctdependencyexception.h \
    ../src/exceptions/ctmissingelementexception.h \
    ../src/provider/messagerelay.h \
    ../src/exceptions/ctexportexception.h \
    ../src/provider/dataprovider.h \
    ../src/provider/ctsettings.h \
    ../src/provider/guistate.h \
    ../src/provider/guicontroller.h \
    ../src/exceptions/ctunimplementedexception.h \
    ../src/provider/imageprovider.h \
    ../src/io/export.h \
    ../src/io/exporthdf5.h \
    ../src/io/import.h \
    ../src/io/importhdf5.h \
    ../src/io/importxml.h \
    ../src/io/hdf5_aux.h \
    ../src/base/autotracklet.h \
    ../src/base/channel.h \
    ../src/base/frame.h \
    ../src/base/info.h \
    ../src/base/movie.h \
    ../src/base/object.h \
    ../src/base/slice.h \
    ../src/tracked/annotateable.h \
    ../src/tracked/annotation.h \
    ../src/tracked/genealogy.h \
    ../src/provider/idprovider.h \
    ../src/tracked/trackevent.h \
    ../src/tracked/trackeventdead.hpp \
    ../src/tracked/trackeventdivision.hpp \
    ../src/tracked/trackeventendofmovie.hpp \
    ../src/tracked/trackeventlost.hpp \
    ../src/tracked/trackeventmerge.hpp \
    ../src/tracked/trackeventunmerge.hpp \
    ../src/tracked/tracklet.h \
    ../src/graphics/separate.h \
    ../src/graphics/merge.h \
    ../src/io/modifyhdf5.h \
    ../src/graphics/base.h \
    ../src/graphics/floodfill.h
