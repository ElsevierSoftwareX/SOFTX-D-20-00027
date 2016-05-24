#ifndef MODIFY_H
#define MODIFY_H

#include <initializer_list>
#include <memory>

#include <base/object.h>

namespace CellTracker {

class Modify {
public:
    Modify() = default;
    ~Modify() = default;

    virtual bool replaceObject(QString file, std::shared_ptr<Object> oldObject, std::initializer_list<std::shared_ptr<Object>> newObjects) = 0;
};
}

#endif // MODIFY_H
