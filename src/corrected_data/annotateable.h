#ifndef ANNOTATEABLE_H
#define ANNOTATEABLE_H

#include <string>

namespace CellTracker {

class Annotateable
{
public:
    enum ANNOTATION_TYPE {
        OBJECT_ANNOTATION,
        ELEMENT_ANNOTATION,
        TRACKLET_ANNOTATION
    };

    Annotateable();
    Annotateable(ANNOTATION_TYPE);
    ~Annotateable();

    ANNOTATION_TYPE getAnnotationType() const;
    void setAnnotationType(const ANNOTATION_TYPE &value);

private:
    ANNOTATION_TYPE annotationType;
};

}

#endif // ANNOTATEABLE_H
