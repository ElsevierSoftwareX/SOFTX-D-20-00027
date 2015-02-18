#ifndef TRACKLET_H
#define TRACKLET_H

#include "trackelement.h"

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
class Tracklet : public TrackElement
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
    };

    Tracklet();
    Tracklet(TRACKLET_TYPE);

    int getID() const;
    TRACKLET_TYPE getType() const;
    void setID(int value);
    void setType(const TRACKLET_TYPE &value);

    QHash<int,QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> getContained() const;
    void setContained(const QHash<int,QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> &);
    void addToContained(const std::shared_ptr<Frame>,const std::shared_ptr<Object>);

    std::shared_ptr<TrackEvent> getNext() const;
    void setNext(const std::shared_ptr<TrackEvent> &value);

private:
    QHash<int, QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> contained;
    TRACKLET_TYPE type;

    std::shared_ptr<TrackEvent> next;
    int id;
};

}


#endif // TRACKLET_H
