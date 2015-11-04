#include "export.h"

namespace CellTracker {

Export::Export() { }

Export::~Export() { }

bool Export::save(std::shared_ptr<Project> project)
{
    return save(project, project->getFileName());
}

///*!
// * \brief checks if the project has all required components
// * \return true, if the Project is ok, false if not
// */
//bool Export::checkProject(std::shared_ptr<Project> project)
//{
//    bool hasGenealogy;

//    // Check if all std::shared_ptrs are != null
//    if (!project || !project->getInfo() || !project->getMovie())
//        return false;
    
//    hasGenealogy = (project->getGenealogy() != nullptr);

//    // check project->info
//    {
//        std::shared_ptr<Info> info = project->getInfo();
//        // check inputFiles
//        // check timeOfConversion;
//        // check trackingInfoAlgorithm;
//        // check trackingInfoILastikVersion;
//        // check trackingInfoTimeOfTracking;
//    }

//    // check project->movie
//    {
//        std::shared_ptr<Movie> movie = project->getMovie();
//        // check frames
//        {
//            QHash<uint32_t,std::shared_ptr<Frame> frames = movie->getFrames();
//            // check id
//            // check slicesDim
//            // check slices
//            for (std::shared_ptr<Frame> frame : frames) {
//                QList<std::shared_ptr<Slice>> slices = frame->getSlices();
//                for (std::shared_ptr<Slice> slice : slices) {
//                    // check id
//                    // check frameId
//                    // check slicePos
//                    // check channels
//                    QHash<int,std::shared_ptr<Channel> channels = slice->getChannels();
//                    // check id
//                    // check image
//                    // check objects
//                }
//            }
//        }
//    }
//    // check project->genealogy if it has one
//    if (hasGenealogy) {

//    }

//    return true;
//}

}
