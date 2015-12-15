#include "base/autotracklet.h"
#include "trackeventdivision.h"

namespace CellTracker {

template <typename T>
TrackEventDivision<T>::TrackEventDivision() :
    TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_DIVISION),
    next(std::shared_ptr<QList<std::shared_ptr<T>>>(new QList<std::shared_ptr<T>>())) {}

template <typename T>
std::shared_ptr<T>
TrackEventDivision<T>::getPrev() const
{
    return prev;
}

template <typename T>
void
TrackEventDivision<T>::setPrev(const std::shared_ptr<T> &value)
{
    prev = value;
}

template <typename T>
std::shared_ptr<QList<std::shared_ptr<T>>>
TrackEventDivision<T>::getNext() const
{
    return next;
}

template <typename T>
void
TrackEventDivision<T>::setNext(const std::shared_ptr<QList<std::shared_ptr<T>>> &value)
{
    next = value;
}

/* Templates suck. See
 * https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor
 */
template class TrackEventDivision<AutoTracklet>;
template class TrackEventDivision<Tracklet>;

}
