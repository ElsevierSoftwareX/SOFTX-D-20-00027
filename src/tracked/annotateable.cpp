#include "annotateable.h"

#include <iostream>
namespace CellTracker {

Annotateable::Annotateable() :
    annotations(new QList<std::shared_ptr<Annotation>>()) { }

Annotateable::~Annotateable()
{

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
    strm << std::endl;
    for (std::shared_ptr<CellTracker::Annotation> an : *a.annotations) {
        strm << *an << std::endl;
    }
    return strm;
}

