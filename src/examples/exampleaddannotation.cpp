#include "examples.h"

#include <memory>

#include "exceptions/ctexception.h"
#include "io/importhdf5.h"

/*!
 * \brief demonstrates how to add an Annotation
 */
void exampleAddAnnotation() {
    std::shared_ptr<CellTracker::Project> proj; /* new project */
    CellTracker::ImportHDF5 ih;
    try {
        proj = ih.load(CURRENT_EXAMPLE);

        int nr = 0;
        std::shared_ptr<CellTracker::Genealogy> g = proj->getGenealogy();
        std::shared_ptr<CellTracker::Tracklet> t = g->getTracklet(nr);
        QString title("Some annotation title");
        QString desc("Some annotation description");
        std::shared_ptr<CellTracker::Annotation> a = std::shared_ptr<CellTracker::Annotation>(new CellTracker::Annotation(CellTracker::Annotation::TRACKLET_ANNOTATION, title, desc));

        proj->getGenealogy()->addAnnotation(a);
        proj->getGenealogy()->annotate(t,a);

        std::shared_ptr<QList<std::shared_ptr<CellTracker::Annotation>>> as = proj->getGenealogy()->getAnnotations();
        for (std::shared_ptr<CellTracker::Annotation> an : *as)
            std::cerr << *an;
    } catch (CellTracker::CTException &e) {
        std::cerr << e.what();
    }

}
