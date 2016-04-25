#include "importxml.h"

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

namespace CellTracker {

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
        std::shared_ptr<Frame> frame(new Frame(id));
        std::shared_ptr<Slice> slice(new Slice(DEFAULT_SLICE,id));
        std::shared_ptr<Channel> channel(new Channel(DEFAULT_CHANNEL,DEFAULT_SLICE,id));
        std::shared_ptr<QImage> image(new QImage(name));

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
            std::shared_ptr<Object> object(new Object());
            uint32_t objID = UINT32_MAX;
            uint32_t trackID = UINT32_MAX;
            std::shared_ptr<QPoint> centroid(new QPoint());
            std::shared_ptr<QRect> bBox(new QRect());
            std::shared_ptr<QPolygonF> outline(new QPolygonF());

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
        std::shared_ptr<AutoTracklet> tracklet = std::make_shared<AutoTracklet>();

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
        std::shared_ptr<Tracklet> tracklet = std::make_shared<Tracklet>();

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
        std::shared_ptr<QList<std::shared_ptr<Tracklet>>> daughters = std::make_shared<QList<std::shared_ptr<Tracklet>>>();
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
            std::shared_ptr<TrackEventDivision<Tracklet>> eventDivision = std::make_shared<TrackEventDivision<Tracklet>>();
            eventDivision->setPrev(genealogy->getTracklet(motherID));
            eventDivision->setNext(daughters);
            tracklet->setNext(eventDivision); }
            break;
        case DEAD: {
            std::shared_ptr<TrackEventDead<Tracklet>> eventDead = std::make_shared<TrackEventDead<Tracklet>>();
            eventDead->setPrev(genealogy->getTracklet(motherID));
            tracklet->setNext(eventDead); }
            break;
        case LOST: {
            std::shared_ptr<TrackEventLost<Tracklet>> eventLost = std::make_shared<TrackEventLost<Tracklet>>();
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

}
