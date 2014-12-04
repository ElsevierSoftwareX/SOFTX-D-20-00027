#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <QImage>

#include "object.h"

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
