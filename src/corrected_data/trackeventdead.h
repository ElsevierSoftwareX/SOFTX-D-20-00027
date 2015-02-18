#ifndef TRACKEVENTDEAD_H
#define TRACKEVENTDEAD_H

#include "trackevent.h"

#include <memory>

#include "tracklet.h"

namespace CellTracker {

/*!
 * \brief The TrackEventDead class
 *
 * This class should be used, when a tracked object is dead, ie. doesn't move
 * anymore and/or dissolves.
 *
 * Consequently there is no member next, but only prev.
 */
class TrackEventDead : public TrackEvent
{
public:
    TrackEventDead();

    std::shared_ptr<Tracklet> getPrev() const;
    void setPrev(const std::shared_ptr<Tracklet> &value);

private:
    std::shared_ptr<Tracklet> prev; /*!< The previous Tracklet whose Object is now dead */
};

}

#endif // TRACKEVENTDEAD_H
