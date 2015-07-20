#ifndef IMAGEPROVIDER2_H
#define IMAGEPROVIDER2_H

#include <QColor>
#include <QImage>
#include <QObject>
#include <QString>
#include <QSize>
#include <QQuickImageProvider>

#include "src/import/importhdf5.h"

namespace CellTracker {
class ImageProvider2 : public QQuickImageProvider
{
public:
    explicit ImageProvider2();
    ~ImageProvider2();

    bool cellIsSelected(std::shared_ptr<Object> o);
    bool cellAutoTrackletIsSelected(std::shared_ptr<Object> o);
    bool cellIsHovered(QPolygonF &outline, QPointF &mousePos);
    bool cellIsInDaughters(std::shared_ptr<Object> daughter);
    bool cellIsInTracklet(std::shared_ptr<Object> o);

    QColor getCellLineColor(std::shared_ptr<Object> o);
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

#endif // IMAGEPROVIDER2_H
