#include <iostream>
#include <memory>
#include <vector>

#include "examples.h"
#include "provider/idprovider.h"
#include "tracked/tracklet.h"

/*!
 * \brief demonstrates the IDProvider and that redistributen of returned IDs works
 */
void exampleIdProvider() {
    std::cout << CellTracker::IdProvider::getNewTrackletId() << std::endl; // 0
    std::cout << CellTracker::IdProvider::getNewTrackletId() << std::endl; // 1
    std::cout << CellTracker::IdProvider::getNewTrackletId() << std::endl; // 2
    std::cout << CellTracker::IdProvider::getNewTrackletId() << std::endl; // 3
    std::cout << CellTracker::IdProvider::getNewTrackletId() << std::endl; // 4

    CellTracker::IdProvider::returnTrackletId(1);
    CellTracker::IdProvider::returnTrackletId(3);

    std::cout << CellTracker::IdProvider::getNewTrackletId() << std::endl; // 1
    std::cout << CellTracker::IdProvider::getNewTrackletId() << std::endl; // 3

    for (uint32_t i = 0; i < 5; i++)
        CellTracker::IdProvider::returnTrackletId(i);

    std::cout << "Testing speed" << std::endl;
    std::cout << "inserting numbers" << std::endl;
    for (uint32_t i = 0; i < 100000; i++)
        CellTracker::IdProvider::getNewTrackletId();

    std::cout << "deleting numbers" << std::endl;
    for (uint32_t i = 100000; i > 0; i--)
        CellTracker::IdProvider::returnTrackletId(i-1);

    std::cout << CellTracker::IdProvider::getNewTrackletId() << std::endl; // 1

    auto t1 = std::make_shared<CellTracker::Tracklet>();
    auto t2 = std::make_shared<CellTracker::Tracklet>();
    auto t3 = std::make_shared<CellTracker::Tracklet>();
    auto t4 = std::make_shared<CellTracker::Tracklet>();
    auto t5 = std::make_shared<CellTracker::Tracklet>();

    std::vector<std::shared_ptr<CellTracker::Tracklet>> ts = {t1,t2,t3,t4,t5};
    for (std::shared_ptr<CellTracker::Tracklet> t: ts)
        std::cout << t->getId() << std::endl;
    ts.clear();

    t2.reset();
    t4.reset();

    auto t6 = std::make_shared<CellTracker::Tracklet>();
    auto t7 = std::make_shared<CellTracker::Tracklet>();

    ts = {t1,t3,t5,t6,t7};
    for (std::shared_ptr<CellTracker::Tracklet> t: ts)
        std::cout << t->getId() << std::endl;
}
