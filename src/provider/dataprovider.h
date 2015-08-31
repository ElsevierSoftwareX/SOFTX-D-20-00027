#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QQmlEngine>
#include <QJSEngine>

#include "src/import/importhdf5.h"
#include "src/export/exporthdf5.h"

namespace CellTracker {

class AnnotationItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ getTitle NOTIFY titleChanged)
    Q_PROPERTY(QString description READ getDescription NOTIFY descriptionChanged)
public:
    AnnotationItem(QString title, QString description) : title(title), description(description) {}
    QString getTitle() const {return title;}
    QString getDescription() const {return description;}
private:
    QString title;
    QString description;
signals:
    void titleChanged(QString);
    void descriptionChanged(QString);
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
    QList<QObject*> getAnnotationsModel();
    Q_PROPERTY(QList<QObject*> annotationsModel READ getAnnotationsModel NOTIFY anntationsModelChanged)

private:
    explicit DataProvider(QObject *parent = 0);
    static DataProvider *theInstance;

    ImportHDF5 importer;
    ExportHDF5 exporter;

    QList<QObject*> annotations;

    double scaleFactor;
signals:
    void anntationsModelChanged(QList<QObject*>);
};

}

#endif // DATAPROVIDER_H
