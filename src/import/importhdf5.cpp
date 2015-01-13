#include "importhdf5.h"

#include <QDebug>

#define PRINT_GNUPLOT_BITMASK 0
#define PRINT_GNUPLOT_POLYGONS 0

#ifndef __FUNCTION_NAME__
        #ifdef WIN32
                #define __FUNCTION_NAME__ __FUNCTION__
        #else
                #define __FUNCTION_NAME__ __func__
        #endif
#endif

ImportHDF5::ImportHDF5()
{
}

std::shared_ptr<Project> ImportHDF5::load(QString fileName)
{
    std::shared_ptr<Project> proj;

    try {
        hid_t file = H5Fopen(fileName.toStdString().c_str(),H5F_ACC_RDONLY,H5P_DEFAULT);

        proj = setupEmptyProject();

        /*! \todo Reorder, current order is due to the unimplemented-Exceptions */
        loadObjects(file,proj);
        loadAnnotations(file,proj);
        loadImages(file,proj);
        loadAutoTracklets(file,proj);
        loadExportedTracks(file,proj);

        H5Fclose(file);
    } catch (H5::FileIException &e) {
        qDebug() << "Opening the file"
                 << fileName
                 << "failed:"
                 << QString(e.getDetailMsg().c_str());
        return nullptr;
    } catch (std::string &e) {
        qDebug() << e.c_str();
        exit(-1);
    }


    return proj;
}

bool ImportHDF5::loadAnnotations(hid_t file, std::shared_ptr<Project> project)
{
    throw std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": function " + std::string(__FUNCTION_NAME__) + " unimplemented";
    return false;
}

bool ImportHDF5::loadImages(hid_t file, std::shared_ptr<Project> project)
{
    throw std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": function " + std::string(__FUNCTION_NAME__) + " unimplemented";
    return false;
}

std::shared_ptr<QPoint> readCentroid(hid_t objGroup) {
    std::shared_ptr<QPoint> point(new QPoint());
    hid_t centroid = H5Dopen (objGroup, "centroid", H5P_DEFAULT);

    hid_t dataspace, memspace;
    int rank = 1;
    const hsize_t dims[] = {2};
    const hsize_t maxdims[] = {2};
    uint16_t buf[2] = {0,0};

    dataspace = H5Dget_space(centroid);
    memspace = H5Screate_simple(rank, dims, maxdims);
    H5Dread(centroid, H5T_NATIVE_UINT16, memspace, dataspace, H5P_DEFAULT, buf);

    H5Sclose(memspace);
    H5Dclose(centroid);

    point->setX(buf[0]);
    point->setY(buf[1]);

    return point;
}

std::shared_ptr<QRect> readBoundingBox(hid_t objGroup) {
    std::shared_ptr<QRect> box (new QRect());
    hid_t bbox = H5Dopen (objGroup, "bbox", H5P_DEFAULT);

    hid_t dataspace, memspace;
    int rank = 2;
    const hsize_t dims[] = {2,2};
    const hsize_t maxdims[] = {2,2};
    uint16_t buf[2][2]  = {{0,0},{0,0}};

    dataspace = H5Dget_space(bbox);
    memspace = H5Screate_simple(rank, dims, maxdims);
    H5Dread(bbox, H5T_NATIVE_UINT16, memspace, dataspace, H5P_DEFAULT, buf);

    H5Sclose(memspace);
    H5Dclose(bbox);

    /*! \todo Check if this is correct */
    box->setCoords(buf[0][0], buf[0][1], buf[1][0], buf[1][1]);

    return box;
}

std::shared_ptr<QPolygonF> bitmaskToPolygon (uint8_t *bitmask, uint32_t length, std::shared_ptr<QRect> bbox, std::shared_ptr<QPoint> centroid) {
    std::shared_ptr<QPolygonF> poly (new QPolygonF());
    QPolygonF temp;
    QPolygonF *merged;
    QPointF p;
    int width = bbox->width();
    int bit, x, y;
    uint8_t field;
    bool set;

    for (unsigned int idx=0; idx<length;++idx) {
        x = idx%width;
        y = idx/width;
        field = bitmask[idx/8];

        bit = 7 - (idx%8);

        set = field & static_cast<uint8_t>(exp2(bit));

#if PRINT_GNUPLOT_BITMASK
        /* Debug output for drawing the read bitmask in gnuplot  */
        std::string bla = "set object rect from "
                + std::to_string(x+centroid->x()) + ","
                + std::to_string(y+centroid->y()) + " to "
                + std::to_string(x+1+centroid->x()) + ","
                + std::to_string(y+1+centroid->y()) + " fc rgb \'"
                + (set?"#303030\'":"#ffffff\'");
        qDebug() << bla.c_str();
#endif
        if (set) {
            temp.clear();

            int x1 = x+centroid->x();
            int y1 = y+centroid->y();
            int x2 = x+1+centroid->x();
            int y2 = y+1+centroid->y();

            QPointF p1(x1,y1), p2(x2,y1), p3(x2,y2), p4(x1,y2);
            temp.append(p1);
            temp.append(p2);
            temp.append(p3);
            temp.append(p4);
            temp.append(p1);

            merged = new QPolygonF(poly->united(temp));
            std::shared_ptr<QPolygonF> mergedP(merged);

            poly = mergedP;
        }
    }

#if PRINT_GNUPLOT_BITMASK
    std::string coords = std::to_string(centroid->x()) + "," + std::to_string(centroid->y());
    std::string bla = "set label \"" + coords + "\" at " + coords + " textcolor rgb '#ff0000'";
    qDebug() << bla.c_str();
#endif

    return poly;
}

