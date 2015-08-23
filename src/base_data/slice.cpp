#include "slice.h"

#include <QDebug>

namespace CellTracker {

Slice::Slice() : sliceId(0), slicePos(std::shared_ptr<QPoint>(new QPoint(0,0))) { }

Slice::Slice(uint32_t id, uint32_t frameId) : sliceId(id), frameId(frameId), slicePos(std::shared_ptr<QPoint>(new QPoint(0,0))) {}

Slice::~Slice() { }

void Slice::addChannel(std::shared_ptr<Channel> channel)
{
    channels.insert(channel->getChanId(),channel);
}

std::shared_ptr<Channel> Slice::getChannel(uint32_t id) const
{
    return channels.value(id,nullptr);
}

//uint32_t Slice::getFrameId() const
//{
//    return this->frameId;
//}
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

//void Slice::addObject(const std::shared_ptr<Object> &o)
//{
//    objects.insert(o->getId(),o);
//}

//int Slice::removeObject(uint32_t id)
//{
//    return objects.remove(id);
//}

QHash<uint32_t, std::shared_ptr<Channel> > Slice::getChannels() const
{
    return channels;
}
uint32_t Slice::getSliceId() const
{
    return sliceId;
}
uint32_t Slice::getFrameId() const
{
    return frameId;
}


//std::shared_ptr<Object> Slice::getObject(uint32_t id) const
//{
//    return objects.value(id,nullptr);
//}

//QHash<uint32_t,std::shared_ptr<Object>> Slice::getObjects()
//{
//    return objects;
//}

}

std::ostream &operator<<(std::ostream &strm, const CellTracker::Slice &s)
{
    strm << "          Slice:" << std::endl;
    strm << "            id: " << s.sliceId << std::endl;
    strm << "            slicePos: " << s.slicePos->x() << "," << s.slicePos->y() << std::endl;
    strm << "            channels:" << std::endl;
    for (std::shared_ptr<CellTracker::Channel> c: s.channels){
        strm << *c;
    }
    strm << "            objects:" << std::endl ;
//    for (std::shared_ptr<CellTracker::Object> o: s.objects){
//        strm << *o;
//    }
   return strm;
}
