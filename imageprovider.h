#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QObject>
#include <QString>
#include <QSize>
#include <QImage>
#include <QQuickImageProvider>

class ImageProvider : public QQuickImageProvider
{
public:
    explicit ImageProvider();
    ~ImageProvider();

    void setSlider(QObject *slider);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

private:
    QImage *lastImage;
    QObject *horizontalSlider;
};

#endif // IMAGEPROVIDER_H
