#ifndef MYCLASS_H
#define MYCLASS_H

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
    Q_INVOKABLE void readData();
    Q_INVOKABLE void setProvider(ImageProvider *provider);
    Q_INVOKABLE void loadHDF5(QString fileName);
    Q_INVOKABLE QImage requestImage(QString fileName, int imageNumber);

private:
    int maximumValue;
    CellTracker::ImportHDF5 MyImport;
    std::shared_ptr<CellTracker::Project> proj;
    ImageProvider *imageProvider;
};

#endif // MYCLASS_H