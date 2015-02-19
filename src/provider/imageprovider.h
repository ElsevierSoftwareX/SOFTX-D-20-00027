#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QObject>
#include <QString>
#include <QSize>
#include <QImage>
#include <QQuickImageProvider>

#include "src/import/importhdf5.h"

class ImageProvider : public QQuickImageProvider
{
public:
    explicit ImageProvider();
    ~ImageProvider();

    void setMouseArea(QObject *area);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

    QList<int> listOfColors;
    QList<QList<int>> listOfImageColors;

    QList<QPoint> listOfPoints;
    QList<QList<QPoint>> listOfPolygons;
    QList<QList<QList<QPoint>>> listOfImages;

private:
    int imageNumber;
    QString path;

    QObject *mouseArea;
    QString localFile;
    QImage *oldImage;
    QImage newImage;
};

#endif // IMAGEPROVIDER_H
