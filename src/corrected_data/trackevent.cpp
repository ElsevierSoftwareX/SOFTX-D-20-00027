#include "trackevent.h"

TrackEvent::TrackEvent() : TrackElement(ELEMENT_EVENT)
{
}

TrackEvent::TrackEvent(TrackEvent::EVENT_TYPE t) : TrackElement(ELEMENT_EVENT)
{
    this->type = t;
}


std::ostream &operator<<(std::ostream &strm, TrackEvent &t)
{
    strm << "TrackEvent: " << t.type;
    return strm;
}
