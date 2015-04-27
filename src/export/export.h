#ifndef EXPORT_H
#define EXPORT_H

#include <memory>

#include "project.h"

namespace CellTracker {

class Export
{
public:
    Export();
    virtual bool save(std::shared_ptr<Project>,QString) = 0;
//    bool checkProject(std::shared_ptr<Project>);
};

}

#endif // EXPORT_H
