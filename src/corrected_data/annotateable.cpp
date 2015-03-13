#include "annotateable.h"

namespace CellTracker {

Annotateable::Annotateable()
{

}

Annotateable::Annotateable(Annotateable::ANNOTATION_TYPE t)
{
    this->annotationType = t;
}

Annotateable::~Annotateable()
{

}

}
