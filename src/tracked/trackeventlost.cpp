#include "trackeventlost.h"

namespace CellTracker {

/*!
 * \brief constructs a new TrackEventLost
 */
template <typename T>
TrackEventLost<T>::TrackEventLost() :
    TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_LOST) {}

/*!
 * \brief returns the previous Tracklet
 */
template <typename T>
std::weak_ptr<Tracklet>
TrackEventLost<T>::getPrev() const
{
    return prev;
}

/*!
 * \brief sets the previous Tracklet
 * \param value the previous Tracklet
 */
template <typename T>
void
TrackEventLost<T>::setPrev(const std::weak_ptr<Tracklet> &value)
{
    prev = value;
}

/* Templates suck. See
 * https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor
 */
template class TrackEventLost<Tracklet>;

}
