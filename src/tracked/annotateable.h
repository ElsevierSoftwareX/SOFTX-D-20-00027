#ifndef ANNOTATEABLE_H
#define ANNOTATEABLE_H

#include <iostream>
#include <memory>
#include <string>

#include "annotation.h"

namespace CellTracker { class Annotateable; }
std::ostream& operator<< (std::ostream&, CellTracker::Annotateable&);

namespace CellTracker {
/*!
 * \brief The Annotateable class
 *
 * Allows Classes inheriting Annotateable to become annotateable (who would have guessed?).
 * Provides the means to hold a QList of Annotation%s, add new Annotation%s to this list
 * and remove them again. Also allows an Annotatable to be checked, if it actually holds any
 * Annotation%s.
 */
class Annotateable
{
    friend std::ostream& ::operator<< (std::ostream&, Annotateable&);
public:
    enum ANNOTATION_TYPE {
        OBJECT_ANNOTATION,
        TRACKLET_ANNOTATION
    };

    Annotateable();
    ~Annotateable();

    std::shared_ptr<QList<std::shared_ptr<Annotation>>> getAnnotations() const;
    void setAnnotations(const std::shared_ptr<QList<std::shared_ptr<Annotation>>> &value);
    void annotate(std::shared_ptr<Annotation>);
    void unannotate(std::shared_ptr<Annotation>);
    bool isAnnotated();
    bool isAnnotatedWith(std::shared_ptr<Annotation>);

private:
    std::shared_ptr<QList<std::shared_ptr<Annotation>>> annotations; /*!< the QList of Annotations */
};

}

#endif // ANNOTATEABLE_H
