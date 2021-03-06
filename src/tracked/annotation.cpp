/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2017, 2016, 2015 Sebastian Wagner
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
#include "annotation.h"
#include "provider/idprovider.h"

#include <QDebug>

namespace TraCurate {

Annotation::Annotation() : QObject() {}

/*!
 * \brief constructs a new Annotation
 * \param type_ the type of the Annotation
 */
Annotation::Annotation(ANNOTATION_TYPE type_) :
    QObject(0),
    type(type_),
    id(IdProvider::getNewAnnotationId()),
    title("New Annotation"),
    description("Put the description here") {}

/*!
 * \brief constructs a new Annotation
 * \param type_ the type of the Annotation
 * \param title_ the title of the Annotation
 * \param description_ the description of the Annotation
 */
Annotation::Annotation(ANNOTATION_TYPE type_, QString title_, QString description_) :
    QObject(0),
    type(type_),
    id(IdProvider::getNewAnnotationId()),
    title(title_),
    description(description_) {}

/*!
 * \brief constructs a new Annotation
 * \param type_ the type of the Annotation
 * \param id_ the ID of the Annotation
 * \param title_ the title of the Annotation
 * \param description_ the description of the Annotation
 *
 * \warning it is in no way guaranteed, that the Annotation will be given the
 * requested ID, so you have to check this yourself after constructing the
 * Annotation. This is mainly used for deserialization, when no AnnotationIDs
 * were handed out beforehand and no IDs are claimed more than once.
 */
Annotation::Annotation(ANNOTATION_TYPE type_, uint32_t id_, QString title_, QString description_) :
    QObject(0),
    type(type_),
    id(IdProvider::claimAnnotationId(id_)?id_:IdProvider::getNewAnnotationId()),
    title(title_),
    description(description_) {}

/*!
 * \brief destructs an Annotation and returns its ID to the IDProvider
 */
Annotation::~Annotation() {
    IdProvider::returnAnnotationId(id);
}

/*!
 * \brief returns the Title of this Annotation
 * \return the title of this Annotation
 */
QString Annotation::getTitle() const
{
    return title;
}

/*!
 * \brief sets the Title of this Annotation
 * \param value the title to set
 */
void Annotation::setTitle(const QString &value)
{
    if (title != value)
        emit titleChanged(title = value);
}

/*!
 * \brief returns the Description of this Annotation
 * \return the description of this Annotation
 */
QString Annotation::getDescription() const
{
    return description;
}

/*!
 * \brief sets the Description of this Annotation
 * \param value the description to set
 */
void Annotation::setDescription(const QString &value)
{
    if (description != value)
        emit descriptionChanged(description = value);
}

/*!
 * \brief returns the ID of this Annotation
 * \return the ID of this Annotation
 */
uint32_t Annotation::getId() const
{
    return id;
}

/*!
 * \brief sets the ID of this Annotation
 * \param value the ID to set
 * \warning IDProvider is circumvented here, so using this function might lead
 * to horrible breakage
 */
void Annotation::setId(const uint32_t &value)
{
    if (id != value)
        emit idChanged(id = value);
}

/*!
 * \brief returns the type of this Annotation
 * \return the type of this Annotation
 */
Annotation::ANNOTATION_TYPE Annotation::getType() const
{
    return type;
}

/*!
 * \brief sets the type of this Annotation
 * \param value the type to set
 */
void Annotation::setType(const ANNOTATION_TYPE &value)
{
    if (type != value)
        emit(type = value);
}

}

std::ostream &operator<<(std::ostream &strm, TraCurate::Annotation &a)
{
    strm << "Annotation " << std::to_string(a.id) << std::endl;
    strm << a.title.toStdString() << std::endl;
    strm << a.description.toStdString() << std::endl;
    return strm;
}
