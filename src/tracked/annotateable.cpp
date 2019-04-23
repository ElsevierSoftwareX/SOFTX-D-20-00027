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
#include "annotateable.h"

#include <iostream>
namespace TraCurate {

/*!
 * \brief constructor for Annotateable
 */
Annotateable::Annotateable() :
    annotations(new QList<std::shared_ptr<Annotation>>()) { }

/*!
 * \brief returns the Annotations of this Annotateable
 * \return a QList of Annotations of this Annotateable
 */
std::shared_ptr<QList<std::shared_ptr<Annotation> > > Annotateable::getAnnotations() const
{
    return annotations;
}

/*!
 * \brief sets the Annotations of this Annotateable
 * \param value the Annotations to set
 */
void Annotateable::setAnnotations(const std::shared_ptr<QList<std::shared_ptr<Annotation> > > &value)
{
    annotations = value;
}

/*!
 * \brief adds an Annotation to the QList of Annotation%s
 * \param a the Annotation to add
 */
void Annotateable::annotate(std::shared_ptr<Annotation> a)
{
    if (!a || annotations->contains(a))
        return;
    annotations->append(a);
}

/*!
 * \brief removes an Annotation from the QList of Annotation%s
 * \param a the Annotation to remove
 */
void Annotateable::unannotate(std::shared_ptr<Annotation> a)
{
    if (!a)
        return;
    annotations->removeOne(a);
}

/*!
 * \brief returns, if this Annotateable is annotated
 * \return true, if this Annotateable is annotated, false if not
 */
bool Annotateable::isAnnotated()
{
    return !annotations->isEmpty();
}

/*!
 * \brief returns, whether this Annotateable is annotated with a specific annotation
 * \param annotation the Annotation to check against
 * \return true if it is annotated, false, if not
 */
bool Annotateable::isAnnotatedWith(std::shared_ptr<Annotation> annotation)
{
    return annotations->contains(annotation);
}

}

std::ostream &operator<<(std::ostream &strm, TraCurate::Annotateable &a)
{
    strm << std::endl;
    for (std::shared_ptr<TraCurate::Annotation> an : *a.annotations) {
        strm << *an << std::endl;
    }
    return strm;
}

