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

    Q_INVOKABLE int getObjectID() const;
    Q_INVOKABLE void setObjectID(int value);

    Q_INVOKABLE int getTrackID() const;
    Q_INVOKABLE void setTrackID(int value);

    Q_INVOKABLE int getStrategyStep() const;
    Q_INVOKABLE void setStrategyStep(int value);

    Q_INVOKABLE int getSelectedTrackID() const;
    Q_INVOKABLE void setSelectedTrackID(int value);

    Q_INVOKABLE bool getIsInTracklet() const;
    Q_INVOKABLE void setIsInTracklet(bool value);

    Q_INVOKABLE uint32_t getSelectedCellID() const;
    Q_INVOKABLE void setSelectedCellID(const uint32_t &value);

    QObject *getMouseArea() const;
    void setMouseArea(QObject *value);

    QImage getNewImage() const;
    void setNewImage(const QImage &value);

    Q_INVOKABLE QString getStatus() const;
    Q_INVOKABLE void setStatus(const QString &value);

    Q_INVOKABLE QString getPath() const;
    Q_INVOKABLE void setPath(const QString &value);

    std::shared_ptr<CellTracker::Project> getProj() const __attribute__((deprecated));
    void setProj(const std::shared_ptr<CellTracker::Project> &value) __attribute__((deprecated));

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

    Q_INVOKABLE int getCurrentFrame() const;
    Q_INVOKABLE void setCurrentFrame(int value);

    Q_INVOKABLE int getMaximumValue() const;
    Q_INVOKABLE void setMaximumValue(int value);

    Q_INVOKABLE float getLastX() const;
    Q_INVOKABLE void setLastX(float value);

    Q_INVOKABLE float getLastY() const;
    Q_INVOKABLE void setLastY(float value);

    Q_INVOKABLE float getSliderValue() const;
    Q_INVOKABLE void setSliderValue(float value);

    Q_INVOKABLE QString getStrategy() const;
    Q_INVOKABLE void setStrategy(const QString &value);

    Q_INVOKABLE QString getJumpStrategy() const;
    Q_INVOKABLE void setJumpStrategy(const QString &value);

    Q_INVOKABLE QString getMouseAction() const;
    Q_INVOKABLE void setMouseAction(const QString &value);

    Q_INVOKABLE bool getMouseAreaActive() const;
    Q_INVOKABLE void setMouseAreaActive(bool value);

private:
    explicit GUIState(QObject *parent = 0);
    static GUIState *theInstance;

    int objectID;
    int trackID;
    int currentFrame;
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

    /* from mouseArea */
    int maximumValue = 1;
    float lastX = 0;
    float lastY = 0;
    float sliderValue = 1;
//    QString status;
    QString strategy;
    QString jumpStrategy;
    QString mouseAction;
//    QString path;
    bool mouseAreaActive = true;
};
}

#endif // GUISTATE_H
