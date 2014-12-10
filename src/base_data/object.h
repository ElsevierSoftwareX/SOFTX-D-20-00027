#ifndef OBJECT_H
#define OBJECT_H

#include <cstdint>
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
    void setCentroid(QPoint *);
    void setBoundingBox(QRect *);
    void setOutline(QPolygonF *);
    friend std::ostream& operator<< (std::ostream&, Object&);

private:
    uint32_t id;        /*!< The ID of this Object */
    uint32_t trackId;   /*!< The trackId, with wich this Object is associated */
    QPoint* centroid;   /*!< The center of this Object */
    QRect* boundingBox; /*!< The boundingBox of this Object */
    QPolygonF* outline;  /*!< The outline of this Object */

};

std::ostream& operator<< (std::ostream&, Object&);

#endif // OBJECT_H
