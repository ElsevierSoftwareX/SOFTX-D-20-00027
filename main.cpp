#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlComponent>

#if 0
#include "examples/examples.h"
#endif
#include "provider/ctsettings.h"
#include "provider/dataprovider.h"
#include "provider/guicontroller.h"
#include "provider/guistate.h"
#include "provider/imageprovider.h"
#include "provider/messagerelay.h"
#include "provider/timetracker.h"

#include <QFile>
#include <QTextStream>
#include <string>
#include <sstream>
#include <QDebug>
#include <QtQml>
#include <QImageWriter>


#include "graphics/merge.h"
int main(int argc, char *argv[])
{
    using namespace CellTracker;
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    CTSettings::getInstance();
    GUIState::getInstance();
    DataProvider::getInstance();

    ImageProvider *provider = new ImageProvider();

    qmlRegisterSingletonType<CTSettings>   ("imb.celltracker", 1,0, "CTSettings",    CTSettings::qmlInstanceProvider);
    qmlRegisterSingletonType<GUIController>("imb.celltracker", 1,0, "GUIController", GUIController::qmlInstanceProvider);
    qmlRegisterSingletonType<GUIState>     ("imb.celltracker", 1,0, "GUIState",      GUIState::qmlInstanceProvider);
    qmlRegisterSingletonType<DataProvider> ("imb.celltracker", 1,0, "DataProvider",  DataProvider::qmlInstanceProvider);
    qmlRegisterSingletonType<MessageRelay> ("imb.celltracker", 1,0, "MessageRelay",  MessageRelay::qmlInstanceProvider);
    qmlRegisterType<Annotation> ("imb.celltracker", 1,0, "Annotation");
    qmlRegisterType<CTOption>   ("imb.celltracker", 1,0, "CTOption");
    qmlRegisterType<Tracklet>   ("imb.celltracker", 1,0, "Tracklet");

    engine.addImageProvider("celltracking", provider);
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    DataProvider::getInstance()->setDevicePixelRatio(app.devicePixelRatio());

    int ret;
    {
        TimeTracker tt;
        ret = app.exec();
    }

    GUIController::getInstance()->abortStrategy();

    /* Wait for threads started by QtConcurrent to finish */
    GUIController::getInstance()->waitForFutures();
    DataProvider::getInstance()->waitForFutures();

    return ret;
}
