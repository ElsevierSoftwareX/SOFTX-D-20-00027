#include <iostream>
#include <memory>

#include "examples.h"
#include "auto_tracklets/autotracklet.h"
#include "base_data/frame.h"
#include "base_data/object.h"

using namespace CellTracker;

void printObjects(std::vector<std::shared_ptr<Object>> os) {
    for (std::shared_ptr<Object> o: os) {
        std::cout << "Object:            " << o->getId() << std::endl
                  << " frameId:          " << o->getFrameId() << std::endl
                  << " isinAutoTracklet: " << (o->isInAutoTracklet()?"true":"false") << std::endl
                  << " autoId:           " << o->getAutoId() << std::endl
                  << " isInTracklet:     " << (o->isInTracklet()?"true":"false") << std::endl
                  << " trackId:          " << o->getTrackId() << std::endl;
    }
}

void printFrames(std::vector<std::shared_ptr<Frame>> fs) {
    for (std::shared_ptr<Frame> f: fs) {
        std::cout << "Frame: " << f->getID() << std::endl;
    }
}

void exampleObject() {
    uint32_t obj1Id, obj2Id, obj3Id, obj4Id,
            frame1Id, frame2Id, frame3Id;

    obj1Id = 0;
    obj2Id = 1;
    obj3Id = 2;
    obj4Id = 3;
    frame1Id = 0;
    frame2Id = 1;
    frame3Id = 2;

    std::shared_ptr<Frame> f1 = std::shared_ptr<Frame>(new Frame(frame1Id));
    std::shared_ptr<Frame> f2 = std::shared_ptr<Frame>(new Frame(frame2Id));
    std::shared_ptr<Frame> f3 = std::shared_ptr<Frame>(new Frame(frame3Id));

    std::shared_ptr<Object> o1 = std::shared_ptr<Object>(new Object(obj1Id, 0, 0, f1->getID()));
    std::shared_ptr<Object> o2 = std::shared_ptr<Object>(new Object(obj2Id, 0, 0, f2->getID()));
    std::shared_ptr<Object> o3 = std::shared_ptr<Object>(new Object(obj3Id, 0, 0, f2->getID()));
    std::shared_ptr<Object> o4 = std::shared_ptr<Object>(new Object(obj4Id, 0, 0, f3->getID()));

    std::vector<std::shared_ptr<Object>> os = {o1,o2,o3,o4};
    std::vector<std::shared_ptr<Frame>> fs = {f1,f2,f3};

    printObjects(os);
    printFrames(fs);

    std::shared_ptr<AutoTracklet> a1 = std::shared_ptr<AutoTracklet>(new AutoTracklet());
    std::shared_ptr<AutoTracklet> a2 = std::shared_ptr<AutoTracklet>(new AutoTracklet());
    a1->setID(3343);
    a2->setID(3344);

    a1->addComponent(f1,o1);
    a1->addComponent(f2,o2);
    a2->addComponent(f2,o3);

    printObjects(os);
    printFrames(fs);
}
