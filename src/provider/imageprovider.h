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
    void setStrategyStep(int step);
    void setProject(std::shared_ptr<CellTracker::Project> proj);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

    std::shared_ptr<CellTracker::Object> getCurrentCell();
    std::shared_ptr<CellTracker::Object> getSelectedCell();

private:
    int objectID;
    int trackID;
    int imageNumber;
    int currentImage;
    int lastObjectID;
    int strategyStep;
    bool isInTracklet;

    uint32_t selectedCellID;
    QObject *mouseArea;
    QImage newImage;
    QString status;
    QString path;

    std::shared_ptr<CellTracker::Project> proj;
    std::shared_ptr<CellTracker::Object> lastObject;
    std::shared_ptr<CellTracker::Object> currentCell;
    std::shared_ptr<CellTracker::Object> selectedCell;
    QList<std::shared_ptr<CellTracker::Object>> listOfPolygons;
};

#endif // IMAGEPROVIDER_H
