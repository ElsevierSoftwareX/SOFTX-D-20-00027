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
#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <QObject>
#include <QString>

#include <iostream>
#include <memory>
#include <string>

namespace TraCurate { class Annotation; }
std::ostream& operator<< (std::ostream&, TraCurate::Annotation&);

namespace TraCurate {

/*!
 * \brief The Annotation class
 *
 * Objects and Tracklets may be annotated with a text. The type of the
 * annotation should be set correctly. If other Classes should be annotated,
 *  extend the enum ANNOTATION_TYPE and let your Class inherit Annotateable
 */
class Annotation : public QObject
{
    Q_OBJECT

    friend std::ostream& ::operator<< (std::ostream&, Annotation&);

    Q_PROPERTY(ANNOTATION_TYPE type READ getType WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(int id READ getId WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
public:
    enum ANNOTATION_TYPE {
        OBJECT_ANNOTATION,
        TRACKLET_ANNOTATION
    };
    Q_ENUMS(ANNOTATION_TYPE)

    Annotation();
    Annotation(ANNOTATION_TYPE);
    Annotation(ANNOTATION_TYPE, QString, QString);
    Annotation(ANNOTATION_TYPE, uint32_t, QString, QString);
    ~Annotation();

    QString getTitle() const;
    void setTitle(const QString &value);

    QString getDescription() const;
    void setDescription(const QString &value);

    uint32_t getId() const;
    void setId(const uint32_t &value);

    ANNOTATION_TYPE getType() const;
    void setType(const ANNOTATION_TYPE &value);

private:
    ANNOTATION_TYPE type; /*!< the type of this Annotation */
    uint32_t id;          /*!< the ID of this Annotation */
    QString title;        /*!< the title of this Annotation */
    QString description;  /*!< the description of this Annotation */

signals:
    void typeChanged(ANNOTATION_TYPE);
    void idChanged(int);
    void titleChanged(QString);
    void descriptionChanged(QString);
};

}

#endif // ANNOTATION_H
