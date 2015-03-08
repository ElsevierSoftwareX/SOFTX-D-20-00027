#include "trackletregular.h"

namespace CellTracker {

TrackletRegular::TrackletRegular() : Tracklet(TRACKLET_REGULAR)
{
}

TrackletRegular::TrackletRegular(std::shared_ptr<AutoTracklet> base) : Tracklet(TRACKLET_REGULAR)
{
    this->baseTracklet = base;
}

}
