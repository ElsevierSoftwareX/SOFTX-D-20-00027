/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2017, 2016, 2015 Sebastian Wagner
 *
 * TraCurate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TraCurate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with TraCurate.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QFuture>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QQmlEngine>
#include <QJSEngine>
#include <QAbstractListModel>

#include "io/importhdf5.h"
#include "io/importxml.h"
#include "io/exporthdf5.h"

namespace TraCurate {
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
    Q_INVOKABLE void runLoad(QString fileName);
    Q_INVOKABLE void loadHDF5(QString fileName);
    Q_INVOKABLE void loadXML(QString fileName);

    Q_INVOKABLE void runSaveHDF5(QString filename, Export::SaveOptions &so);
    Q_INVOKABLE void runSaveHDF5(QString fileName);
    Q_INVOKABLE void runSaveHDF5();
    Q_INVOKABLE void saveHDF5(QString filename, bool sAnnotations, bool sAutoTracklets, bool sEvents, bool sImages, bool sInfo, bool sObjects, bool sTracklets);
    Q_INVOKABLE void saveHDF5(QString fileName);
    Q_INVOKABLE void saveHDF5();
    Q_INVOKABLE bool sanityCheckOptions(QString filename, bool sAnnotations, bool sAutoTracklets, bool sEvents, bool sImages, bool sInfo, bool sObjects, bool sTracklets);

    Q_INVOKABLE void runImportFiji(Project::XMLProjectSpec xps);
    Q_INVOKABLE void importFiji(QJSValue data);

    Q_INVOKABLE QString localFileFromURL(QString path);

    Q_INVOKABLE QImage requestImage(QString fileName, int frameNumber, int sliceNumber, int channelNumber);

    static DataProvider *getInstance();
    static QObject *qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

    /* new functions */
    Q_INVOKABLE std::shared_ptr<Object> cellAtFrame(int frame, int slice, int channel, double x, double y);
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

    void waitForFutures();
    qreal getDevicePixelRatio() const;
    void setDevicePixelRatio(const qreal &value);

private:
    explicit DataProvider(QObject *parent = 0);
    static DataProvider *theInstance;

    std::shared_ptr<Import> importer;
    ExportHDF5 exporter;

    QList<QObject *> annotations;
    QList<QObject *> tracklets;

    QList<QFuture<void>> futures;

    double scaleFactor;
    qreal devicePixelRatio;
signals:
    void annotationsChanged(QList<QObject*> value);
    void trackletsChanged(QList<QObject*> value);
};

}

#endif // DATAPROVIDER_H
