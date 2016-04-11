#ifndef EXPORT_H
#define EXPORT_H

#include <memory>

#include "project.h"

namespace CellTracker {

/*!
 * \brief The Export class
 *
 * Superclass for Classes that save a given Project to a resource that can be
 * identified via a QString.
 *
 * Also provides methods to save to same file that the Project was loaded from.
 */
class Export
{
public:
    Export() = default;
    virtual ~Export() = 0;

    virtual bool save(std::shared_ptr<Project>,QString) = 0;
    bool save(std::shared_ptr<Project>);
};

}

#endif // EXPORT_H
