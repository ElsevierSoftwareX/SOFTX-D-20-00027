#ifndef TRACKLET_H
#define TRACKLET_H

#include "trackelement.h"
#include "annotateable.h"

#include <iostream>
#include <memory>

#include <QHash>
#include <QPair>

#include "base_data/frame.h"
#include "base_data/object.h"
#include "trackevent.h"

namespace CellTracker { class Tracklet; }
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
    /*!
     * \brief The TRACKLET_TYPE enum
     *
     * This enum is used to specify the type of a Tracklet.
     */
    enum TRACKLET_TYPE {
        TRACKLET_REGULAR, /*!< Tracklet is of type TrackletRegular */
        TRACKLET_MERGED,  /*!< Tracklet is of type TrackletMerged */
        TRACKLET_SPLIT    /*!< Tracklet is of type TrackletSplit */
    } __attribute__ ((deprecated));

    Tracklet();
    Tracklet(TRACKLET_TYPE) __attribute__ ((deprecated));

    int getID() const;
    TRACKLET_TYPE getType() const __attribute__ ((deprecated));
    void setID(int value);
    void setType(const TRACKLET_TYPE &value) __attribute__ ((deprecated));

    QList<QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> getObjectsAt(int frameId) const;

    QHash<int,QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> getContained() const;
    void setContained(const QHash<int,QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> &);
    void addToContained(const std::shared_ptr<Frame>,const std::shared_ptr<Object>);
    void removeFromContained(int frameId, uint32_t objId);

    std::shared_ptr<TrackEvent<Tracklet>> getNext() const;
    void setNext(const std::shared_ptr<TrackEvent<Tracklet>> &value);

private:
    QHash<int, QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> contained;
    TRACKLET_TYPE type __attribute__ ((deprecated));

    std::shared_ptr<TrackEvent<Tracklet>> next;
    int id;
};

}


#endif // TRACKLET_H
