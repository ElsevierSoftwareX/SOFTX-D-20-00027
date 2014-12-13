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
    if (id >= 0 && id < objects.size()){
        /*! \todo compare the objects */
        return objects.at(id);
    }else{
        return nullptr;
    }
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
