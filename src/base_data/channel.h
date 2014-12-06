#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <QImage>

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

private:
    std::string chanName;
    QImage image;
    QList<Object> objects;
};

#endif // CHANNEL_H
