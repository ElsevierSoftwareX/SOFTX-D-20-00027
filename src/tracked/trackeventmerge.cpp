#include "trackeventmerge.h"

namespace CellTracker {

template <typename T>
TrackEventMerge<T>::TrackEventMerge() :
    TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_MERGE),
    prev(std::shared_ptr<QList<std::shared_ptr<T>>>(new QList<std::shared_ptr<T>>())) {}

template <typename T>
std::shared_ptr<QList<std::shared_ptr<T>>>
TrackEventMerge<T>::getPrev() const
{
    return prev;
}

template <typename T>
void
TrackEventMerge<T>::setPrev(const std::shared_ptr<QList<std::shared_ptr<T> > > &value)
{
    prev = value;
}

template <typename T>
std::shared_ptr<T>
TrackEventMerge<T>::getNext() const
{
    return next;
}

template <typename T>
void
TrackEventMerge<T>::setNext(const std::shared_ptr<T> &value)
{
    next = value;
}

/* Templates suck. See
 * https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor
 */
template class TrackEventMerge<Tracklet>;

}
