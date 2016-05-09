#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QQmlEngine>
#include <QJSEngine>
#include <QAbstractListModel>

#include "io/importhdf5.h"
#include "io/exporthdf5.h"

namespace CellTracker {
/*!
 * \brief The DataProvider class
 *
 * This class is used to provide Data for the GUI and convert it to a
 * format understandable by QML. Some functions from this class should
 * in the long term be moved to GUIController and some to GUIState.
 */
class DataProvider : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE void runLoadHDF5(QString fileName);
    Q_INVOKABLE void loadHDF5(QString fileName);
    Q_INVOKABLE void runSaveHDF5(QString fileName);
    Q_INVOKABLE void runSaveHDF5();
    Q_INVOKABLE void saveHDF5(QString fileName);
    Q_INVOKABLE void saveHDF5();

    Q_INVOKABLE QString localFileFromURL(QString path);

    Q_INVOKABLE QImage requestImage(QString fileName, int imageNumber);

    static DataProvider *getInstance();
    static QObject *qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

    /* new functions */
    Q_INVOKABLE std::shared_ptr<Object> cellAtFrame(int frame, double x, double y);
    Q_INVOKABLE std::shared_ptr<Object> cellAt(double x, double y);
    Q_INVOKABLE int cellIDAt(double x, double y);

    double getScaleFactor() const;
    void setScaleFactor(double value);

    /* trackletModel for projectView */
    QList<QObject *> getTracklets();
    void setTracklets(const QList<QObject *> &value);

    /* annotationsModel for projectView */
    QList<QObject *> getAnnotations();
    void setAnnotations(const QList<QObject *> &value);
    Q_INVOKABLE int addAnnotation(int);
    Q_INVOKABLE void changeAnnotation(int, int, QString, QString);
    Q_INVOKABLE void deleteAnnotation(int);
    Q_INVOKABLE bool isAnnotatedWith(int);
    Q_INVOKABLE void toggleAnnotate(int);
    Q_INVOKABLE void annotateSelectedObject(int id);
    Q_INVOKABLE void annotateSelectedTracklet(int id);

    Q_PROPERTY(QList<QObject*> annotations READ getAnnotations WRITE setAnnotations NOTIFY annotationsChanged)
    Q_PROPERTY(QList<QObject*> tracklets READ getTracklets WRITE setTracklets NOTIFY trackletsChanged)
private:
    explicit DataProvider(QObject *parent = 0);
    static DataProvider *theInstance;

    ImportHDF5 importer;
    ExportHDF5 exporter;

    QList<QObject *> annotations;
    QList<QObject *> tracklets;

    double scaleFactor;
signals:
    void annotationsChanged(QList<QObject*> value);
    void trackletsChanged(QList<QObject*> value);
};

}

#endif // DATAPROVIDER_H
