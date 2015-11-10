#ifndef TRACKLET_H
#define TRACKLET_H

#include "trackelement.h"
#include "annotateable.h"

#include <iostream>
#include <memory>

#include <QHash>
#include <QPair>

#include "base/frame.h"
#include "base/object.h"
#include "trackevent.h"

namespace CellTracker { class Tracklet; template <typename T> class TrackEvent; }
std::ostream& operator<< (std::ostream&, CellTracker::Tracklet&);

namespace CellTracker {

/*!
 * \brief The Tracklet class
 *
 * Tracklet is the superclass to the TrackletRegular, TrackletMerged and
 * TrackletSplit classes. The concrete type of the class is specified in type.
 */
class Tracklet : public TrackElement, public Annotateable
{
    friend std::ostream& ::operator<< (std::ostream&, Tracklet&);

public:
    Tracklet();
    ~Tracklet();

    int getId() const;
    void setId(int value);

    QList<QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> getObjectsAt(int frameId) const;

    QHash<int,QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> getContained() const;
    void setContained(const QHash<int,QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> &);
    void addToContained(const std::shared_ptr<Frame>&,const std::shared_ptr<Object>&);
    void addToContained(const QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>);
    void removeFromContained(int frameId, uint32_t objId);
    bool hasObjectAt(int objId, int frameId);

    QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> getEnd() const;
    QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> getStart() const;

    std::shared_ptr<TrackEvent<Tracklet>> getNext() const;
    std::shared_ptr<TrackEvent<Tracklet>> getPrev() const;
    void setNext(const std::shared_ptr<TrackEvent<Tracklet>> &value);
    void setPrev(const std::shared_ptr<TrackEvent<Tracklet>> &value);

private:
    QHash<int, QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> contained;

    std::shared_ptr<TrackEvent<Tracklet>> next;
    std::shared_ptr<TrackEvent<Tracklet>> prev;
    int id;
};

}

#endif // TRACKLET_H
