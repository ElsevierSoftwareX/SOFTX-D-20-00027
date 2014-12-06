#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <string>

/*!
 * \brief The Annotation class
 *
 * Objects and Tracklets may be annotated with a text. The type of the
 * annotation should be set correctly.
 */
class Annotation
{
public:
    enum ANNOTATION_TYPE {
        OBJECT_ANNOTATION,
        ELEMENT_ANNOTATION
    };

    Annotation();

private:
    ANNOTATION_TYPE type;
    std::string text;
    void *annotated;
};

#endif // ANNOTATION_H
