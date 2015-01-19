#include "channel.h"

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

void Channel::addObject(const std::shared_ptr<Object> o)
{
    return objects.append(o);
}

std::shared_ptr<Object> Channel::getObject(uint32_t id) const
{
    for (std::shared_ptr<Object> o:objects){
        if(o->getID() == id)
            return o;
    }
    return std::shared_ptr<Object>();
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
    strm << "              objects:" << std::endl ;
    for (std::shared_ptr<Object> o: c.objects){
        strm << *o;
    }
    return strm;
}
