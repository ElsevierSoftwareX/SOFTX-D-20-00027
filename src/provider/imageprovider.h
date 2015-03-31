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
    int getSelectedCellID();
    int getImageNumber();
    int getCurrentImage();

    QString getStatus();

    void setMouseArea(QObject *area);
    void setLastObjectID(int id);
    void setProject(std::shared_ptr<CellTracker::Project> proj);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

    std::shared_ptr<CellTracker::Object> getCurrentCell();

private:
    int objectID;
    int trackID;
    int imageNumber;
    int currentImage;
    int lastObjectID;

    uint32_t selectedCellID;
    QObject *mouseArea;
    QImage newImage;
    QString status;
    QString path;

    std::shared_ptr<CellTracker::Project> proj;
    std::shared_ptr<CellTracker::Object> currentCell;
    QList<std::shared_ptr<CellTracker::Object>> listOfPolygons;
};

#endif // IMAGEPROVIDER_H
