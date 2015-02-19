#ifndef OBJECT_H
#define OBJECT_H

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
class Object
{
public:
    Object();
    Object(uint32_t);
    ~Object();

    uint32_t getID() const;
    uint32_t getTrackID() const;

    std::shared_ptr<QPolygonF> getOutline() const;

    void setID(uint32_t);
    void setCentroid(std::shared_ptr<QPoint>);
    void setBoundingBox(std::shared_ptr<QRect>);
    void setOutline(std::shared_ptr<QPolygonF>);

    void setTrackID(const uint32_t &value);

    friend std::ostream& ::operator<< (std::ostream&, Object&);

private:
    uint32_t id;        /*!< The ID of this Object */
    uint32_t trackId = UINT32_MAX;   /*!< The trackId, with wich this Object is associated */
    std::shared_ptr<QPoint> centroid;   /*!< The center of this Object */
    std::shared_ptr<QRect> boundingBox; /*!< The boundingBox of this Object */
    std::shared_ptr<QPolygonF> outline;  /*!< The outline of this Object */

};

}


#endif // OBJECT_H
