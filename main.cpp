#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "imageprovider.h"

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  ImageProvider *provider = new ImageProvider();

  QQmlApplicationEngine engine;
  engine.addImageProvider("celltracking", provider);
  engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

  //QMetaType::registerType("Widget", 1, 0, "Widget");

  QObject *root = engine.rootObjects().first();
  QObject *slider = root->findChild<QObject*>("horizontalSlider");
  provider->setSlider(slider);

  return app.exec();
}
