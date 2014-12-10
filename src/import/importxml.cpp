#include "importxml.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QDomDocument>

#include <iostream>

#include "../base_data/movie.h"

ImportXML::ImportXML()
{
}

Movie *ImportXML::load(QString dir)
{
    Movie *movie;
    QString fpFileXML, fpDirImages, fpDirXML;
    QFile *fileXML;
    QDir *dirImages, *dirXML;

    movie = new Movie();

    qDebug() << "ImportXML::load()";
    qDebug() << dir;

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
        qDebug() << "Either the directory xml or images does not exist";
        goto err;
    }

    fpFileXML = qd.filePath("tracksXML.xml");
    fpDirImages = qd.filePath("images");
    fpDirXML = qd.filePath("xml");

    fileXML = new QFile(fpFileXML);
    if (!fileXML->open(QIODevice::ReadOnly)){
        qDebug() << "Opening the file tracksXML.xml failed";
        goto cleanup;
    }

    dirImages = new QDir(fpDirImages);
    dirImages->setFilter(QDir::Files|QDir::NoDotAndDotDot);
    if(!dirImages->isReadable()){
        qDebug() << "Directory images is not readable";
        goto cleanup;
    }

    dirXML = new QDir(fpDirXML);
    dirXML->setFilter(QDir::Files|QDir::NoDotAndDotDot);
    if(!dirXML->isReadable()){
        qDebug() << "Directory xml is not readable";
        goto cleanup;
    }

    {   QDirIterator imgit(*dirImages, QDirIterator::NoIteratorFlags);
        uint32_t id = 0;
        while (imgit.hasNext()){
            QString name = imgit.next();
            qDebug() << "Adding image " << name;
            Frame *frame = new Frame();
            Slice *slice = new Slice();
            Channel *channel = new Channel();
            QImage *image = new QImage(name);

            movie->addFrame(frame);
            frame->addSlice(slice);
            frame->setID(id);
            slice->addChannel(channel);
            channel->setImage(image);
            channel->setChanName("Regular Channel");

            id++;
    }}

    {   QDirIterator xmlit(*dirXML, QDirIterator::NoIteratorFlags);
        uint32_t frameID = 0;
        while (xmlit.hasNext()){
            QString name = xmlit.next();
            qDebug() << "Found xml: " << name;
            QFile file(name);
            QDomDocument doc;

            /* TODO: Error handling */
            doc.setContent(&file,false,nullptr,nullptr,nullptr);
            QDomElement root = doc.documentElement();
            QDomElement c1, c2, c3;

            c1 = root.firstChildElement("Object");
            while (!c1.isNull()){
                Object *object = new Object();
                uint32_t objID = UINT32_MAX;
                uint32_t trackID = UINT32_MAX;
                QPoint *centroid = new QPoint();
                QRect * bBox = new QRect();
                QPolygonF *outline = new QPolygonF();

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

                /* TODO: set the object in the channel */
                Frame *f = movie->getFrame(frameID);
                Slice *s = f->getSlice(0,0);
                Channel *c = s->getChannel("Regular Channel");
                c->addObject(object);

                c1 = c1.nextSiblingElement("Object");
            }

            frameID++;
        }
    }

cleanup:
    fileXML->close();
    delete fileXML;
    delete dirImages;
    delete dirXML;

err:
    return movie;
}
