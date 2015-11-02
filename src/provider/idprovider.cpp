#include <iostream>
#include "idprovider.h"

#include <QDebug>
namespace CellTracker {

std::set<uint32_t> IdProvider::usedTrackletIds = std::set<uint32_t>();
uint32_t IdProvider::minFreeTrackletId = 0;
std::set<uint32_t> IdProvider::usedAnnotationIds = std::set<uint32_t>();
uint32_t IdProvider::minFreeAnnotationId = 0;

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

void IdProvider::returnTrackletId(uint32_t id)
{
    usedTrackletIds.erase(id);
    if (id < minFreeTrackletId)
        minFreeTrackletId = id;
}

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

bool IdProvider::claimAnnotationId(uint32_t id)
{
    uint32_t i, ret;

    for (uint32_t a : usedAnnotationIds)
                qDebug() << a;
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

void IdProvider::returnAnnotationId(uint32_t id)
{
    usedAnnotationIds.erase(id);
    if (id < minFreeAnnotationId)
        minFreeAnnotationId = id;
}

}
