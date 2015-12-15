#include "channel.h"

namespace CellTracker {

Channel::Channel() :
    chanId(UINT32_MAX),
    sliceId(UINT32_MAX),
    frameId(UINT32_MAX) {}

Channel::Channel(uint32_t chanId, uint32_t sliceId, uint32_t frameId) :
    chanId(chanId),
    sliceId(sliceId),
    frameId(frameId) {}

Channel::~Channel() {}

void Channel::setImage(const std::shared_ptr<QImage> &img)
{
    image = img;
}

uint32_t Channel::getChanId() const
{
    return this->chanId;
}

std::shared_ptr<QImage> Channel::getImage() const
{
   return this->image;
}

void Channel::addObject(const std::shared_ptr<Object> &o)
{
    objects.insert(o->getId(),o);
}

int Channel::removeObject(uint32_t id)
{
    return objects.remove(id);
}

std::shared_ptr<Object> Channel::getObject(uint32_t id) const
{
    return objects.value(id,nullptr);
}

QHash<uint32_t,std::shared_ptr<Object>> Channel::getObjects()
{
    return objects;
}
uint32_t Channel::getSliceId() const
{
    return sliceId;
}

uint32_t Channel::getFrameId() const
{
    return frameId;
}

}

std::ostream &operator<<(std::ostream &strm, const CellTracker::Channel &c)
{
    strm << "            Channel:" << std::endl;
    strm << "              id: " << std::to_string(c.chanId) << std::endl;
    strm << "              image: " << c.image << std::endl;
    return strm;
}
