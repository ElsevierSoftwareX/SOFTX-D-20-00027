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
Annotateable::ANNOTATION_TYPE Annotateable::getAnnotationType() const
{
    return annotationType;
}

void Annotateable::setAnnotationType(const ANNOTATION_TYPE &value)
{
    annotationType = value;
}

}
