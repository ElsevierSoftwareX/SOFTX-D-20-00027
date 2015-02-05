#include "slice.h"

Slice::Slice()
{
    this->id = 0;
    this->slicePos = std::shared_ptr<QPoint>(new QPoint(0,0));
}

Slice::Slice(int id)
{
    this->id = id;
    this->slicePos = std::shared_ptr<QPoint>(new QPoint(0,0));
}

Slice::~Slice()
{
}

void Slice::addChannel(std::shared_ptr<Channel> channel)
{
    return channels.append(channel);
}

std::shared_ptr<Channel> Slice::getChannel(int id) const
{
    for (std::shared_ptr<Channel> c: channels)
        if(c->getChanId() == id)
            return c;
    return nullptr;
}

int Slice::getId() const
{
    return this->id;
}
std::shared_ptr<QPoint> Slice::getSlicePos() const
{
    return slicePos;
}

void Slice::setSlicePos(const std::shared_ptr<QPoint> &value)
{
    slicePos = value;
}

void Slice::setSlicePos(int x, int y)
{
    slicePos = std::shared_ptr<QPoint>(new QPoint(x,y));
}

void Slice::addObject(const std::shared_ptr<Object> o)
{
    return objects.append(o);
}

std::shared_ptr<Object> Slice::getObject(uint32_t id) const
{
    for (std::shared_ptr<Object> o:objects){
        if(o->getID() == id)
            return o;
    }
    return std::shared_ptr<Object>();
}


std::ostream &operator<<(std::ostream &strm, const Slice &s)
{
    strm << "          Slice:" << std::endl;
    strm << "            id: " << s.id << std::endl;
    strm << "            slicePos: " << s.slicePos->x() << "," << s.slicePos->y() << std::endl;
    strm << "            channels:" << std::endl;
    for (std::shared_ptr<Channel> c: s.channels){
        strm << *c;
    }
    strm << "            objects:" << std::endl ;
    for (std::shared_ptr<Object> o: s.objects){
        strm << *o;
    }
   return strm;
}
