#include "annotation.h"
#include "provider/idprovider.h"

namespace CellTracker {

Annotation::Annotation() : QObject(0) { }

Annotation::Annotation(QString title, QString description) : QObject(0), title(title), description(description) {
    id = IdProvider::getNewAnnotationId();
}

Annotation::Annotation(uint32_t id, QString title, QString description) : title(title), description(description)
{
    id = (IdProvider::claimAnnotationId(id))?id:IdProvider::getNewAnnotationId();
}

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
    id = value;
}

}

std::ostream &operator<<(std::ostream &strm, CellTracker::Annotation &a)
{
    strm << "Annotation " << std::to_string(a.id) << std::endl;
    strm << a.title.toStdString() << std::endl;
    strm << a.description.toStdString() << std::endl;
    return strm;
}
