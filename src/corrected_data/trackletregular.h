#ifndef TRACKLETREGULAR_H
#define TRACKLETREGULAR_H

#include "tracklet.h"

#include "auto_tracklets/autotracklet.h"

namespace CellTracker {

/*!
 * \brief The TrackletRegular class
 *
 * TrackletRegular is used in situations, when the automatically generated
 * Tracklet fits and nothing else has to be done.
 */
class TrackletRegular : public Tracklet
{
public:
    TrackletRegular();

private:
    AutoTracklet baseTracklet;
};

}

#endif // TRACKLETREGULAR_H
