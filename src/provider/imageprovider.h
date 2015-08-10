#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QColor>
#include <QImage>
#include <QObject>
#include <QString>
#include <QSize>
#include <QQuickImageProvider>

#include "src/import/importhdf5.h"

namespace CellTracker {
class ImageProvider : public QQuickImageProvider
{
public:
    explicit ImageProvider();
    ~ImageProvider();

    bool cellIsSelected(std::shared_ptr<Object> o);
    bool cellAutoTrackletIsSelected(std::shared_ptr<Object> o);
    bool cellIsHovered(std::shared_ptr<Object> o);
    bool cellIsInDaughters(std::shared_ptr<Object> daughter);
    bool cellIsInTracklet(std::shared_ptr<Object> o);

    QColor getCellLineColor(std::shared_ptr<Object> o);
    qreal getCellLineWidth(std::shared_ptr<Object> o);
    QColor getCellBgColor(std::shared_ptr<Object> o, QPolygonF &outline, QPointF &mousePos);
    Qt::BrushStyle getCellBrushStyle(std::shared_ptr<Object> o, QPolygonF &outline, QPointF &mousePos);

    QImage defaultImage(QSize *size, const QSize &requestedSize);

    void drawPolygon(QPainter &painter, QPolygonF &poly, QColor col, Qt::BrushStyle style);
    void drawOutlines(QImage &image, int frame, double scaleFactor);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

private:
    QImage cachedImage;
    QString cachedPath = "";
    int cachedFrame = -1;
};
}

#endif // IMAGEPROVIDER_H
