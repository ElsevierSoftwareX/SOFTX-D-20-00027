#include "trackeventdead.h"

namespace CellTracker {

/*!
 * \brief constructs a new TrackEventDead
 */
template <typename T>
TrackEventDead<T>::TrackEventDead() :
    TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_DEAD) {}

/*!
 * \brief returns the previous Tracklet
 * \return the previous Tracklet
 */
template <typename T>
std::shared_ptr<T>
TrackEventDead<T>::getPrev() const
{
    return prev;
}

/*!
 * \brief sets the previous Tracklet
 * \param value the Tracklet to set as previous
 */
template <typename T>
void
TrackEventDead<T>::setPrev(const std::shared_ptr<T> &value)
{
    prev = value;
}

/* Templates suck. See
 * https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor
 */
template class TrackEventDead<Tracklet>;

}
