#ifndef TRACKELEMENT_H
#define TRACKELEMENT_H

namespace CellTracker {

/*!
 * \brief The TrackElement class
 *
 * A TrackElement may either be of type Tracklet or of type TrackEvent. Both
 * should alternate in the tree. The concrete type should be specified using
 * the ELEMENT_TYPE enum.
 */
class TrackElement
{
public:
    /*!
     * \brief The ELEMENT_TYPE enum
     *
     * This enum is used to specify the type of a TrackElement.
     */
    enum ELEMENT_TYPE {
        ELEMENT_TRACKLET, /*!< TrackElement is of type Tracklet */
        ELEMENT_EVENT     /*!< TrackElement is of type TrackEvent */
    };

    TrackElement();
    TrackElement(ELEMENT_TYPE);

private:
    ELEMENT_TYPE elemType;
};

}

#endif // TRACKELEMENT_H
