#ifndef OBJECT_H
#define OBJECT_H

#include "tracked/annotateable.h"

#include <cstdint>
#include <iostream>
#include <memory>

#include <QPoint>
#include <QPolygonF>
#include <QRect>

namespace CellTracker { class Object; }
std::ostream& operator<< (std::ostream&, CellTracker::Object&);

namespace CellTracker {

/*!
 * \brief The Object class
 *
 * An Object represents something that is automatically recognized in the Image
 * that it is contained in.
 */
class Object : public Annotateable
{
public:
    Object();
    Object(uint32_t id, uint32_t chanId, uint32_t sliceId, uint32_t frameId);
    ~Object();

    uint32_t getId() const;
    uint32_t getTrackId() const;
    uint32_t getChannelId() const;
    uint32_t getSliceId() const;
    uint32_t getFrameId() const;
    uint32_t getAutoId() const;

    bool isInTracklet() const;
    bool isInAutoTracklet() const;

    std::shared_ptr<QRect> getBoundingBox() const;
    std::shared_ptr<QPolygonF> getOutline() const;
    std::shared_ptr<QPoint> getCentroid() const;

    void setId(uint32_t);
    void setTrackId(const uint32_t &trackId);
    void setAutoId(const uint32_t &autoId);

    void setCentroid(std::shared_ptr<QPoint>);
    void setBoundingBox(std::shared_ptr<QRect>);
    void setOutline(std::shared_ptr<QPolygonF>);

    friend std::ostream& ::operator<< (std::ostream&, Object&);

private:
    uint32_t id;        /*!< The ID of this Object */
    uint32_t chanId;
    uint32_t sliceId;
    uint32_t frameId;
    uint32_t trackId;   /*!< The trackId, with wich this Object is associated */
    uint32_t autoId;
    std::shared_ptr<QPoint> centroid;   /*!< The center of this Object */
    std::shared_ptr<QRect> boundingBox; /*!< The boundingBox of this Object */
    std::shared_ptr<QPolygonF> outline;  /*!< The outline of this Object */

};

}

#endif // OBJECT_H