std::shared_ptr<QPolygonF> readOutline(hid_t objGroup, std::shared_ptr<QRect> bbox, std::shared_ptr<QPoint> centroid) {
    std::shared_ptr<QPolygonF> poly (new QPolygonF());
    hid_t outline = H5Dopen (objGroup, "packed", H5P_DEFAULT);

    /* rank is always 1 */
    hid_t dataspace, memspace;
    int rank = 1;
    hsize_t *dims = new hsize_t[rank];
    hsize_t *maxdims = new hsize_t[rank];

    dataspace = H5Dget_space(outline);
    H5Sget_simple_extent_dims(dataspace, dims, maxdims);

    uint8_t *buf = new uint8_t[maxdims[0]];

    memspace = H5Screate_simple(rank, dims, maxdims);
    H5Dread(outline, H5T_NATIVE_UINT8, memspace, dataspace, H5P_DEFAULT, buf);

    uint32_t length;
    hid_t len = H5Aopen(outline, "length", H5P_DEFAULT);
    H5Aread(len,H5T_NATIVE_UINT32,&length);
    H5Aclose(len);

    H5Dclose(outline);

    poly = bitmaskToPolygon(buf, length, bbox, centroid);
    delete[] (buf);
    delete[] (dims);
    delete[] (maxdims);

#if PRINT_GNUPLOT_POLYGONS
    /* Debug output for drawing the generated polygons in gnuplot */
    std::string txt,coords;
    coords = std::to_string((int)centroid->x()) + "," + std::to_string((int)centroid->y());
    txt = "set object polygon from ";
    for (QPointF point: *poly)
        txt += std::to_string((int)point.x()) + "," + std::to_string((int)point.y()) + " to ";
    txt = txt.substr(0,txt.size()-3) + "\n";
    txt += "set label \""
            + coords
            + "\" at "
            + coords
            + " textcolor rgb '#ff0000'";
    qDebug() << txt.c_str();
#endif

    return poly;
}

herr_t process_object(hid_t group_id, const char *name, void *op_data) {
    Channel *cptr = static_cast<Channel*>(op_data);

    std::shared_ptr<Object> obj(new Object());
    cptr->addObject(obj);
    int id = atoi(name);
    obj->setID(id);

    hid_t objGroup = H5Gopen (group_id, name, H5P_DEFAULT);

    std::shared_ptr<QPoint> centroid = readCentroid(objGroup);
    obj->setCentroid(centroid);

    std::shared_ptr<QRect> bbox = readBoundingBox(objGroup);
    obj->setBoundingBox(bbox);

    std::shared_ptr<QPolygonF> outline = readOutline(objGroup, bbox, centroid);
    obj->setOutline(outline);

    H5Gclose(objGroup);

    /*! \todo Implement parsing for:
     * - centroid
     * - bbox
     * - packed */

    /*! \todo Set object properties */

    return 0;
}

herr_t process_frame(hid_t group_id, const char *name, void *op_data) {
    H5G_stat_t statbuf;
    Project *pptr = static_cast<Project *> (op_data);

    if (statbuf.type == H5G_GROUP){
        int frameNr = atoi (name);

        if (!pptr->getMovie()->getFrame(frameNr)) {
            /*! \todo Change code, when Slice/Channel are implemented in the
             * HDF5-file */
            std::shared_ptr<Frame> frame (new Frame(frameNr));
            std::shared_ptr<Slice> slice (new Slice(DEFAULT_SLICE));
            std::shared_ptr<Channel> chan (new Channel(DEFAULT_CHANNEL));

            pptr->getMovie()->addFrame(frame);
            frame->addSlice(slice);
            slice->addChannel(chan);
        }

        std::shared_ptr<Channel> chan = pptr->getMovie()->getFrame(frameNr)->getSlice(DEFAULT_SLICE)->getChannel(DEFAULT_CHANNEL);

        qDebug() << "#frame " << name;
        H5Giterate(group_id, name, NULL, process_object, &(*chan));
    }

    return 0;
}

bool ImportHDF5::loadObjects(hid_t file, std::shared_ptr<Project> project) {

    H5Giterate(file, "frames", NULL, process_frame, &(*project));

    return false;
}

bool ImportHDF5::loadAutoTracklets(hid_t file, std::shared_ptr<Project> project)
{
    throw std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": function " + std::string(__FUNCTION_NAME__) + " unimplemented";
    return false;
}
bool ImportHDF5::loadExportedTracks(hid_t file, std::shared_ptr<Project> project)
{
    throw std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": function " + std::string(__FUNCTION_NAME__) + " unimplemented";
    return false;
}

std::shared_ptr<Project> ImportHDF5::setupEmptyProject() {
    std::shared_ptr<Project> project(new Project());
    std::shared_ptr<Movie> movie(new Movie());
    std::shared_ptr<Genealogy> genealogy(new Genealogy());

    project->setMovie(movie);
    project->setGenealogy(genealogy);

    return project;
}
