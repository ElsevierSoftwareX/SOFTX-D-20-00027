#include <iostream>
#include "idprovider.h"

namespace CellTracker {

std::set<uint32_t> IdProvider::usedAutoTrackletIds = std::set<uint32_t>();
uint32_t IdProvider::minFreeAutoTrackletId = 0;

uint32_t IdProvider::getNewAutoTrackletId()
{
    uint32_t i, ret;

    ret = minFreeAutoTrackletId;
    usedAutoTrackletIds.insert(ret);

    /* find next free number */
    for (i = minFreeAutoTrackletId;; i++) {
        if (usedAutoTrackletIds.find(i) == usedAutoTrackletIds.end()) {
            minFreeAutoTrackletId = i;
            break;
        }
    }

    return ret;
}

void IdProvider::returnAutoTrackletId(uint32_t id)
{
    usedAutoTrackletIds.erase(id);
    if (id < minFreeAutoTrackletId)
        minFreeAutoTrackletId = id;
}

}
