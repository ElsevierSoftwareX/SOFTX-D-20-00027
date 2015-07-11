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

    void setMotherCell();
    void setDaughterCells();

    QColor getCellLineColor(std::shared_ptr<Object> o);
    QColor getCellBgColor(std::shared_ptr<Object> o, QPolygonF &outline, QPointF &mousePos);

    QImage defaultImage(QSize *size, const QSize &requestedSize);

    void drawPolygon(QPainter &painter, QPolygonF &poly, QColor col);
    void drawOutlines(QImage &image, int frame, double scaleFactor);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
};
}

#endif // IMAGEPROVIDER2_H
