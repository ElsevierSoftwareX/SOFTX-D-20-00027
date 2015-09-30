#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QQmlEngine>
#include <QJSEngine>
#include <QAbstractListModel>

#include "src/io/importhdf5.h"
#include "src/io/exporthdf5.h"

namespace CellTracker {

class DataProvider : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE void runLoadHDF5(QString fileName);
    Q_INVOKABLE void loadHDF5(QString fileName);
    Q_INVOKABLE void saveHDF5(QString fileName);

    Q_INVOKABLE QImage requestImage(QString fileName, int imageNumber);

    static DataProvider *getInstance();
    static QObject *qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

    /* new functions */
    Q_INVOKABLE std::shared_ptr<Object> cellAtFrame(int frame, double x, double y);
    Q_INVOKABLE std::shared_ptr<Object> cellAt(double x, double y);
    Q_INVOKABLE int cellIDAt(double x, double y);

    double getScaleFactor() const;
    void setScaleFactor(double value);

    /* annotationsModel for projectView */
    QList<QObject*> getAnnotations();
    Q_PROPERTY(QList<QObject*> annotations READ getAnnotations NOTIFY annotationsChanged)

private:
    explicit DataProvider(QObject *parent = 0);
    static DataProvider *theInstance;

    ImportHDF5 importer;
    ExportHDF5 exporter;

    QList<QObject*> annotations;

    double scaleFactor;
signals:
    void annotationsChanged(QList<QObject*> value);
};

}

#endif // DATAPROVIDER_H
