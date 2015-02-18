#ifndef TRACKEVENTLOST_H
#define TRACKEVENTLOST_H

#include <memory>

#include "trackevent.h"
#include "tracklet.h"

/*!
 * \brief The TrackEventLost class
 *
 * TrackEventLost is intended for the situation in which an object ist lost (and
 * doesn't reapper or cannot realiably be assigned to an existing Tracklet.
 *
 * Consequently this TrackEvent only holds the previous Tracklet.
 */
class TrackEventLost : public TrackEvent
{
public:
    TrackEventLost();

    std::shared_ptr<Tracklet> getPrev() const;
    void setPrev(const std::shared_ptr<Tracklet> &value);

private:
    std::shared_ptr<Tracklet> prev; /*!< The previous Tracklet, which is now lost */
};

#endif // TRACKEVENTLOST_H
