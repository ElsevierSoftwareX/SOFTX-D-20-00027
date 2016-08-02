#include "importxml.h"

#if 0
#include <cstdint>
#include <iostream>

#include <QDebug>
#include <QDirIterator>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QImage>
#include <QIODevice>
#include <QList>
#include <QPoint>
#include <QPointF>
#include <QPolygonF>
#include <QRect>

#include "base/movie.h"
#include "tracked/genealogy.h"
#include "tracked/trackeventdead.h"
#include "tracked/trackeventdivision.h"
#include "tracked/trackeventlost.h"
#include "tracked/trackeventmerge.h"
#include "tracked/trackeventunmerge.h"
#endif

#include <memory>

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QDomDocument>
#include <QImage>

#include "exceptions/ctimportexception.h"
#include "provider/messagerelay.h"

namespace CellTracker {

std::shared_ptr<Project> ImportXML::load(QString filePath) {
    std::shared_ptr<Project> proj = Import::setupEmptyProject();
    proj->setFileName(filePath);

    MessageRelay::emitUpdateOverallName("Importing from XML");
    MessageRelay::emitUpdateOverallMax(4);

    /* setup frames/slices/channels. as the XML-format does not support Slices/Channels,
     *  this can be done in one step */
    bool ret = loadFrames(filePath, proj);
    if (!ret)
        throw CTImportException("loading of frames failed");
    MessageRelay::emitIncreaseOverall();

    ret = loadInfo(filePath, proj);
    if (!ret)
        throw CTImportException("loading of info failed");
    MessageRelay::emitIncreaseOverall();

    ret = loadObjects(filePath, proj);
    if (!ret)
        throw CTImportException("loading of objects failed");
    MessageRelay::emitIncreaseOverall();

    ret = loadAutoTracklets(filePath, proj);
    if (!ret)
        throw CTImportException("loading of autotracklets failed");
    MessageRelay::emitIncreaseOverall();

    return proj;
}

bool ImportXML::loadInfo(QString filePath, std::shared_ptr<Project> const &proj) {
    using CSI = Project::CoordinateSystemInfo;
    using CSD = CSI::CoordinateSystemData;

    /* load a test image */
    QDir imgDir(filePath);
    if (!imgDir.exists() || !imgDir.isReadable())
        throw CTImportException("The root directory of the XML project does not exist or is not readable");
    imgDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    imgDir.cd("images");
    if (!imgDir.exists() || !imgDir.isReadable())
        throw CTImportException("The image directory of the XML project does not exist or is not readable");
    imgDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);

    QDirIterator dit(imgDir, QDirIterator::NoIteratorFlags);
    QString firstFile = dit.next();

    QImage qi(firstFile);
    uint32_t height = qi.height();
    uint32_t width = qi.width();

    auto csi = std::make_shared<CSI>();
    CSD csd {height, width};
    csi->setCoordinateSystemType(CSI::CST_CARTESIAN);
    csi->setCoordinateSystemData(csd);
    proj->setCoordinateSystemInfo(csi);

    return true;
}

bool ImportXML::loadFrames(QString filePath, std::shared_ptr<Project> const &proj) {
    QDir imgDir(filePath);
    if (!imgDir.exists() || !imgDir.isReadable())
        throw CTImportException("The root directory of the XML project does not exist or is not readable");
    imgDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    imgDir.cd("images");
    if (!imgDir.exists() || !imgDir.isReadable())
        throw CTImportException("The image directory of the XML project does not exist or is not readable");
    imgDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);

    MessageRelay::emitUpdateDetailName("Loading frames");
    MessageRelay::emitUpdateDetailMax(imgDir.count());

    std::shared_ptr<Movie> mov = proj->getMovie();
    QDirIterator dit(imgDir, QDirIterator::NoIteratorFlags);

    for (int frameNr = 0; dit.hasNext(); frameNr++) {
        auto frame = std::make_shared<Frame>(frameNr);
        mov->addFrame(frame);
        auto slice = std::make_shared<Slice>(0, frameNr);
        frame->addSlice(slice);
        auto chan  = std::make_shared<Channel>(0, 0, frameNr);
        slice->addChannel(chan);

        dit.next();
        MessageRelay::emitIncreaseDetail();
    }

    return true;
}

