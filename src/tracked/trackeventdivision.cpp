#include "base/autotracklet.h"
#include "trackeventdivision.h"

namespace CellTracker {

/*!
 * \brief constructs a new TrackEventDivision
 */
template <typename T>
TrackEventDivision<T>::TrackEventDivision() :
    TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_DIVISION),
    next(std::make_shared<QList<std::shared_ptr<T>>>()) {}

/*!
 * \brief returns the previous (Auto)Tracklet
 * \return the previous (Auto)Tracklet
 */
template <typename T>
std::shared_ptr<T>
TrackEventDivision<T>::getPrev() const
{
    return prev;
}

/*!
 * \brief sets the previous (Auto)Tracklet
 * \param value the previous (Auto)Tracklet to set
 */
template <typename T>
void
TrackEventDivision<T>::setPrev(const std::shared_ptr<T> &value)
{
    prev = value;
}

/*!
 * \brief returns a QList of next (Auto)Tracklet%s
 * \return a Qlist of next (Auto)Tracklet%s
 */
template <typename T>
std::shared_ptr<QList<std::shared_ptr<T>>>
TrackEventDivision<T>::getNext() const
{
    return next;
}

/*!
 * \brief sets the QList of next (Auto)Tracklet%s
 * \param value the QList of next (Auto)Tracklet%s to set
 */
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
