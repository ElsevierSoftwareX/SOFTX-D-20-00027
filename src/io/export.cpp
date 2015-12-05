#include "export.h"

namespace CellTracker {

Export::Export() { }

Export::~Export() { }

bool Export::save(std::shared_ptr<Project> project)
{
    return save(project, project->getFileName());
}

}