bool ImportXML::loadObjects(QString filePath, std::shared_ptr<Project> const &proj) {
    QDir qd(filePath);
    if (!qd.exists() || !qd.isReadable())
        throw CTImportException("The root directory of the XML project does not exist or is not readable");
    qd.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    qd.cd("xml");
    if (!qd.exists())
        throw CTImportException("The xml directory of the XML project does not exist");
    qd.setFilter(QDir::Files | QDir::NoDotAndDotDot);

    MessageRelay::emitUpdateDetailName("Loading Objects");
    MessageRelay::emitUpdateDetailMax(qd.count());

    std::shared_ptr<Movie> mov = proj->getMovie();
    QDirIterator dit(qd, QDirIterator::NoIteratorFlags);

    int frameNr;
    QString currFile;
    for (frameNr = 0; dit.hasNext(); frameNr++) {
        currFile = dit.next();
        std::shared_ptr<Channel> chan = mov->getFrame(frameNr)->getSlice(0)->getChannel(0);

        if (!loadObjectsInFrame(currFile, chan))
            return false;

        MessageRelay::emitIncreaseDetail();
    }
    return true;
}

bool ImportXML::loadObjectsInFrame(QString fileName, std::shared_ptr<Channel> &chan) {
    using QDE = QDomElement;
    QFile file(fileName);
    QDomDocument dom;
    dom.setContent(&file, true, nullptr, nullptr, nullptr);

    uint32_t frameId = chan->getFrameId();
    std::string frameName = "Frame_";
    frameName += std::to_string(frameId + 1);

    QDE frame = dom.firstChildElement(frameName.c_str());
    if (frame.isNull()) {
        throw CTImportException(frameName + " not found");
    }

    for (QDE obj = frame.firstChildElement("Object"); !obj.isNull(); obj = obj.nextSiblingElement("Object")) {
        QDE objId = obj.firstChildElement("ObjectID").firstChildElement("value");
        QDE objCntr = obj.firstChildElement("ObjectCenter").firstChildElement("point");
        QDE objBB = obj.firstChildElement("ObjectBoundingBox");
        QDE objOutline = obj.firstChildElement("Outline");

        unsigned id = objId.text().toUInt() - 1; /* Object IDs 1-based in XML format */
        qreal cntrX = objCntr.firstChildElement("x").text().toDouble();
        qreal cntrY = objCntr.firstChildElement("y").text().toDouble();
        QDE objBB1 = objBB.firstChildElement("point");
        QDE objBB2 = objBB1.nextSiblingElement("point");
        qreal objBB1X = objBB1.firstChildElement("x").text().toDouble();
        qreal objBB1Y = objBB1.firstChildElement("y").text().toDouble();
        qreal objBB2X = objBB2.firstChildElement("x").text().toDouble();
        qreal objBB2Y = objBB2.firstChildElement("y").text().toDouble();

        /*! \todo increase precision in Object? QPoint/QRect could be float */
        std::shared_ptr<Object> o = std::make_shared<Object>(id, chan);
        std::shared_ptr<QPoint> cntr = std::make_shared<QPoint>(cntrX, cntrY);
        std::shared_ptr<QRect> bb = std::make_shared<QRect>(QPoint(objBB1X, objBB1Y), QPoint(objBB2X, objBB2Y));
        std::shared_ptr<QPolygonF> outline = loadObjectOutline(objOutline);

        o->setCentroid(cntr);
        o->setBoundingBox(bb);
        o->setOutline(outline);
        chan->addObject(o);
    }

    return true;
}

std::shared_ptr<QPolygonF> ImportXML::loadObjectOutline(QDomElement &objElem) {
    using QDE = QDomElement;

    std::shared_ptr<QPolygonF> outline = std::make_shared<QPolygonF>();

    for(QDE elem = objElem.firstChildElement("point"); !elem.isNull(); elem = elem.nextSiblingElement("point")) {
        qreal x = elem.firstChildElement("x").text().toDouble();
        qreal y = elem.firstChildElement("y").text().toDouble();

        outline->push_back(QPointF(x,y));
    }

    /* close the polygon */
    outline->push_back(outline->first());
    return outline;
}

