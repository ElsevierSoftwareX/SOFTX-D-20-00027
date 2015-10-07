#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <QObject>
#include <QString>

#include <iostream>
#include <memory>
#include <string>

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
    friend std::ostream& ::operator<< (std::ostream&, Annotation&);

    Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
public:
    Annotation();
    Annotation(QString, QString);
    Annotation(uint32_t, QString, QString);
    ~Annotation();

    QString getTitle() const;
    void setTitle(const QString &value);

    QString getDescription() const;
    void setDescription(const QString &value);

    uint32_t getId() const;
    void setId(const uint32_t &value);

private:
    uint32_t id;
    QString title;
    QString description;

signals:
    void titleChanged(QString);
    void descriptionChanged(QString);
};

}

#endif // ANNOTATION_H
