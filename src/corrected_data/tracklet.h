#ifndef TRACKLET_H
#define TRACKLET_H

#include "trackelement.h"

class Tracklet : public TrackElement
{
    enum TRACKLET_TYPE {
        TRACKLET_REGULAR,
        TRACKLET_MERGED,
        TRACKLET_SPLIT
    };

public:
    Tracklet();

private:
    TRACKLET_TYPE type;
};

#endif // TRACKLET_H