std::shared_ptr<QImage> ImportXML::requestImage(QString filePath, int frame, int slice, int channel) {
    if (slice != 0 || channel != 0)
        throw CTImportException("The XML-Format does not support Slices and Channels");

    /* frame n is the n-th file in th images directory */
    QDir imgDir(filePath);
    if (!imgDir.exists() || !imgDir.isReadable())
        throw CTImportException("The root directory of the XML project does not exist or is not readable");
    imgDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    imgDir.cd("images");
    if (!imgDir.exists() || !imgDir.isReadable())
        throw CTImportException("The image directory of the XML project does not exist or is not readable");
    imgDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);

    QDirIterator dit(imgDir, QDirIterator::NoIteratorFlags);
    QString fileName = dit.next();

    /*! \todo: ugly, find a way to directly access the n-th item */
    int i;
    for (i = 0; i < frame && dit.hasNext(); i++)
        fileName = dit.next();

    if (i != frame) /* means we aborted earlier because dit did not have a next */
        throw CTImportException("The image file for frame " + std::to_string(frame) + " does not exist");

    QFile imageFile(fileName);
    if (!imageFile.exists())
        throw CTImportException("The image file for frame " + std::to_string(frame) + " does not exist");

    auto qI = std::make_shared<QImage>(fileName);
    if (qI->isNull())
        throw CTImportException("The image was invalid");

    return qI;
}

bool ImportXML::loadAutoTracklets(QString filePath, std::shared_ptr<Project> const &proj) {
    using QDE = QDomElement;

    QDir qd(filePath);
    std::shared_ptr<Genealogy> gen = proj->getGenealogy();
    std::shared_ptr<Movie> mov = proj->getMovie();

    if (!qd.exists())
        throw CTImportException("The root directory of the XML project does not exist or is not readable");

    QString fileName = qd.filePath("tracksXML.xml");
    QFile tracksFile(fileName);

    if (!tracksFile.exists())
        throw CTImportException("The tracksXML.xml file of the XML project does not exist");

    QDomDocument dom;
    dom.setContent(&tracksFile, true, nullptr, nullptr, nullptr);
    QDE root = dom.documentElement();

    /*! \todo: This could reduce performance on large datasets depending on how it is implemented in QT, maybe also use it */
    QDomNodeList qdnl = root.elementsByTagName("Track");
    MessageRelay::emitUpdateDetailName("Loading AutoTracklets");
    MessageRelay::emitUpdateDetailMax(qdnl.count());

    for(QDE trackElem = root.firstChildElement("Track"); !trackElem.isNull(); trackElem = trackElem.nextSiblingElement("Track")) {
        QDE trackID = trackElem.firstChildElement("TrackID");
        unsigned tid = trackID.text().toUInt() - 1;

        std::shared_ptr<AutoTracklet> at = std::make_shared<AutoTracklet>();
        at->setID(tid);

        for (QDE objElem = trackElem.firstChildElement("object"); !objElem.isNull(); objElem = objElem.nextSiblingElement("object")) {
            QDE objID = objElem.firstChildElement("ObjectID");
            QDE frameID = objElem.firstChildElement("Time");

            unsigned oid = objID.text().toUInt() - 1; /* Object IDs 1-based in XML format */
            unsigned fid = frameID.text().toUInt() - 1; /* Frame IDs 1-based in XML format */

            std::shared_ptr<Frame> frame = mov->getFrame(fid);
            std::shared_ptr<Object> obj = frame->getSlice(0)->getChannel(0)->getObject(oid);

            if (!frame)
                throw CTImportException("Did not find frame");
            if (!obj)
                throw CTImportException("Did not find object");

            at->addComponent(frame, obj);
        }
        proj->addAutoTracklet(at);
        MessageRelay::emitIncreaseDetail();
    }
    return true;
}

#if 0
/*!
 * \warning DO NOT USE THIS.
 */
std::shared_ptr<Project> ImportXML::load(QString dir)
{
    std::shared_ptr<Project> project;
    qDebug() << "ImportXML::load(" << dir << ")";

    project = setupEmptyProject();

    QDir qd(dir);
    if (!qd.exists() || !qd.isReadable()){
        qDebug() << "Directory " << dir << " does not exists or is not readable.";
        goto err;
    }

    if (!qd.exists("images")){
        qDebug() << "The directory images does not exist.";
        goto err;
    } else if (!loadImages(qd,project)) {
        qDebug() << "Loading from the directory images failed.";
        goto err;
    }

    if (!qd.exists("xml")){
        qDebug() << "The directory xml does not exist.";
        goto err;
    } else if (!loadObjects(qd,project)) {
        qDebug() << "Loading from the directory xml failed.";
        goto err;
    }

    if (!qd.exists("tracksXML.xml")) {
        qDebug() << "File tracksXML.xml does not exist.";
        goto err;
    } else if (!loadAutoTracklets(qd,project)) {
        qDebug() << "Loading the file tracksXML.xml failed";
        goto err;
    }

    if (!qd.exists("NewTracks_export.xml")){
        qDebug() << "File NewTracks_export.xml does not exist.";
    } else if (!loadExportedTracks(qd,project)) {
        qDebug() << "Loading the file NewTracks_export.xml failed";
    }

    qDebug() << "Loading was successfull";
    return project;
err:
    qDebug() << "Loading has failed.";
    return nullptr;
}

