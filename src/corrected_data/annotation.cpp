#include "annotation.h"

namespace CellTracker {

Annotation::Annotation() { }

std::shared_ptr<Annotateable> Annotation::getAnnotated() const
{
    return annotated;
}

void Annotation::setAnnotated(const std::shared_ptr<Annotateable> &value)
{
    annotated = value;
}
QString Annotation::getTitle() const
{
    return title;
}

void Annotation::setTitle(const QString &value)
{
    title = value;
}

QString Annotation::getDescription() const
{
    return description;
}

void Annotation::setDescription(const QString &value)
{
    description = value;
}

}

std::ostream &operator<<(std::ostream &strm, CellTracker::Annotation &a)
{
    strm << "Annotation: ";
    std::shared_ptr<CellTracker::Annotateable> annotated = a.annotated;
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
    strm << a.title.toStdString() << std::endl;
    strm << a.description.toStdString() << std::endl;

    return strm;
}
