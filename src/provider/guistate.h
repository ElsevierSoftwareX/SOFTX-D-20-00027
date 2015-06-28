#ifndef GUISTATE_H
#define GUISTATE_H

#include "project.h"
#include "base_data/object.h"

#include <memory>

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>

namespace CellTracker {

class GUIState : public QObject
{
    Q_OBJECT

public:
    static GUIState *getInstance();
    static QObject *qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

    int getObjectID() const;
    void setObjectID(int value);

    int getTrackID() const;
    void setTrackID(int value);

    int getImageNumber() const;
    void setImageNumber(int value);

    int getCurrentImage() const;
    void setCurrentImage(int value);

    int getStrategyStep() const;
    void setStrategyStep(int value);

    int getSelectedTrackID() const;
    void setSelectedTrackID(int value);

    bool getIsInTracklet() const;
    void setIsInTracklet(bool value);

    uint32_t getSelectedCellID() const;
    void setSelectedCellID(const uint32_t &value);

    QObject *getMouseArea() const;
    void setMouseArea(QObject *value);

    QImage getNewImage() const;
    void setNewImage(const QImage &value);

    QString getStatus() const;
    void setStatus(const QString &value);

    QString getPath() const;
    void setPath(const QString &value);

    std::shared_ptr<CellTracker::Project> getProj() const;
    void setProj(const std::shared_ptr<CellTracker::Project> &value);

    std::shared_ptr<CellTracker::Object> getLastObject() const;
    void setLastObject(const std::shared_ptr<CellTracker::Object> &value);

    std::shared_ptr<CellTracker::Object> getCurrentCell() const;
    void setCurrentCell(const std::shared_ptr<CellTracker::Object> &value);

    std::shared_ptr<CellTracker::Object> getSelectedCell() const;
    void setSelectedCell(const std::shared_ptr<CellTracker::Object> &value);

    std::shared_ptr<CellTracker::Object> getMotherCell() const;
    void setMotherCell(const std::shared_ptr<CellTracker::Object> &value);

    QList<std::shared_ptr<CellTracker::Object> > getDaughterCells() const;
    void setDaughterCells(const QList<std::shared_ptr<CellTracker::Object> > &value);

    QList<std::shared_ptr<CellTracker::Object> > getListOfPolygons() const;
    void setListOfPolygons(const QList<std::shared_ptr<CellTracker::Object> > &value);

private:
    explicit GUIState(QObject *parent = 0);
    static GUIState *theInstance;

    int objectID;
    int trackID;
    int imageNumber;
    int currentImage;
    int strategyStep;
    int selectedTrackID;
    bool isInTracklet;

    uint32_t selectedCellID;
    QObject *mouseArea;
    QImage newImage;
    QString status;
    QString path;

    std::shared_ptr<CellTracker::Project> proj;
    std::shared_ptr<CellTracker::Object> lastObject;
    std::shared_ptr<CellTracker::Object> currentCell;
    std::shared_ptr<CellTracker::Object> selectedCell;
    std::shared_ptr<CellTracker::Object> motherCell;
    QList<std::shared_ptr<CellTracker::Object>> daughterCells;
    QList<std::shared_ptr<CellTracker::Object>> listOfPolygons;

};
}

#endif // GUISTATE_H
