#include "channel.h"

Channel::Channel()
{
}

Channel::~Channel()
{
    delete image;
    for (Object *o: objects)
        delete o;
}

void Channel::setImage(QImage *img)
{
    image = img;
}

void Channel::setChanName(QString name)
{
    chanName = name;
}

QString Channel::getChanName()
{
    return chanName;
}

void Channel::addObject(Object *o)
{
    return objects.append(o);
}


std::ostream &operator<<(std::ostream &strm, const Channel &c)
{
    strm << "            Channel:\n";
    strm << "              chanName: " << c.chanName.toStdString() << "\n";
    strm << "              image: " << c.image << "\n";
    strm << "              objects:\n" ;
    for (Object *o: c.objects){
        strm << *o;
    }
    return strm;
}
