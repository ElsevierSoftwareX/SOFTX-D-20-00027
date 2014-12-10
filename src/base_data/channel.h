#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <QImage>
#include <QString>

#include "object.h"

/*!
 * \brief The Channel class
 *
 * A Channel actually contains the QImage to display. As there can be more than
 * one Channel, either one QImage%s should be selected or more QImage%s should
 * be composed into one. Each Channel holds a list of Object%s that are present
 * in the QImage.
 */
class Channel
{
public:
    Channel();
    ~Channel();
    void setImage(QImage *);
    void setChanName(QString);
    QString getChanName();
    void addObject(Object *);
    friend std::ostream &operator<<(std::ostream&, const Channel&);

private:
    QString chanName;
    QImage *image;
    QList<Object*> objects;
};

std::ostream &operator<<(std::ostream&, const Channel&);
#endif // CHANNEL_H
