#include "annotation.h"

namespace CellTracker {

Annotation::Annotation()
{
}

Annotation::Annotation(Annotation::ANNOTATION_TYPE t, void *o)
{
    this->type = t;
    this->annotated = o;
}
std::string Annotation::getText() const
{
    return text;
}

void Annotation::setText(const std::string &value)
{
    text = value;
}

}

std::ostream &operator<<(std::ostream &strm, CellTracker::Annotation &a)
{
    strm << "Annotation: ";
    switch (a.type) {
    case CellTracker::Annotation::OBJECT_ANNOTATION:
        strm << "[OBJECT_ANNOTATION] ";
        break;
    case CellTracker::Annotation::ELEMENT_ANNOTATION:
        strm << "[ELEMENT_ANNOTATION] ";
        break;
    case CellTracker::Annotation::TRACK_ANNOTATION:
        strm << "[TRACK_ANNOTATION] ";
        break;
    default:
        strm << "[UNKNOWN] ";
    }
    strm << a.text << std::endl;

    return strm;
}
