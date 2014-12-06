#ifndef TRACKLET_H
#define TRACKLET_H

#include "trackelement.h"

/*!
 * \brief The Tracklet class
 *
 * Tracklet is the superclass to the TrackletRegular, TrackletMerged and
 * TrackletSplit classes. The concrete type of the class is specified in type.
 */
class Tracklet : public TrackElement
{
public:
    /*!
     * \brief The TRACKLET_TYPE enum
     *
     * This enum is used to specify the type of a Tracklet.
     */
    enum TRACKLET_TYPE {
        TRACKLET_REGULAR, /*!< Tracklet is of type TrackletRegular */
        TRACKLET_MERGED,  /*!< Tracklet is of type TrackletMerged */
        TRACKLET_SPLIT    /*!< Tracklet is of type TrackletSplit */
    };

    Tracklet();

private:
    TRACKLET_TYPE type;
};

#endif // TRACKLET_H
