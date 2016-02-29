#include "trackeventendofmovie.h"

namespace CellTracker {

/*!
 * \brief constructs a new TrackEventEndOfMovie
 */
template <typename T>
TrackEventEndOfMovie<T>::TrackEventEndOfMovie() :
    TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_ENDOFMOVIE) {}

/*!
 * \brief returns the previous Tracklet
 * \return the previous Tracklet
 */
template <typename T>
std::shared_ptr<T>
TrackEventEndOfMovie<T>::getPrev() const
{
    return prev;
}

/*!
 * \brief sets the previous Tracklet
 * \param value the Tracklet to set as previous
 */
template <typename T>
void
TrackEventEndOfMovie<T>::setPrev(const std::shared_ptr<T> &value)
{
    prev = value;
}

/* Templates suck. See
 * https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor
 */
template class TrackEventEndOfMovie<Tracklet>;

}
