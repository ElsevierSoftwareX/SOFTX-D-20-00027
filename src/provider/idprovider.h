#ifndef IDPROVIDER_H
#define IDPROVIDER_H

#include <cstdint>
#include <set>

namespace CellTracker {

class IdProvider
{
public:
    static uint32_t getNewTrackletId();
    static void returnTrackletId(uint32_t id);

    static uint32_t getNewAnnotationId();
    static bool claimAnnotationId(uint32_t id);
    static void returnAnnotationId(uint32_t id);

private:
    static std::set<uint32_t> usedTrackletIds;
    static uint32_t minFreeTrackletId;
    static std::set<uint32_t> usedAnnotationIds;
    static uint32_t minFreeAnnotationId;
};

}

#endif // IDPROVIDER_H
