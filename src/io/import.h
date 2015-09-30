#ifndef IMPORT_H
#define IMPORT_H

#include <memory>

#include <QString>

#include "project.h"

namespace CellTracker {

#define DEFAULT_CHANNEL 0
#define DEFAULT_SLICE 0

class Import
{
public:
    Import();
    virtual std::shared_ptr<Project> load(QString) = 0;

protected:
    std::shared_ptr<Project> setupEmptyProject();
};

}

#endif // IMPORT_H
