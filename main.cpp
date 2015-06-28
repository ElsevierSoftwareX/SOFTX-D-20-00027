#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlComponent>

#if 0
#include "examples/examples.h"
#endif
#include "src/provider/ctsettings.h"
#include "src/provider/imageprovider.h"
#include "src/provider/imageprovider2.h"
#include "src/provider/dataprovider.h"
#include "provider/guistate.h"
#include "provider/messagerelay.h"

#include <QFile>
#include <QTextStream>
#include <string>
#include <sstream>
#include <QDebug>
#include <QtQml>
#include <QImageWriter>

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  CellTracker::CTSettings::getInstance();

//  ImageProvider *provider = new ImageProvider();
  ImageProvider2 *provider2 = new ImageProvider2();
  DataProvider dataProvider;

  dataProvider.setProvider(provider2);
//  qmlRegisterType<CellTracker::CTSettings>("imb.celltracker.settings", 1, 0, "settings");
  qmlRegisterSingletonType<CellTracker::CTSettings>("imb.celltracker.settings", 1,0,"CTSettings", CellTracker::CTSettings::qmlInstanceProvider);
  qmlRegisterSingletonType<CellTracker::GUIState>("imb.celltracker.guistate", 1,0,"GUIState", CellTracker::GUIState::qmlInstanceProvider);

  QQmlApplicationEngine engine;
//  engine.addImageProvider("celltracking", provider);
  engine.addImageProvider("celltracking2", provider2);

  //QMetaType::registerType("Widget", 1, 0, "Widget");

  engine.rootContext()->setContextProperty("dataProvider", &dataProvider);
  engine.rootContext()->setContextProperty("messageRelay", MessageRelay::getInstance());
//  engine.rootContext()->setContextProperty("settings", CellTracker::CTSettings::getInstance());
  engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

  QObject *root = engine.rootObjects().first();
  QObject *area = root->findChild<QObject*>("mouseArea");
//  provider->setMouseArea(area);
  provider2->setMouseArea(area);

  int ret = app.exec();

  return ret;
}
