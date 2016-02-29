#include "trackeventmerge.h"

namespace CellTracker {

/*!
 * \brief constructs a TrackEventMerge
 */
template <typename T>
TrackEventMerge<T>::TrackEventMerge() :
    TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_MERGE),
    prev(std::shared_ptr<QList<std::shared_ptr<T>>>(new QList<std::shared_ptr<T>>())) {}

/*!
 * \brief returns a QList of previous Tracklets
 * \return a QList of previous Tracklets
 */
template <typename T>
std::shared_ptr<QList<std::shared_ptr<T>>>
TrackEventMerge<T>::getPrev() const
{
    return prev;
}

/*!
 * \brief sets the QList of previous Tracklets
 * \param value the QList of previous Tracklets to set
 */
template <typename T>
void
TrackEventMerge<T>::setPrev(const std::shared_ptr<QList<std::shared_ptr<T> > > &value)
{
    prev = value;
}

/*!
 * \brief returns the next Tracklet
 * return the next Tracklet
 */
template <typename T>
std::shared_ptr<T>
TrackEventMerge<T>::getNext() const
{
    return next;
}

/*!
 * \brief sets the next Tracklet
 * \param value the next Tracklet to set
 */
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