bool ImportXML::loadImages(const QDir qd, std::shared_ptr<Project> project)
{
    QString fpDirImages;
    std::unique_ptr<QDir> dirImages;
    std::shared_ptr<Movie> movie = project->getMovie();

    fpDirImages = qd.filePath("images");

    dirImages = std::unique_ptr<QDir>(new QDir(fpDirImages));
    dirImages->setFilter(QDir::Files|QDir::NoDotAndDotDot);
    if (!dirImages->isReadable()){
        qDebug() << "Directory images is not readable";
        return false;
    }

    /* Read all the images in dirImages */
    qDebug() << "Reading folder img";
    QDirIterator imgit(*dirImages, QDirIterator::NoIteratorFlags);
    uint32_t id = 0;
    while (imgit.hasNext()){
        QString name = imgit.next();
        auto frame   = std::make_shared<Frame>(id);
        auto slice   = std::make_shared<Slice>(DEFAULT_SLICE,id);
        auto channel = std::make_shared<Channel>(DEFAULT_CHANNEL,DEFAULT_SLICE,id);
        auto image   = std::make_shared<QImage>(name);

        movie->addFrame(frame);
        frame->addSlice(slice);
        frame->setID(id);
        slice->addChannel(channel);
        channel->setImage(image);

        id++;
    }

    return true;
}

bool ImportXML::loadObjects(const QDir qd, std::shared_ptr<Project> project)
{
    QString fpDirXML;
    std::unique_ptr<QDir> dirXML;
    std::shared_ptr<Movie> movie = project->getMovie();

    fpDirXML = qd.filePath("xml");

    dirXML = std::unique_ptr<QDir>(new QDir(fpDirXML));
    dirXML->setFilter(QDir::Files|QDir::NoDotAndDotDot);
    if (!dirXML->isReadable()){
        qDebug() << "Directory xml is not readable";
        return false;
    }

    /* Parse all the files in the dirXML directory */
    qDebug() << "Reading folder xml";
    QDirIterator xmlit(*dirXML, QDirIterator::NoIteratorFlags);
    uint32_t frameID = 0;
    while (xmlit.hasNext()){
        QString name = xmlit.next();
        QFile file(name);
        QDomDocument doc;

        doc.setContent(&file,true,nullptr,nullptr,nullptr);
        QDomElement root = doc.documentElement();
        QDomElement c1, c2, c3; /* XML elements on different levels of the document */

        c1 = root.firstChildElement("Object");
        while (!c1.isNull()){
            uint32_t objID = UINT32_MAX;
            uint32_t trackID = UINT32_MAX;
            auto object   = std::make_shared<Object>();
            auto centroid = std::make_shared<QPoint>();
            auto bBox     = std::make_shared<QRect>();
            auto outline  = std::make_shared<QPolygonF>();

            c2 = c1.firstChildElement("ObjectID");
            if (!c2.isNull())
                objID = c2.firstChildElement("value").text().toInt()-1;

            c2 = c1.firstChildElement("ObjectCenter");
            c3 = c2.firstChildElement("point");
            if (!c2.isNull() || !c3.isNull()){
                centroid->setX(c3.firstChildElement("x").text().toDouble());
                centroid->setY(c3.firstChildElement("y").text().toDouble());
            }

            c2 = c1.firstChildElement("ObjectBoundoingBox");
            c3 = c2.firstChildElement("point");
            if (!c2.isNull() || !c3.isNull()){
                bBox->setLeft(c3.firstChildElement("x").text().toInt());
                bBox->setTop(c3.firstChildElement("y").text().toInt());
            }
            c3 = c3.nextSiblingElement("point");
            if (!c2.isNull() || !c3.isNull()){
                bBox->setRight(c3.firstChildElement("x").text().toInt());
                bBox->setBottom(c3.firstChildElement("y").text().toInt());
            }

            c2 = c1.firstChildElement("Outline");
            c3 = c2.firstChildElement("point");
            while ((!c2.isNull()) && (!c3.isNull())) {
                qreal x = qreal(c3.firstChildElement("x").text().toFloat());
                qreal y = qreal(c3.firstChildElement("y").text().toFloat());
                qreal off_x = bBox->left();
                qreal off_y = bBox->right();
                *outline << QPointF(x-off_x,y-off_y);
                c3 = c3.nextSiblingElement("point");
            }

            c2 = c1.firstChildElement("TrackID");
            if (!c2.isNull()){
                trackID = (c2.firstChildElement("value").text().toInt())-1;
            }

            object->setId(objID);
            object->setTrackId(trackID);
            object->setCentroid(centroid);
            object->setBoundingBox(bBox);
            object->setOutline(outline);

            std::shared_ptr<Frame> f = movie->getFrame(frameID);

            /* The old model isn't designed to have slices, so we use the
                 * default slice DEFAULT_SLICE */
            std::shared_ptr<Slice> s = f->getSlice(DEFAULT_SLICE);

            /* Channels also aren't considered, so we use DEFAULT_CHANNEL */
            std::shared_ptr<Channel> c = s->getChannel(DEFAULT_CHANNEL);

            if (c->getObject(objID) == nullptr)
                c->addObject(object);

            c1 = c1.nextSiblingElement("Object");
        }

        frameID++;
    }
    return true;
}

