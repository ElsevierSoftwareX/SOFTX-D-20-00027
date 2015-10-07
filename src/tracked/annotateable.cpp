#include "annotateable.h"

#include <iostream>
namespace CellTracker {

Annotateable::Annotateable(Annotateable::ANNOTATION_TYPE t) :
    annotationType(t),
    annotations(new QList<std::shared_ptr<Annotation>>()) { }

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

std::shared_ptr<QList<std::shared_ptr<Annotation> > > Annotateable::getAnnotations() const
{
    return annotations;
}

void Annotateable::setAnnotations(const std::shared_ptr<QList<std::shared_ptr<Annotation> > > &value)
{
    annotations = value;
}

}

std::ostream &operator<<(std::ostream &strm, CellTracker::Annotateable &a)
{
    switch (a.annotationType) {
    case CellTracker::Annotateable::OBJECT_ANNOTATION:
        strm << "[OBJECT_ANNOTATION] ";
        break;
    case CellTracker::Annotateable::ELEMENT_ANNOTATION:
        strm << "[ELEMENT_ANNOTATION] ";
        break;
    case CellTracker::Annotateable::TRACKLET_ANNOTATION:
        strm << "[TRACKLET_ANNOTATION] ";
        break;
    }
    strm << std::endl;
    for (std::shared_ptr<CellTracker::Annotation> an : *a.annotations) {
        strm << *an << std::endl;
    }
    return strm;
}

