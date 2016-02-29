#ifndef IMPORT_H
#define IMPORT_H

#include <memory>

#include <QString>

#include "project.h"

namespace CellTracker {

#define DEFAULT_CHANNEL 0
#define DEFAULT_SLICE 0

/*!
 * \brief The Import class
 *
 * Superclass for Import-Classes that load a Project from a certain resource
 * that can be identified by a QString.
 *
 * Also provides a method to set up an empty Project.
 */
class Import
{
public:
    Import();
    virtual ~Import() = 0;

    virtual std::shared_ptr<Project> load(QString) = 0;

protected:
    std::shared_ptr<Project> setupEmptyProject();
};

}

#endif // IMPORT_H
