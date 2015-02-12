#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QObject>
#include <QString>
#include <QSize>
#include <QImage>
#include <QQuickImageProvider>

class MyClass : public QObject
{
   Q_OBJECT
   //Q_PROPERTY(QString myProperty READ prop NOTIFY propChanged)

public:
   //MyClass(QObject * parent = 0) : QObject(parent) {}
   //QString prop() { return QString("Hello from MyClass"); }
   Q_INVOKABLE void clickedButton(QString fileUrl);
};

class ImageProvider : public QQuickImageProvider
{
public:
    explicit ImageProvider();
    ~ImageProvider();

    void setMouseArea(QObject *area);
    void readData();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

private:
    int imageNumber;

    QObject *mouseArea;
    QString localFile;
    QImage *oldImage;
    QImage newImage;

    QList<int> listOfColors;
    QList<QList<int>> listOfImageColors;

    QList<QPoint> listOfPoints;
    QList<QList<QPoint>> listOfPolygons;
    QList<QList<QList<QPoint>>> listOfImages;
};

#endif // IMAGEPROVIDER_H
