#ifndef IMAGEPROVIDER2_H
#define IMAGEPROVIDER2_H

#include <QColor>
#include <QImage>
#include <QObject>
#include <QString>
#include <QSize>
#include <QQuickImageProvider>

#include "src/import/importhdf5.h"

class ImageProvider2 : public QQuickImageProvider
{
public:
    explicit ImageProvider2();
    ~ImageProvider2();

    int getObjectID();
    int getTrackID();
    int getSelectedCellID();
    int getImageNumber();
    int getCurrentImage();

    QString getStatus();

    void setMotherCell();
    void setDaughterCells();
    void setMouseArea(QObject *area);
    void setStrategyStep(int step);
    void setProject(std::shared_ptr<CellTracker::Project> proj);
    QColor getCellColor(std::shared_ptr<CellTracker::Object> o);
    void drawPolygon(QPainter &painter, QPolygonF &poly, QColor col);
    void drawOutlines(QImage &image, int frame, double scaleFactor);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

    std::shared_ptr<CellTracker::Object> getCurrentCell();
    std::shared_ptr<CellTracker::Object> getSelectedCell();

private:
    int objectID;
    int trackID;
    int imageNumber;
    int currentImage;
    int strategyStep;
    int selectedTrackID;
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
    std::shared_ptr<CellTracker::Object> motherCell;
    QList<std::shared_ptr<CellTracker::Object>> daughterCells;
    QList<std::shared_ptr<CellTracker::Object>> listOfPolygons;
};

#endif // IMAGEPROVIDER2_H
