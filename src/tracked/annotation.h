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
    ANNOTATION_TYPE type;
    uint32_t id;
    QString title;
    QString description;

signals:
    void typeChanged(ANNOTATION_TYPE);
    void idChanged(int);
    void titleChanged(QString);
    void descriptionChanged(QString);
};

}

#endif // ANNOTATION_H
