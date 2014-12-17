#include "importxml.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QDomDocument>

#include <iostream>

#include "../base_data/movie.h"
#include "../corrected_data/genealogy.h"
#include "../corrected_data/trackletregular.h"
#include "../corrected_data/trackeventdead.h"
#include "../corrected_data/trackeventdivision.h"
#include "../corrected_data/trackeventlost.h"
#include "../corrected_data/trackeventmerge.h"
#include "../corrected_data/trackeventunmerge.h"

#define DEFAULT_CHANNEL "Default Channel"
#define DEFAULT_SLICE 0,0

ImportXML::ImportXML()
{
}

std::shared_ptr<Project> ImportXML::load(QString dir)
{
    std::shared_ptr<Project> project(new Project());
    std::shared_ptr<Movie> movie(new Movie());
    std::shared_ptr<Genealogy> genealogy(new Genealogy());
    QString fpFileXML, fpDirImages, fpDirXML, fpFileExport;
    std::unique_ptr<QFile> fileXML, fileExport;
    std::unique_ptr<QDir> dirImages, dirXML;

    project->setMovie(movie);
    project->setGenealogy(genealogy);

    qDebug() << "ImportXML::load(" << dir << ")";

    QDir qd(dir);
    if (!qd.exists() || !qd.isReadable()){
        qDebug() << "Directory " << dir << " does not exists or is not readable.";
        goto err;
    }

    if (!qd.exists("tracksXML.xml")){
        qDebug() << "No file tracksXML.xml does exist in the directory.";
        goto err;
    }

    if (!qd.exists("xml") || !qd.exists("images")){
        qDebug() << "Either the directory xml or images does not exist.";
        goto err;
    }

    if (!qd.exists("NewTracks_export.xml")){
        qDebug() << "File NewTracks_export.xml does not exist.";
    }

    fpFileXML = qd.filePath("tracksXML.xml");
    fpDirImages = qd.filePath("images");
    fpDirXML = qd.filePath("xml");
    fpFileExport = qd.filePath("NewTracks_export.xml");

    fileXML = std::unique_ptr<QFile>(new QFile(fpFileXML));
    if (!fileXML->open(QIODevice::ReadOnly)){
        qDebug() << "Opening the file tracksXML.xml failed";
        goto cleanup;
    }

    dirImages = std::unique_ptr<QDir>(new QDir(fpDirImages));
    dirImages->setFilter(QDir::Files|QDir::NoDotAndDotDot);
    if(!dirImages->isReadable()){
        qDebug() << "Directory images is not readable";
        goto cleanup;
    }

    dirXML = std::unique_ptr<QDir>(new QDir(fpDirXML));
    dirXML->setFilter(QDir::Files|QDir::NoDotAndDotDot);
    if(!dirXML->isReadable()){
        qDebug() << "Directory xml is not readable";
        goto cleanup;
    }

    fileExport = std::unique_ptr<QFile>(new QFile(fpFileExport));
    if (!fileExport->open(QIODevice::ReadOnly)){
        qDebug() << "OPening the file NewTracks_export.xml failed.";
        goto cleanup;
    }

    /* Read all the images in dirImages */
    {
        qDebug() << "Reading folder img";
        QDirIterator imgit(*dirImages, QDirIterator::NoIteratorFlags);
        uint32_t id = 0;
        while (imgit.hasNext()){
            QString name = imgit.next();
            std::shared_ptr<Frame> frame(new Frame());
            std::shared_ptr<Slice> slice(new Slice());
            std::shared_ptr<Channel> channel(new Channel());
            std::shared_ptr<QImage> image(new QImage(name));

            movie->addFrame(frame);
            frame->addSlice(slice);
            frame->setID(id);
            slice->addChannel(channel);
            channel->setImage(image);
            channel->setChanName(DEFAULT_CHANNEL);

            id++;
        }
    }

    /* Parse all the files in the dirXML directory */
    {
        qDebug() << "Reading folder xml";
        QDirIterator xmlit(*dirXML, QDirIterator::NoIteratorFlags);
        uint32_t frameID = 0;
        while (xmlit.hasNext()){
            QString name = xmlit.next();
            QFile file(name);
            QDomDocument doc;

            /*! \todo Error handling */
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
                if(!c2.isNull())
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
                c2 = c1.firstChildElement("TrackID");
                if (!c2.isNull()){
                    trackID = (c2.firstChildElement("value").text().toInt())-1;
                }

                object->setID(objID);
                object->setTrackID(trackID);
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
    }

    /* Parse tracksXML.xml */
    {
        qDebug() << "Reading file tracksXML.xml";
        QDomDocument doc;

        /*! \todo Error handling */
        doc.setContent(fileXML.get(),true,nullptr,nullptr,nullptr);

        QDomElement root = doc.documentElement();
        QDomElement trackElement = root.firstChildElement("Track");
        QDomElement objectElement = trackElement.firstChildElement("object");
        QDomElement objectIDElement = objectElement.firstChildElement("ObjectID");
        QDomElement timeElement = objectElement.firstChildElement("Time");

        while (!trackElement.isNull()) {
            int trackID, cellID, time;
            std::shared_ptr<AutoTracklet> tracklet = std::shared_ptr<AutoTracklet>(new AutoTracklet());

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
                    std::shared_ptr<Object> obj = chan->getObject(cellID);

                    tracklet->addComponent(frame,obj);

                    objectElement = objectElement.nextSiblingElement("object");
                }
            }

            /* collect the generated auto_tracklet in project */
            project->addAutoTracklet(tracklet);

            trackElement = trackElement.nextSiblingElement("Track");
        }
    }

    /* Import tracks if they exist */
    {
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

        /*! \todo Error handling */
        doc.setContent(fileExport.get(),true,nullptr,nullptr,nullptr);

        QDomElement root = doc.documentElement();
        QDomElement trackElement = root.firstChildElement("Track");

        while (!trackElement.isNull()) {
            int id;
            std::shared_ptr<Tracklet> tracklet = std::shared_ptr<Tracklet>(new Tracklet());

            QDomElement idElement = trackElement.firstChildElement("TrackID");
            QDomElement objectElement = trackElement.firstChildElement("object");

            id = idElement.text().toInt();
            tracklet->setID(id);

            /* find the autotracklet belonging to the first object */
            while (!objectElement.isNull()){
                int time = objectElement.firstChildElement("Time").text().toInt()-1; /*!< \todo -1? */
                int oID = objectElement.firstChildElement("ObjectID").text().toInt()-1; /*!< \todo -1? */

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
            int s, motherID, daughterID, trackID;
            QList<std::shared_ptr<Tracklet>> daughters;
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
                return nullptr;
            }

            motherID = motherElement.text().toInt(); /*!< \todo -1? */

            while (!daugtherIDElement.isNull()) {
                daughterID = daugtherIDElement.text().toInt(); /*!< \todo -1? */
                daughters.append(genealogy->getTracklet(daughterID));
                daugtherIDElement = daugtherIDElement.nextSiblingElement("DaugterID");
            }

            switch (status) {
            case DIVI: {
                std::shared_ptr<TrackEventDivision> eventDivision = std::shared_ptr<TrackEventDivision>(new TrackEventDivision());
                eventDivision->setPrev(genealogy->getTracklet(motherID));
                eventDivision->setNext(daughters);
                tracklet->setNext(eventDivision); }
                break;
            case DEAD: {
                std::shared_ptr<TrackEventDead> eventDead = std::shared_ptr<TrackEventDead>(new TrackEventDead());
                eventDead->setPrev(genealogy->getTracklet(motherID));
                tracklet->setNext(eventDead); }
                break;
            case LOST: {
                std::shared_ptr<TrackEventLost> eventLost = std::shared_ptr<TrackEventLost>(new TrackEventLost());
                eventLost->setPrev(genealogy->getTracklet(motherID));
                tracklet->setNext(eventLost); }
                break;
            case OPEN: /* fall-through */
            case ENDM:
                tracklet->setNext(nullptr);
                break;
            case COLO:
            default:
                qDebug() << "Unknown status or colony (which is unimplemented)!";
                tracklet->setNext(nullptr);
                break;
            }

            trackElement = trackElement.nextSiblingElement("Track");
        }

        /*! \todo Add only roots to the genealogy */
    }

    qDebug() << "Parsing was successfull";

cleanup:
    fileExport->close();
    fileXML->close();

err:
    return project;
}

/*
std::shared_ptr<Project> loadImages(QDir dir)
{
        return nullptr;
}

std::shared_ptr<Project> loadObjects(QDir dir)
{
        return nullptr;
}

std::shared_ptr<Project> loadTracklets(QFile file)
{
        return nullptr;
}*/
