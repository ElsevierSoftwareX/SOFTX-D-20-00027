#include "examples.h"

#include <memory>
#include <string>

#include <QImage>
#include <QImageWriter>

#include "io/importhdf5.h"

/*!
 * \brief demonstrates how to write out all Images in the Movie (to validate,
 * that reading of image data was done correctly)
 */
void exampleWriteAllImages() {
    CellTracker::ImportHDF5 ih;
    std::shared_ptr<QImage> img;
    for (int i =0; i<209; i++) {
        img = ih.requestImage(CURRENT_EXAMPLE, i, 0, 0);
        char *cs = new char[200];
        sprintf(cs,"%03d",i+1);
        QImageWriter writer((std::string("/tmp/own_image") + cs + ".png").c_str());
        writer.write(*img);
  }

}
