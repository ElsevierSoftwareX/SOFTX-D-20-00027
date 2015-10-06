#include "trackelement.h"

namespace CellTracker {

TrackElement::TrackElement()
{
}

TrackElement::TrackElement(TrackElement::ELEMENT_TYPE type)
{
   this->elemType =  type;
}

}
