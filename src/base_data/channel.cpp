#include "channel.h"

#include <iostream>

Channel::Channel()
{
    this->id = 0;
}

Channel::Channel(int id)
{
    this->id = id;
}

Channel::~Channel()
{
}

void Channel::setImage(const std::shared_ptr<QImage> img)
{
    image = img;
}

int Channel::getChanId() const
{
    return this->id;
}

std::shared_ptr<QImage> Channel::getImage() const
{
   return this->image;
}


std::ostream &operator<<(std::ostream &strm, const Channel &c)
{
    strm << "            Channel:" << std::endl;
    strm << "              id: " << std::to_string(c.id) << std::endl;
    strm << "              image: " << c.image << std::endl;
    return strm;
}
