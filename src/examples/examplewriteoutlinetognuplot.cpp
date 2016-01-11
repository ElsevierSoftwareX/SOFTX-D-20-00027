#include "examples.h"

#include <memory>

#include <QImage>
#include <QImageWriter>

#include "exceptions/ctexception.h"
#include "io/importhdf5.h"

/*!
 * \brief demonstrates how to write out all outlines of objects in
 * Channel 0, Slice 0, Frame 0 of the Movie in a format that
 * is suitable to be fed to GNU Plot, so you can visualize if parsing
 * and rendering the outlines was done correctly
 */
void exampleWriteOutlineToGnuplot () {
    CellTracker::ImportHDF5 ih;
    std::shared_ptr<CellTracker::Project> proj;
    std::shared_ptr<QImage> img;
    try {
        QString in_file = CURRENT_EXAMPLE;
        QString tmp_file = "/tmp/image.png";

        proj = ih.load(in_file);
        img = ih.requestImage(in_file, 0, 0, 0);
        QImageWriter writer (tmp_file);
        writer.write(*img);

        std::cout << "set terminal pngcairo transparent" << std::endl
                  << "set output 'out.png'" << std::endl
                  << "set multiplot" << std::endl
                  << "set size ratio 1" << std::endl
                  << "set autoscale fix" << std::endl
                  << "plot '" << tmp_file.toStdString() << "' binary filetype=png with rgbimage" << std::endl;
        for (std::shared_ptr<CellTracker::Object> o: proj->getMovie()->getFrame(0)->getSlice(0)->getChannel(0)->getObjects().values()) {
            std::shared_ptr<QPolygonF> poly = o->getOutline();
            std::shared_ptr<QRect> rect = o->getBoundingBox();

            int length = poly->size();

            std::cout << "set object polygon from ";
            for (int i = 0; i<length; i++) {
                QPointF p = poly->at(i);
                int x = rect->topLeft().x() - rect->topLeft().y() + p.y();
                int y = rect->topLeft().y() - rect->topLeft().x() + p.x();
                if (i < length-1)
                    std::cout << x << "," << y << " to ";
                else
                    std::cout << x << "," << y << std::endl;
            }
        }
        std::cout << "set yrange [250:0]" << std::endl
                  << "plot [0:250] [0:250] x" << std::endl
                  << "unset multiplot" << std::endl
                  << "set output" << std::endl;

    } catch (CellTracker::CTException &e) {
        std::cout << e.what();
    }
}
