#include <iostream>
#include <memory>
#include <vector>

#include "examples.h"
#include "corrected_data/idprovider.h"
#include "corrected_data/tracklet.h"

void exampleIdProvider() {
    std::cout << CellTracker::IdProvider::getNewAutoTrackletId() << std::endl; // 0
    std::cout << CellTracker::IdProvider::getNewAutoTrackletId() << std::endl; // 1
    std::cout << CellTracker::IdProvider::getNewAutoTrackletId() << std::endl; // 2
    std::cout << CellTracker::IdProvider::getNewAutoTrackletId() << std::endl; // 3
    std::cout << CellTracker::IdProvider::getNewAutoTrackletId() << std::endl; // 4

    CellTracker::IdProvider::returnAutoTrackletId(1);
    CellTracker::IdProvider::returnAutoTrackletId(3);

    std::cout << CellTracker::IdProvider::getNewAutoTrackletId() << std::endl; // 1
    std::cout << CellTracker::IdProvider::getNewAutoTrackletId() << std::endl; // 3

    for (uint32_t i = 0; i < 5; i++)
        CellTracker::IdProvider::returnAutoTrackletId(i);

    std::cout << "Testing speed" << std::endl;
    std::cout << "inserting numbers" << std::endl;
    for (uint32_t i = 0; i < 100000; i++)
        CellTracker::IdProvider::getNewAutoTrackletId();

    std::cout << "deleting numbers" << std::endl;
    for (uint32_t i = 100000; i > 0; i--)
        CellTracker::IdProvider::returnAutoTrackletId(i-1);

    std::cout << CellTracker::IdProvider::getNewAutoTrackletId() << std::endl; // 1

    std::shared_ptr<CellTracker::Tracklet> t1 = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::Tracklet());
    std::shared_ptr<CellTracker::Tracklet> t2 = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::Tracklet());
    std::shared_ptr<CellTracker::Tracklet> t3 = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::Tracklet());
    std::shared_ptr<CellTracker::Tracklet> t4 = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::Tracklet());
    std::shared_ptr<CellTracker::Tracklet> t5 = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::Tracklet());

    std::vector<std::shared_ptr<CellTracker::Tracklet>> ts = {t1,t2,t3,t4,t5};
    for (std::shared_ptr<CellTracker::Tracklet> t: ts)
        std::cout << t->getID() << std::endl;
    ts.clear();

    t2.reset();
    t4.reset();

    std::shared_ptr<CellTracker::Tracklet> t6 = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::Tracklet());
    std::shared_ptr<CellTracker::Tracklet> t7 = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::Tracklet());

    ts = {t1,t3,t5,t6,t7};
    for (std::shared_ptr<CellTracker::Tracklet> t: ts)
        std::cout << t->getID() << std::endl;
}
