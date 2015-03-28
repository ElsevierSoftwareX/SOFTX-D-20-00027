#ifndef IDPROVIDER_H
#define IDPROVIDER_H

#include <cstdint>
#include <set>


namespace CellTracker {

class IdProvider
{
public:
    static uint32_t getNewAutoTrackletId();
    static void returnAutoTrackletId(uint32_t id);

private:
    static std::set<uint32_t> usedAutoTrackletIds;
    static uint32_t minFreeAutoTrackletId;
};

}

#endif // IDPROVIDER_H
