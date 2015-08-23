#ifndef CHANNEL_H
#define CHANNEL_H

#include "object.h"

#include <iostream>
#include <string>
#include <memory>

#include <QImage>
#include <QHash>

namespace CellTracker { class Channel; }
std::ostream &operator<<(std::ostream&, const CellTracker::Channel&);

namespace CellTracker {

/*!
 * \brief The Channel class
 *
 * A Channel actually contains the QImage to display. As there can be more than
 * one Channel, either one QImage%s should be selected or more QImage%s should
 * be composed into one. Each Channel holds a list of Object%s that are present
 * in the QImage.
 */
class Channel
{
public:
    Channel();
    Channel(uint32_t chanId, uint32_t sliceId, uint32_t frameId);
    ~Channel();

    std::shared_ptr<QImage> getImage() const;

    void setImage(const std::shared_ptr<QImage> &);

    friend std::ostream& ::operator<<(std::ostream&, const Channel&);

    void addObject(const std::shared_ptr<Object> &);
    int removeObject(uint32_t);
    std::shared_ptr<Object> getObject(uint32_t) const;
    QHash<uint32_t,std::shared_ptr<Object>> getObjects();

    uint32_t getChanId() const;
    uint32_t getSliceId() const;
    uint32_t getFrameId() const;

private:
    uint32_t chanId;
    uint32_t sliceId = UINT32_MAX;
    uint32_t frameId = UINT32_MAX;
    std::shared_ptr<QImage> image;
    QHash<uint32_t,std::shared_ptr<Object>> objects;
};

}

#endif // CHANNEL_H
