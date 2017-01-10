#ifndef TRACKLET_H
#define TRACKLET_H

#include "annotateable.h"

#include <iostream>
#include <memory>

#include <QHash>
#include <QPair>

#include "base/frame.h"
#include "base/object.h"
#include "trackevent.h"
#include "trackeventdivision.h"

namespace CellTracker { class Tracklet; template <typename T> class TrackEvent; }
std::ostream& operator<< (std::ostream&, CellTracker::Tracklet&);

namespace CellTracker {

/*!
 * \brief The Tracklet class
 *
 * A Tracklet is a list of Object%s at certain Frame%s. It saves the Tracklets the user has tracked.
 * Each Tracklet might have a TrackEvent as next and/or previous Event. If those shared pointers are null,
 * it is interpreted as being open.
 */
class Tracklet : public QObject, public Annotateable
{
    Q_OBJECT
    friend std::ostream& ::operator<< (std::ostream&, Tracklet&);

public:
    Tracklet();
    ~Tracklet();

    int getId() const;
    void setId(int value);

    QList<QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> getObjectsAt(int frameId) const;

    QHash<uint,QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> getContained() const;
    void setContained(const QHash<uint,QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> &value);
    void addToContained(const std::shared_ptr<Frame>&,const std::shared_ptr<Object>&);
    void addToContained(const QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>);
    void removeFromContained(int frameId, uint32_t objId);
    bool hasObjectAt(int objId, int frameId);

    QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> getEnd() const;
    QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> getStart() const;

    std::shared_ptr<TrackEvent<Tracklet>> getNext() const;
    std::shared_ptr<TrackEvent<Tracklet>> getPrev() const;
    void setNext(std::shared_ptr<TrackEvent<Tracklet>> value);
    void setPrev(std::shared_ptr<TrackEvent<Tracklet>> value);

    /* QML-Stuff */
    Q_PROPERTY(QString id        READ qmlId)
    Q_PROPERTY(QString start     READ qmlStart)
    Q_PROPERTY(QString end       READ qmlEnd)
    Q_PROPERTY(QString previous  READ qmlPrevious)
    Q_PROPERTY(QString next      READ qmlNext)
    Q_PROPERTY(QString status    READ qmlStatus)
    Q_PROPERTY(QString tanno     READ qmlTAnno)
    Q_PROPERTY(QString oanno     READ qmlOAnno)
    Q_INVOKABLE QString qmlId();
    Q_INVOKABLE QString qmlStart();
    Q_INVOKABLE QString qmlEnd();
    Q_INVOKABLE QString qmlPrevious();
    Q_INVOKABLE QString qmlNext();
    Q_INVOKABLE QString qmlStatus();
    Q_INVOKABLE QString qmlTAnno();
    Q_INVOKABLE QString qmlOAnno();

private:
    /*! \todo change to QHash<int, std::shared_ptr<Object>> if possible */
    QHash<uint, QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> contained;

    std::shared_ptr<TrackEvent<Tracklet>> next;
    std::shared_ptr<TrackEvent<Tracklet>> prev;
    int id;
};

}

#endif // TRACKLET_H
