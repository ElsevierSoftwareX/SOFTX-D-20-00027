#include "annotation.h"

Annotation::Annotation()
{
}

Annotation::Annotation(Annotation::ANNOTATION_TYPE t, void *o)
{
    this->type = t;
    this->annotated = o;
}
