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

    Annotateable();
    ~Annotateable();

    std::shared_ptr<QList<std::shared_ptr<Annotation>>> getAnnotations() const;
    void setAnnotations(const std::shared_ptr<QList<std::shared_ptr<Annotation>>> &value);
    void annotate(std::shared_ptr<Annotation>);
    void unannotate(std::shared_ptr<Annotation>);
    bool isAnnotated();
    bool isAnnotatedWith(std::shared_ptr<Annotation>);

private:
    std::shared_ptr<QList<std::shared_ptr<Annotation>>> annotations;
};

}

#endif // ANNOTATEABLE_H
