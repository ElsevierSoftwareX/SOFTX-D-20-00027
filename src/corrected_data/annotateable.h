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

private:
    ANNOTATION_TYPE annotationType;
};

}

#endif // ANNOTATEABLE_H
