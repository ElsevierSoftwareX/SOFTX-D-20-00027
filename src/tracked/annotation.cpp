#include "annotation.h"
#include "provider/idprovider.h"

#include <QDebug>

namespace CellTracker {

Annotation::Annotation() : QObject() {}

Annotation::Annotation(ANNOTATION_TYPE type) :
    QObject(0),
    type(type),
    id(IdProvider::getNewAnnotationId()),
    title("New Annotation"),
    description("Put the description here") {}

Annotation::Annotation(ANNOTATION_TYPE type, QString title, QString description) :
    QObject(0),
    type(type),
    id(IdProvider::getNewAnnotationId()),
    title(title),
    description(description) {}

Annotation::Annotation(ANNOTATION_TYPE type, uint32_t id, QString title, QString description) :
    QObject(0),
    type(type),
    id(IdProvider::claimAnnotationId(id)?id:IdProvider::getNewAnnotationId()),
    title(title),
    description(description) {}

Annotation::~Annotation() {
    IdProvider::returnAnnotationId(id);
}

QString Annotation::getTitle() const
{
    return title;
}

void Annotation::setTitle(const QString &value)
{
    if (title != value)
        emit titleChanged(title = value);
}

QString Annotation::getDescription() const
{
    return description;
}

void Annotation::setDescription(const QString &value)
{
    if (description != value)
        emit descriptionChanged(description = value);
}

uint32_t Annotation::getId() const
{
    return id;
}

void Annotation::setId(const uint32_t &value)
{
    if (id != value)
        emit idChanged(id = value);
}

Annotation::ANNOTATION_TYPE Annotation::getType() const
{
    return type;
}

void Annotation::setType(const ANNOTATION_TYPE &value)
{
    if (type != value)
        emit(type = value);
}

}

std::ostream &operator<<(std::ostream &strm, CellTracker::Annotation &a)
{
    strm << "Annotation " << std::to_string(a.id) << std::endl;
    strm << a.title.toStdString() << std::endl;
    strm << a.description.toStdString() << std::endl;
    return strm;
}
