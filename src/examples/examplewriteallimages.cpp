/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Sebastian Wagner
 *
 * Celltracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Celltracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Celltracker.  If not, see <https://www.gnu.org/licenses/>.
 */
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