bool ImportXML::loadAutoTracklets(const QDir qd, std::shared_ptr<Project> project)
{
    std::unique_ptr<QFile> fileXML;
    std::shared_ptr<Movie> movie;
    QString fpFileXML;

    movie = project->getMovie();

    fpFileXML = qd.filePath("tracksXML.xml");
    fileXML = std::unique_ptr<QFile>(new QFile(fpFileXML));
    if (!fileXML->open(QIODevice::ReadOnly)){
        qDebug() << "Opening the file tracksXML.xml failed";
        return false;
    }

    /* Parse tracksXML.xml */
    qDebug() << "Reading file tracksXML.xml";
    QDomDocument doc;

    doc.setContent(fileXML.get(),true,nullptr,nullptr,nullptr);

    QDomElement root = doc.documentElement();
    QDomElement trackElement = root.firstChildElement("Track");
    QDomElement objectElement = trackElement.firstChildElement("object");
    QDomElement objectIDElement = objectElement.firstChildElement("ObjectID");
    QDomElement timeElement = objectElement.firstChildElement("Time");

    while (!trackElement.isNull()) {
        int trackID, cellID, time;
        auto tracklet = std::make_shared<AutoTracklet>();

        objectElement = trackElement.firstChildElement("object");
        objectIDElement = objectElement.firstChildElement("ObjectID");
        timeElement = objectElement.firstChildElement("Time");

        trackID = (trackElement.firstChildElement("TrackID").text().toInt())-1;

        tracklet->setID(trackID);

        while (!objectElement.isNull()){
            if (!objectIDElement.isNull() || !timeElement.isNull()){
                cellID = (objectElement.firstChildElement("ObjectID").text().toInt())-1;
                time = (objectElement.firstChildElement("Time").text().toInt())-1;

                std::shared_ptr<Frame> frame = movie->getFrame(time);
                /* get the default channel/slice */
                std::shared_ptr<Channel> chan = frame
                        ->getSlice(DEFAULT_SLICE)
                        ->getChannel(DEFAULT_CHANNEL);
                std::shared_ptr<Object> obj = chan
                        ->getObject(cellID);

                tracklet->addComponent(frame,obj);

                objectElement = objectElement.nextSiblingElement("object");
            }
        }

        /* collect the generated auto_tracklet in project */
        project->addAutoTracklet(tracklet);

        trackElement = trackElement.nextSiblingElement("Track");
    }

    fileXML->close();
    return true;
}

