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

//    int getObjectID() __attribute__((deprecated));
//    int getTrackID() __attribute__((deprecated));
//    int getSelectedCellID() __attribute__((deprecated));
//    int getImageNumber() __attribute__((deprecated));
//    int getCurrentImage() __attribute__((deprecated));

//    QString getStatus() __attribute__((deprecated));

    void setMotherCell();
    void setDaughterCells();
    void setMouseArea(QObject *area);
//    void setStrategyStep(int step) __attribute__((deprecated));
//    void setProject(std::shared_ptr<CellTracker::Project> proj) __attribute__((deprecated));
    QColor getCellColor(std::shared_ptr<CellTracker::Object> o, QPolygonF &outline, QPointF &mousePos);
    void drawPolygon(QPainter &painter, QPolygonF &poly, QColor col);
    void drawOutlines(QImage &image, int frame, double scaleFactor);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

//    std::shared_ptr<CellTracker::Object> getCurrentCell() __attribute__((deprecated));
//    std::shared_ptr<CellTracker::Object> getSelectedCell() __attribute__((deprecated));

private:
//    int objectID __attribute__((deprecated));
//    int trackID __attribute__((deprecated));
//    int imageNumber __attribute__((deprecated));
//    int currentImage __attribute__((deprecated));
//    int strategyStep __attribute__((deprecated));
//    int selectedTrackID __attribute__((deprecated));
//    bool isInTracklet __attribute__((deprecated));

//    uint32_t selectedCellID __attribute__((deprecated));
    QObject *mouseArea;
//    QImage newImage __attribute__((deprecated));
//    QString status __attribute__((deprecated));
//    QString path __attribute__((deprecated));

//    std::shared_ptr<CellTracker::Project> proj __attribute__((deprecated));
//    std::shared_ptr<CellTracker::Object> lastObject __attribute__((deprecated));
//    std::shared_ptr<CellTracker::Object> currentCell __attribute__((deprecated));
//    std::shared_ptr<CellTracker::Object> selectedCell __attribute__((deprecated));
//    std::shared_ptr<CellTracker::Object> motherCell __attribute__((deprecated));
//    QList<std::shared_ptr<CellTracker::Object>> daughterCells __attribute__((deprecated));
//    QList<std::shared_ptr<CellTracker::Object>> listOfPolygons __attribute__((deprecated));
};

#endif // IMAGEPROVIDER2_H
