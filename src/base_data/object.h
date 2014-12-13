#ifndef OBJECT_H
#define OBJECT_H

#include <cstdint>
#include <memory>
#include <QPoint>
#include <QRect>
#include <QPolygon>

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
    ~Object();
    void setID(uint32_t);
    void setTrackID(uint32_t);
    void setCentroid(std::shared_ptr<QPoint>);
    void setBoundingBox(std::shared_ptr<QRect>);
    void setOutline(std::shared_ptr<QPolygonF>);
    friend std::ostream& operator<< (std::ostream&, Object&);

private:
    uint32_t id;        /*!< The ID of this Object */
    uint32_t trackId;   /*!< The trackId, with wich this Object is associated */
    std::shared_ptr<QPoint> centroid;   /*!< The center of this Object */
    std::shared_ptr<QRect> boundingBox; /*!< The boundingBox of this Object */
    std::shared_ptr<QPolygonF> outline;  /*!< The outline of this Object */

};

std::ostream& operator<< (std::ostream&, Object&);

#endif // OBJECT_H
