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
    friend std::ostream& operator<< (std::ostream&, Annotation&);

public:
    enum ANNOTATION_TYPE {
        OBJECT_ANNOTATION,
        ELEMENT_ANNOTATION,
        TRACK_ANNOTATION
    };

    Annotation();
    Annotation(ANNOTATION_TYPE, void*);

    std::string getText() const;
    void setText(const std::string &value);

private:
    ANNOTATION_TYPE type;
    std::string text;
    void *annotated;
};

std::ostream& operator<< (std::ostream&, Annotation&);
#endif // ANNOTATION_H
