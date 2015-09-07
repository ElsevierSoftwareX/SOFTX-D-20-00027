#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QQmlEngine>
#include <QJSEngine>
#include <QAbstractListModel>

#include "src/import/importhdf5.h"
#include "src/export/exporthdf5.h"

namespace CellTracker {

class AnnotationsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    AnnotationsModel(QObject *parent = 0)
        : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const {
        Q_UNUSED(parent) return annotations.length();
    }
    QVariant data(const QModelIndex &index, int role) const {
        return (role == Qt::DisplayRole && index.row() <= annotations.size())? annotations.at(index.row()) : QVariant();
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const {
        if (orientation == Qt::Horizontal)
            return QString("Column %1").arg(section);
        else
            return QString("Row %1").arg(section);
    }

private:
    QList<QVariant> annotations;
};

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
