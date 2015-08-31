#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <QObject>
#include <QString>

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
class Annotation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)

    friend std::ostream& ::operator<< (std::ostream&, Annotation&);

public:
    Annotation();

    std::shared_ptr<Annotateable> getAnnotated() const;
    void setAnnotated(const std::shared_ptr<Annotateable> &value);

    Q_INVOKABLE QString getTitle() const;
    Q_INVOKABLE void setTitle(const QString &value);

    Q_INVOKABLE QString getDescription() const;
    Q_INVOKABLE void setDescription(const QString &value);

private:
    QString title;
    QString description;
    std::shared_ptr<Annotateable> annotated;

signals:
    void titleChanged(QString);
    void descriptionChanged(QString);

};

}

#endif // ANNOTATION_H
