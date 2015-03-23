#include "trackeventlost.h"

namespace CellTracker {

template <typename T>
TrackEventLost<T>::TrackEventLost() : TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_LOST)
{
}

template <typename T>
std::shared_ptr<Tracklet>
TrackEventLost<T>::getPrev() const
{
    return prev;
}

template <typename T>
void
TrackEventLost<T>::setPrev(const std::shared_ptr<Tracklet> &value)
{
    prev = value;
}

/* Templates suck. See
 * https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor
 */
template class TrackEventLost<Tracklet>;

}
