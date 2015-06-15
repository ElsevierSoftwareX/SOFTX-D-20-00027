#ifndef IMPORTOBJECT_H
#define IMPORTOBJECT_H

#include <QObject>
#include <QString>
#include <QUrl>

#include "src/import/importhdf5.h"
#include "src/export/exporthdf5.h"
#include "src/provider/imageprovider.h"

class DataProvider : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE int getMaximumValue();
    Q_INVOKABLE int getCurrentObjectID();
    Q_INVOKABLE int getSelectedObjectID();

    Q_INVOKABLE int getCurrentTrackID();
    Q_INVOKABLE int getSelectedTrackID();
    Q_INVOKABLE int getTrackStart(int id);
    Q_INVOKABLE int getTrackEnd(int id);
    Q_INVOKABLE int getTrackLength(int id);

    Q_INVOKABLE int getCurrentAutoTrackID();
    Q_INVOKABLE int getSelectedAutoTrackID();
    Q_INVOKABLE int getAutoTrackStart(int id);
    Q_INVOKABLE int getAutoTrackEnd(int id);
    Q_INVOKABLE int getAutoTrackLength(int id);

    Q_INVOKABLE bool connectTracks();
    Q_INVOKABLE bool isCurrentInTracklet();
    Q_INVOKABLE bool isSelectedInTracklet();
    Q_INVOKABLE void setMotherCell();
    Q_INVOKABLE void setDaughterCells();
    Q_INVOKABLE void setStrategyStep(int step);
    Q_INVOKABLE void setProvider(ImageProvider *provider);
    Q_INVOKABLE void setStatus(QString status);
    Q_INVOKABLE void loadHDF5(QString fileName);
    Q_INVOKABLE void saveHDF5(QString fileName);

    Q_INVOKABLE QString getStatus();
    Q_INVOKABLE QList<int> getTrackletFrames(int id);
    Q_INVOKABLE QList<int> getAutoTrackletFrames(int id);
    Q_INVOKABLE QList<QPair<QString, QString>> getAnnotations();
    Q_INVOKABLE QImage requestImage(QString fileName, int imageNumber);

private:
    int maximumValue;
    CellTracker::ImportHDF5 importer;
    CellTracker::ExportHDF5 exporter;
    std::shared_ptr<CellTracker::Project> proj;
    ImageProvider *imageProvider;
};

#endif // IMPORTOBJECT_H