bool ImportXML::loadExportedTracks(const QDir qd, std::shared_ptr<Project> project)
{
    std::unique_ptr<QFile> fileExport;
    std::shared_ptr<Movie> movie = project->getMovie();
    std::shared_ptr<Genealogy> genealogy = project->getGenealogy();
    QString fpFileExport;

    fpFileExport = qd.filePath("NewTracks_export.xml");
    fileExport = std::unique_ptr<QFile>(new QFile(fpFileExport));
    if (!fileExport->open(QIODevice::ReadOnly)){
        qDebug() << "OPening the file NewTracks_export.xml failed.";
        return false;
    }

    /* Import tracks if they exist */
    enum STATUS {
        OPEN = 0, /* open */
        DIVI = 1, /* cell division */
        DEAD = 2, /* dead */
        LOST = 3, /* lost */
        ENDM = 4, /* end of movie */
        COLO = 5  /* colony */
    };

    qDebug() << "Reading file NewTracks_export.xml";
    QDomDocument doc;

    doc.setContent(fileExport.get(),true,nullptr,nullptr,nullptr);

    QDomElement root = doc.documentElement();
    QDomElement trackElement = root.firstChildElement("Track");

    while (!trackElement.isNull()) {
        int id;
        auto tracklet = std::make_shared<Tracklet>();

        QDomElement idElement = trackElement.firstChildElement("TrackID");
        QDomElement objectElement = trackElement.firstChildElement("object");

        id = idElement.text().toInt();
        tracklet->setId(id);

        /* find the autotracklet belonging to the first object */
        while (!objectElement.isNull()){
            int time = objectElement.firstChildElement("Time").text().toInt()-1;
            int oID = objectElement.firstChildElement("ObjectID").text().toInt()-1;

            if (oID == -2){ /* was -1 but we substacted 1 above (this should be the case when in some frame the current object is not recognized) */
                objectElement = objectElement.nextSiblingElement("object");
                continue;
            }

            std::shared_ptr<Frame> frame = movie->getFrame(time);
            std::shared_ptr<Object> object = frame
                    ->getSlice(DEFAULT_SLICE)
                    ->getChannel(DEFAULT_CHANNEL)
                    ->getObject(oID);

            tracklet->addToContained(frame,object);

            objectElement = objectElement.nextSiblingElement("object");
        }

        genealogy->addTracklet(tracklet);

        trackElement = trackElement.nextSiblingElement("Track");
    }

    /* reset trackElement */
    trackElement = root.firstChildElement("Track");

    /* Fix all the TrackEvents (their next/previous member don't have
         * values yet) */
    while (!trackElement.isNull()){
        /* append trackevent corresponding to status */
        int s, motherID, trackID;
        auto daughters = std::make_shared<QList<std::weak_ptr<Tracklet>>>();
        STATUS status;

        QDomElement statusElement = trackElement.firstChildElement("Status");
        QDomElement daugtherElement = trackElement.firstChildElement("Daugters");
        QDomElement daugtherIDElement = daugtherElement.firstChildElement("DaugterID");
        QDomElement motherElement = trackElement.firstChildElement("MotherID");
        QDomElement idElement = trackElement.firstChildElement("TrackID");

        trackID = idElement.text().toInt();
        std::shared_ptr<Tracklet> tracklet = genealogy->getTracklet(trackID);

        s = statusElement.text().toInt();
        if (s >= 0 && s <= 5){
            status = static_cast<STATUS>(s);
        } else {
            qDebug() << "Status is not between 0 and 5.";
            return false;
        }

        motherID = motherElement.text().toInt();

        while (!daugtherIDElement.isNull()) {
            int daughterID = daugtherIDElement.text().toInt();
            daughters->append(genealogy->getTracklet(daughterID));
            daugtherIDElement = daugtherIDElement.nextSiblingElement("DaugterID");
        }

        switch (status) {
        case DIVI: {
            auto eventDivision = std::make_shared<TrackEventDivision<Tracklet>>();
            eventDivision->setPrev(genealogy->getTracklet(motherID));
            eventDivision->setNext(daughters);
            tracklet->setNext(eventDivision); }
            break;
        case DEAD: {
            auto eventDead = std::make_shared<TrackEventDead<Tracklet>>();
            eventDead->setPrev(genealogy->getTracklet(motherID));
            tracklet->setNext(eventDead); }
            break;
        case LOST: {
            auto eventLost = std::make_shared<TrackEventLost<Tracklet>>();
            eventLost->setPrev(genealogy->getTracklet(motherID));
            tracklet->setNext(eventLost); }
            break;
        case OPEN: /* fall-through */
        case ENDM:
            tracklet->setNext(nullptr);
            break;
        case COLO:
            qDebug() << "Unknown status or colony (which is unimplemented)!";
            tracklet->setNext(nullptr);
            break;
        }

        trackElement = trackElement.nextSiblingElement("Track");
    }

    fileExport->close();

    return true;
}
#endif /* if 0 */
}
