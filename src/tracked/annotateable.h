#ifndef ANNOTATEABLE_H
#define ANNOTATEABLE_H

#include <iostream>
#include <memory>
#include <string>

#include "annotation.h"

namespace CellTracker { class Annotateable; }
std::ostream& operator<< (std::ostream&, CellTracker::Annotateable&);

namespace CellTracker {
class Annotateable
{
    friend std::ostream& ::operator<< (std::ostream&, Annotateable&);
public:
    enum ANNOTATION_TYPE {
        OBJECT_ANNOTATION,
        TRACKLET_ANNOTATION
    };

    Annotateable(ANNOTATION_TYPE);
    ~Annotateable();

    ANNOTATION_TYPE getAnnotationType() const;
    void setAnnotationType(const ANNOTATION_TYPE &value);

    std::shared_ptr<QList<std::shared_ptr<Annotation>>> getAnnotations() const;
    void setAnnotations(const std::shared_ptr<QList<std::shared_ptr<Annotation>>> &value);

private:
    ANNOTATION_TYPE annotationType;
    std::shared_ptr<QList<std::shared_ptr<Annotation>>> annotations;
};

}

#endif // ANNOTATEABLE_H
