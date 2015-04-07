#ifndef IMPORTOBJECT_H
#define IMPORTOBJECT_H

#include <QObject>
#include <QString>
#include <QUrl>

#include "src/import/importhdf5.h"
#include "src/provider/imageprovider.h"

class ImportObject : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE int getMaximumValue();
    Q_INVOKABLE int getObjectID();
    Q_INVOKABLE int getTrackID();
    Q_INVOKABLE int getTrackID(int id);
    Q_INVOKABLE int getTrackStart(int id);
    Q_INVOKABLE int getTrackEnd(int id);
    Q_INVOKABLE int getTrackLength(int id);
    Q_INVOKABLE bool connectTracks();
    Q_INVOKABLE bool isAutoTracklet();
    Q_INVOKABLE void setLastObjectID(int id);
    Q_INVOKABLE void setProvider(ImageProvider *provider);
    Q_INVOKABLE void loadHDF5(QString fileName);
    Q_INVOKABLE QString getStatus();
    Q_INVOKABLE QList<int> getTrackletFrames(int id);
    Q_INVOKABLE QList<QPair<QString, QString>> getAnnotations();
    Q_INVOKABLE QImage requestImage(QString fileName, int imageNumber);

private:
    int maximumValue;
    CellTracker::ImportHDF5 MyImport;
    std::shared_ptr<CellTracker::Project> proj;
    ImageProvider *imageProvider;
};

#endif // IMPORTOBJECT_H
