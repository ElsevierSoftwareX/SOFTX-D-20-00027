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
    Channel(QString);
    ~Channel();
    void setImage(std::shared_ptr<QImage>);
    void setChanName(QString);
    void addObject(std::shared_ptr<Object>);
    QString getChanName() const;
    std::shared_ptr<Object> getObject(uint32_t);
    friend std::ostream &operator<<(std::ostream&, const Channel&);

private:
    QString chanName;
    std::shared_ptr<QImage> image;
    QList<std::shared_ptr<Object>> objects;
};

std::ostream &operator<<(std::ostream&, const Channel&);
#endif // CHANNEL_H
