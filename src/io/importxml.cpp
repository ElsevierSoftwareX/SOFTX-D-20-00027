#include "importxml.h"

#include <memory>

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QDomDocument>
#include <QImage>

#include "exceptions/ctimportexception.h"
#include "provider/guistate.h"
#include "provider/messagerelay.h"

namespace CellTracker {

std::shared_ptr<Project> ImportXML::load(QString filePath) {
    std::shared_ptr<Project> proj = Import::setupEmptyProject();
    QDir qd(filePath);
    proj->setFileName(filePath);
    Project::XMLProjectSpec xps;
    xps.cols = 1;
    xps.rows = 1;
    xps.projectFile = qd.filePath("");
    Project::XMLSliceSpec xss;
    xss.channels.append(qd.filePath("images"));
    xss.tracks = qd.filePath("tracksXML.xml");
    xss.xml = qd.filePath("xml");
    xps.slices.append(xss);

    MessageRelay::emitUpdateOverallName("Importing from XML");
    MessageRelay::emitUpdateOverallMax(4);

    /* setup frames/slices/channels. as the XML-format does not support Slices/Channels,
     *  this can be done in one step */
    bool ret = loadFrames(filePath, proj, 0, 0);
    if (!ret)
        throw CTImportException("loading of frames failed");
    MessageRelay::emitIncreaseOverall();

    ret = loadInfo(filePath, proj);
    if (!ret)
        throw CTImportException("loading of info failed");
    MessageRelay::emitIncreaseOverall();

    ret = loadObjects(filePath, proj, 0, 0);
    if (!ret)
        throw CTImportException("loading of objects failed");
    MessageRelay::emitIncreaseOverall();

    ret = loadAutoTracklets(filePath + "/tracksXML.xml", proj, 0, 0);
    if (!ret)
        throw CTImportException("loading of autotracklets failed");
    MessageRelay::emitIncreaseOverall();

    proj->setProjectSpec(xps);
    proj->setImported(true);
    return proj;
}

std::shared_ptr<Project> ImportXML::load(Project::XMLProjectSpec &spec)
{
    std::shared_ptr<Project> proj = Import::setupEmptyProject();
    proj->setFileName(spec.projectFile);

    MessageRelay::emitUpdateOverallName("Importing from XML");
    MessageRelay::emitUpdateOverallMax(4);

    bool ret = true;
    int numSlices = spec.slices.size();

    for (int sNr = 0; sNr < numSlices; sNr++) {
        int numChannels = spec.slices.at(sNr).channels.size();
        Project::XMLSliceSpec slice = spec.slices[sNr];
        for (int cNr = 0; cNr < numChannels; cNr++) {
            QString channel = slice.channels[cNr];

            ret = loadFrames(channel, proj, sNr, cNr);
            if (!ret)
                throw CTImportException("loading of frames failed");
        }
    }
    MessageRelay::emitIncreaseOverall();

    for (int sNr = 0; sNr < numSlices; sNr++) {
        int numChannels = spec.slices.at(sNr).channels.size();
        Project::XMLSliceSpec slice = spec.slices[sNr];
        for (int cNr = 0; cNr < numChannels; cNr++) {
            QString channel = slice.channels[cNr];

            ret = loadInfo(channel, proj);
            if (!ret)
                throw CTImportException("loading of info failed");
        }
    }
    MessageRelay::emitIncreaseOverall();

    for (int sNr = 0; sNr < numSlices; sNr++) {
        int numChannels = spec.slices.at(sNr).channels.size();
        Project::XMLSliceSpec slice = spec.slices[sNr];
        for (int cNr = 0; cNr < numChannels; cNr++) {
            ret = loadObjects(slice.xml, proj, sNr, cNr);
            if (!ret)
                throw CTImportException("loading of objects failed");
        }
    }
    MessageRelay::emitIncreaseOverall();

    for (int sNr = 0; sNr < numSlices; sNr++) {
        int numChannels = spec.slices.at(sNr).channels.size();
        Project::XMLSliceSpec slice = spec.slices[sNr];
        for (int cNr = 0; cNr < numChannels; cNr++) {
            ret = loadAutoTracklets(slice.tracks, proj, sNr, cNr);
            if (!ret)
                throw CTImportException("loading of autotracklets failed");
        }
    }
    MessageRelay::emitIncreaseOverall();

    proj->setProjectSpec(spec);
    proj->setImported(true);
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

bool ImportXML::loadFrames(QString filePath, std::shared_ptr<Project> const &proj, int sliceNr, int channelNr) {
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
        std::shared_ptr<Frame> frame = mov->getFrame(frameNr);
        if (!frame) {
            frame = std::make_shared<Frame>(frameNr);
            mov->addFrame(frame);
        }
        std::shared_ptr<Slice> slice = frame->getSlice(sliceNr);
        if (!slice) {
            slice = std::make_shared<Slice>(sliceNr, frameNr);
            frame->addSlice(slice);
        }
        std::shared_ptr<Channel> chan = slice->getChannel(channelNr);
        if (!chan) {
            chan  = std::make_shared<Channel>(channelNr, sliceNr, frameNr);
            slice->addChannel(chan);
        }

        dit.next();
        MessageRelay::emitIncreaseDetail();
    }

    return true;
}

bool ImportXML::loadObjects(QString filePath, std::shared_ptr<Project> const &proj, int sliceNr, int channelNr) {
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
        std::shared_ptr<Channel> chan = mov->getFrame(frameNr)->getSlice(sliceNr)->getChannel(channelNr);

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
        throw CTImportException(frameName + " not found in file " + fileName.toStdString());
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
    Q_UNUSED(filePath)

    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    if (!proj)
        return nullptr;
    Project::XMLProjectSpec projectSpec = proj->getProjectSpec();

    /* frame n is the n-th file in th images directory */
    QDir imgDir(projectSpec.slices.at(slice).channels.at(channel));
    if (!imgDir.exists() || !imgDir.isReadable())
        throw CTImportException("The root directory of the XML project does not exist or is not readable");
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

bool ImportXML::loadAutoTracklets(QString fileName, std::shared_ptr<Project> const &proj, int sliceNr, int channelNr) {
    using QDE = QDomElement;

    std::shared_ptr<Genealogy> gen = proj->getGenealogy();
    std::shared_ptr<Movie> mov = proj->getMovie();

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

        std::shared_ptr<AutoTracklet> at = std::make_shared<AutoTracklet>(tid);

        for (QDE objElem = trackElem.firstChildElement("object"); !objElem.isNull(); objElem = objElem.nextSiblingElement("object")) {
            QDE objID = objElem.firstChildElement("ObjectID");
            QDE frameID = objElem.firstChildElement("Time");

            unsigned oid = objID.text().toUInt() - 1; /* Object IDs 1-based in XML format */
            unsigned fid = frameID.text().toUInt() - 1; /* Frame IDs 1-based in XML format */

            std::shared_ptr<Frame> frame = mov->getFrame(fid);
            if (!frame)
                throw CTImportException("Did not find frame");

            std::shared_ptr<Slice> slice = frame->getSlice(sliceNr);
            if (!slice)
                throw CTImportException("Did not find slice");

            std::shared_ptr<Object> obj = slice->getChannel(channelNr)->getObject(oid);
            if (!obj)
                throw CTImportException("Did not find object");

            at->addComponent(frame, obj);
        }
        proj->addAutoTracklet(at);
        MessageRelay::emitIncreaseDetail();
    }
    return true;
}

}

std::ostream &operator<<(std::ostream &os, QString &q) {
    return os << q.toStdString();
}

std::ostream &operator<<(std::ostream &os, CellTracker::Project::XMLSliceSpec &s) {
    os << "slice { .tracks = " << s.tracks << ", .xml = " << s.xml << ", .channels = {";
    for (QString &c : s.channels)
        os << c << ",";
    os << "} }";
    return os;
}
std::ostream &operator<<(std::ostream &os, CellTracker::Project::XMLProjectSpec &p) {
    os << "project { .projectFile = " << p.projectFile << ", .rows = " << p.rows << ", .cols = " << p.cols << ", .slices = {";
    for (CellTracker::Project::XMLSliceSpec &s : p.slices)
        os << s << ", ";
    os << "} }";
    return os;
}

