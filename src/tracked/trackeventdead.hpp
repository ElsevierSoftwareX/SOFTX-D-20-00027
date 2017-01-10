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
template <typename T>
class TrackEventDead : public TrackEvent<T>
{
    friend std::ostream& ::operator<< <>(std::ostream&, CellTracker::TrackEvent<T>&);
public:
    /*!
     * \brief constructs a new TrackEventDead
     */
    TrackEventDead() : TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_DEAD) {}

   /*!
    * \brief returns the previous Tracklet
    * \return the previous Tracklet
    */
    std::weak_ptr<T> getPrev() const {
        return prev;
    }
    /*!
     * \brief sets the previous Tracklet
     * \param value the Tracklet to set as previous
     */
    void setPrev(const std::weak_ptr<T> &value) {
        prev = value;
    }

private:
    std::weak_ptr<T> prev; /*!< The previous Tracklet whose Object is now dead */
};

}

#endif // TRACKEVENTDEAD_H
