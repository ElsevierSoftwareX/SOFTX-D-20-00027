#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlComponent>

#if 1
#include "examples/examples.h"
#endif
#include "src/provider/imageprovider.h"
#include "src/provider/dataprovider.h"
#include "provider/messagerelay.h"

#include <QFile>
#include <QTextStream>
#include <string>
#include <sstream>
#include <QDebug>
#include <QImageWriter>

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  ImageProvider *provider = new ImageProvider();
  DataProvider dataProvider;

  dataProvider.setProvider(provider);

  QQmlApplicationEngine engine;
  engine.addImageProvider("celltracking", provider);

  //QMetaType::registerType("Widget", 1, 0, "Widget");

  engine.rootContext()->setContextProperty("dataProvider", &dataProvider);
  engine.rootContext()->setContextProperty("MessageRelay", MessageRelay::getInstance());
  engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

  QObject *root = engine.rootObjects().first();
  QObject *area = root->findChild<QObject*>("mouseArea");
  provider->setMouseArea(area);

  return app.exec();
}
