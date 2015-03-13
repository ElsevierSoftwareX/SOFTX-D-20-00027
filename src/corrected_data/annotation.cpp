#include "annotation.h"

namespace CellTracker {

Annotation::Annotation()
{

}

std::shared_ptr<Annotateable> Annotation::getAnnotated() const
{
    return annotated;
}

void Annotation::setAnnotated(const std::shared_ptr<Annotateable> &value)
{
    annotated = value;
}
std::string Annotation::getAnnotationText() const
{
    return annotationText;
}

void Annotation::setAnnotationText(const std::string &value)
{
    annotationText = value;
}



}

std::ostream &operator<<(std::ostream &strm, CellTracker::Annotation &a)
{
    strm << "Annotation: ";
    std::shared_ptr<CellTracker::Annotateable> annotated = a.getAnnotated();
    switch (annotated->getAnnotationType()) {
    case CellTracker::Annotateable::OBJECT_ANNOTATION:
        strm << "[OBJECT_ANNOTATION] ";
        break;
    case CellTracker::Annotateable::ELEMENT_ANNOTATION:
        strm << "[ELEMENT_ANNOTATION] ";
        break;
    case CellTracker::Annotateable::TRACKLET_ANNOTATION:
        strm << "[TRACKLET_ANNOTATION] ";
        break;
    default:
        strm << "[UNKNOWN] ";
    }
    strm << a.annotationText << std::endl;

    return strm;
}
