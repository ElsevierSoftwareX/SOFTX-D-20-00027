#include "channel.h"

Channel::Channel()
{
}

Channel::Channel(QString name)
{
    chanName = name;
}

Channel::~Channel()
{
}

void Channel::setImage(std::shared_ptr<QImage> img)
{
    image = img;
}

void Channel::setChanName(QString name)
{
    chanName = name;
}

QString Channel::getChanName() const
{
    return chanName;
}

void Channel::addObject(std::shared_ptr<Object> o)
{
    return objects.append(o);
}

std::shared_ptr<Object> Channel::getObject(uint32_t id)
{
    for (std::shared_ptr<Object> o:objects){
        if(o->getID() == id)
            return o;
    }
    return std::shared_ptr<Object>();
}


std::ostream &operator<<(std::ostream &strm, const Channel &c)
{
    strm << "            Channel:" << std::endl;
    strm << "              chanName: " << c.chanName.toStdString() << std::endl;
    strm << "              image: " << c.image << std::endl;
    strm << "              objects:" << std::endl ;
    for (std::shared_ptr<Object> o: c.objects){
        strm << *o;
    }
    return strm;
}
