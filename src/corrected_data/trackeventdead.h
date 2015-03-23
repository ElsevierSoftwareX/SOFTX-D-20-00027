#ifndef TRACKEVENTDEAD_H
#define TRACKEVENTDEAD_H

#include "trackevent.h"

#include <memory>

#include "tracklet.h"

namespace CellTracker { template <typename T> class TrackEvent; }
template <typename T> std::ostream& operator<< (std::ostream&, CellTracker::TrackEvent<T>&);

namespace CellTracker {

/*!
 * \brief The TrackEventDead class
 *
 * This class should be used, when a tracked object is dead, ie. doesn't move
 * anymore and/or dissolves.
 *
 * Consequently there is no member next, but only prev.
 */
template <typename T>
class TrackEventDead : public TrackEvent<T>
{
    friend std::ostream& ::operator<< (std::ostream&, TrackEvent<T>&);
public:
    TrackEventDead();

    std::shared_ptr<T> getPrev() const;
    void setPrev(const std::shared_ptr<T> &value);

private:
    std::shared_ptr<T> prev; /*!< The previous Tracklet whose Object is now dead */
};

}

#endif // TRACKEVENTDEAD_H
