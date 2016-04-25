#include "trackeventunmerge.h"

namespace CellTracker {

/*!
 * \brief constructs a new TrackEventUnmerge
 */
template <typename T>
TrackEventUnmerge<T>::TrackEventUnmerge() :
    TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_UNMERGE),
    next(std::make_shared<QList<std::shared_ptr<T>>>()) {}

/*!
 * \brief returns the previous Tracklet
 * \return the previous Tracklet
 */
template <typename T>
std::shared_ptr<T>
TrackEventUnmerge<T>::getPrev() const
{
    return prev;
}

/*!
 * \brief sets the previous Tracklet
 * \param value the previous Tracklet to set
 */
template <typename T>
void
TrackEventUnmerge<T>::setPrev(const std::shared_ptr<T> &value)
{
    prev = value;
}

/*!
 * \brief returns a QList of next Tracklet%s
 * \return a QList of next Tracklet%s
 */
template <typename T>
std::shared_ptr<QList<std::shared_ptr<T>>>
TrackEventUnmerge<T>::getNext() const
{
    return next;
}

/*!
 * \brief sets the QList of next Tracklet%s
 * \param value the QList of next Tracklet%s to set
 */
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
