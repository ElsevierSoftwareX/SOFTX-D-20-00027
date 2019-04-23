/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Sebastian Wagner
 *
 * TraCurate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TraCurate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with TraCurate.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef ANNOTATEABLE_H
#define ANNOTATEABLE_H

#include <iostream>
#include <memory>
#include <string>

#include "annotation.h"

namespace TraCurate { class Annotateable; }
std::ostream& operator<< (std::ostream&, TraCurate::Annotateable&);

namespace TraCurate {
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
    ~Annotateable() = default;

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
