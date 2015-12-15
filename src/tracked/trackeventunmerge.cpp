#include "trackeventunmerge.h"

namespace CellTracker {

template <typename T>
TrackEventUnmerge<T>::TrackEventUnmerge() :
    TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_UNMERGE),
    next(std::shared_ptr<QList<std::shared_ptr<T>>>(new QList<std::shared_ptr<T>>())) {}

template <typename T>
std::shared_ptr<T>
TrackEventUnmerge<T>::getPrev() const
{
    return prev;
}

template <typename T>
void
TrackEventUnmerge<T>::setPrev(const std::shared_ptr<T> &value)
{
    prev = value;
}

template <typename T>
std::shared_ptr<QList<std::shared_ptr<T>>>
TrackEventUnmerge<T>::getNext() const
{
    return next;
}

template <typename T>
void
TrackEventUnmerge<T>::setNext(const std::shared_ptr<QList<std::shared_ptr<T>>> &value)
{
    next = value;
}

/* Templates suck. See
 * https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor
 */
template class TrackEventUnmerge<Tracklet>;

}
