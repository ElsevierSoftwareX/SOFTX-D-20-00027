#include <iostream>
#include "idprovider.h"

#include <QDebug>
namespace CellTracker {

std::set<uint32_t> __attribute__((init_priority(101))) IdProvider::usedTrackletIds = std::set<uint32_t>();
uint32_t IdProvider::minFreeTrackletId = 0;
std::set<uint32_t> __attribute__((init_priority(101))) IdProvider::usedAnnotationIds = std::set<uint32_t>();
uint32_t IdProvider::minFreeAnnotationId = 0;

/*!
 * \brief returns an unused ID for use in a Tracklet
 * \return an unused ID
 */
uint32_t IdProvider::getNewTrackletId()
{
    uint32_t i, ret;

    ret = minFreeTrackletId;
    usedTrackletIds.insert(ret);

    /* find next free number */
    for (i = minFreeTrackletId;; i++) {
        if (usedTrackletIds.find(i) == usedTrackletIds.end()) {
            minFreeTrackletId = i;
            break;
        }
    }

    return ret;
}

/*!
 * \brief gives back the given ID, so it can be reused
 * \param id the ID to give back
 */
void IdProvider::returnTrackletId(uint32_t id)
{
    usedTrackletIds.erase(id);
    if (id < minFreeTrackletId)
        minFreeTrackletId = id;
}

/*!
 * \brief returns an unused ID for use in an Annotation
 * \return an unused ID
 */
uint32_t IdProvider::getNewAnnotationId()
{
    uint32_t i, ret;

    ret = minFreeAnnotationId;
    usedAnnotationIds.insert(ret);

    /* find next free number */
    for (i = minFreeAnnotationId;; i++) {
        if (usedAnnotationIds.find(i) == usedAnnotationIds.end()) {
            minFreeAnnotationId = i;
            break;
        }
    }

    return ret;
}

/*!
 * \brief tries to reserve a given ID
 * \param id the ID to reserve
 * \return true if reserving the ID succeeded, false otherwise
 *
 * \warning This may fail! Check the return value to see if the ID could be reserved
 */
bool IdProvider::claimAnnotationId(uint32_t id)
{
    uint32_t i, ret;

    if (usedAnnotationIds.find(id) == usedAnnotationIds.end()) {
        /* not yet in set */
        ret = id;
        usedAnnotationIds.insert(id);
        if (ret == minFreeAnnotationId) {
            for (i = minFreeAnnotationId;; i++) {
                if (usedAnnotationIds.find(i) == usedAnnotationIds.end()) {
                    minFreeAnnotationId = i;
                    break;
                }
            }
        }
        return true;
    } else {
        return false;
    }
}

/*!
 * \brief gives back the given ID, so it can be reused
 * \param id the ID to give back
 */
void IdProvider::returnAnnotationId(uint32_t id)
{
    usedAnnotationIds.erase(id);
    if (id < minFreeAnnotationId)
        minFreeAnnotationId = id;
}

}
