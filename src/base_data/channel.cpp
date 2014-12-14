#include "channel.h"

Channel::Channel()
{
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
    /*! \todo compiler warnings */
    for (std::shared_ptr<Object> o:objects){
        if(o->getID() == id)
            return o;
    }
    return std::shared_ptr<Object>();
}


std::ostream &operator<<(std::ostream &strm, const Channel &c)
{
    strm << "            Channel:\n";
    strm << "              chanName: " << c.chanName.toStdString() << "\n";
    strm << "              image: " << c.image << "\n";
    strm << "              objects:\n" ;
    for (std::shared_ptr<Object> o: c.objects){
        strm << *o;
    }
    return strm;
}
