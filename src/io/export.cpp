#include "export.h"

namespace CellTracker {

Export::Export() {}

Export::~Export() {}

/*!
 * \brief function for saving a Project that already has a file to the same file
 * \param project the Project to save
 * \return true if saving was successful
 */
bool Export::save(std::shared_ptr<Project> project)
{
    return save(project, project->getFileName());
}

}
