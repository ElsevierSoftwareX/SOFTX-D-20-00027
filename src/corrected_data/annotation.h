#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <iostream>
#include <memory>
#include <string>

#include "annotateable.h"

namespace CellTracker { class Annotation; }
std::ostream& operator<< (std::ostream&, CellTracker::Annotation&);

namespace CellTracker {

/*!
 * \brief The Annotation class
 *
 * Objects and Tracklets may be annotated with a text. The type of the
 * annotation should be set correctly.
 */
class Annotation
{
    friend std::ostream& ::operator<< (std::ostream&, Annotation&);

public:

    Annotation();
    Annotation(Annotateable::ANNOTATION_TYPE);

    std::string getAnnotationText() const;
    void setAnnotationText(const std::string &value);

    std::shared_ptr<Annotateable> getAnnotated() const;
    void setAnnotated(const std::shared_ptr<Annotateable> &value);

private:
    std::string annotationText;
    std::shared_ptr<Annotateable> annotated;
};

}

#endif // ANNOTATION_H
