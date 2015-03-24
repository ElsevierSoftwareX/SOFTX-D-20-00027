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

    int getObjectID();
    int getTrackID();

    QString getStatus();

    void setMouseArea(QObject *area);
    void setLastObjectID(int id);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

    QList<int> listOfColors;
    QList<QList<int>> listOfImageColors;

    QList<std::shared_ptr<CellTracker::Object>> listOfPolygons;
    QList<QList<std::shared_ptr<CellTracker::Object>>> listOfImages;

private:
    int objectID;
    int trackID;
    int imageNumber;
    int currentImage;
    int lastObjectID;

    uint32_t selectedCell;
    QObject *mouseArea;
    QImage newImage;
    QString status;
    QString path;
};

#endif // IMAGEPROVIDER_H
