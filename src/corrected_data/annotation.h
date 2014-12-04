#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <string>

class Annotation
{
    enum ANNOTATION_TYPE {
        OBJECT_ANNOTATION,
        ELEMENT_ANNOTATION
    };

public:
    Annotation();

private:
    ANNOTATION_TYPE type;
    std::string text;
};

#endif // ANNOTATION_H
