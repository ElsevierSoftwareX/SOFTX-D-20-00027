#ifndef EXPORT_H
#define EXPORT_H

#include <memory>

#include "project.h"

namespace CellTracker {

class Export
{
public:
    Export();
    virtual ~Export() = 0;

    virtual bool save(std::shared_ptr<Project>,QString) = 0;
    bool save(std::shared_ptr<Project>);
//    bool checkProject(std::shared_ptr<Project>);
};

}

#endif // EXPORT_H
