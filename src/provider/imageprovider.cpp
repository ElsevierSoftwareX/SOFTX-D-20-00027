#include <QtDebug>
#include <QPainter>

#include "imageprovider.h"
#include "tracked/trackevent.h"
#include "tracked/trackeventdead.h"
#include "tracked/trackeventdivision.h"
#include "tracked/trackeventendofmovie.h"
#include "tracked/trackeventlost.h"
#include "tracked/trackeventmerge.h"
#include "tracked/trackeventunmerge.h"
#include "provider/ctsettings.h"
#include "provider/dataprovider.h"
#include "provider/guistate.h"

#ifndef GIT_VERSION
#define GIT_VERSION "unknown"
#endif

using namespace CellTracker;

/*!
 * \brief constructor of ImageProvider
 */
ImageProvider::ImageProvider() :
    QQuickImageProvider(Image) {}

/*!
 * \brief tells, if a given object is currently selected
 * \param o the Object to check
 * \return true if it is selected, false if not
 */
bool ImageProvider::cellIsSelected(std::shared_ptr<Object> o) {
    std::shared_ptr<Object> selected = GUIState::getInstance()->getSelectedCell();

    return (selected
            && selected->getId() == o->getId()
            && selected->getFrameId() == o->getFrameId());
}

/*!
 * \brief tells, if the AutoTracklet of a given Object is selected
 * \param o the Object to check
 * \return true if the Object%s AutoTracklet is selected, false if not
 */
bool ImageProvider::cellAutoTrackletIsSelected(std::shared_ptr<Object> o) {
    std::shared_ptr<AutoTracklet> selected = GUIState::getInstance()->getSelectedAutoTrack();
    return (selected
            && selected->getID() >= 0
            && (uint32_t)selected->getID() == o->getAutoId());
}

/*!
 * \brief tells, if the Object is hovered
 * \param o the Object to check
 * \return true if it is hovered, false if not
 */
bool ImageProvider::cellIsHovered(std::shared_ptr<Object> o) {
    std::shared_ptr<Object> hovered = GUIState::getInstance()->getHoveredCell();

    return (hovered
            && hovered->getId() == o->getId()
            && hovered->getFrameId() == o->getFrameId());
}

/*!
 * \brief tells, if the given Object is the beginning of a daughter track of the currently selected Tracklet
 * \param daughter the Object to check
 * \return true if it is, false otherwise
 */
bool ImageProvider::cellIsInDaughters(std::shared_ptr<Object> daughter) {
    bool objInDaughters = false;

    std::shared_ptr<Tracklet> t = GUIState::getInstance()->getSelectedTrack();

    if(t && t->getNext() && t->getNext()->getType() == TrackEvent<Tracklet>::EVENT_TYPE_DIVISION) {
        std::shared_ptr<TrackEventDivision<Tracklet>> ev = std::static_pointer_cast<TrackEventDivision<Tracklet>>(t->getNext());
        for (std::shared_ptr<Tracklet> dt: *ev->getNext()) {
            objInDaughters |= dt->getStart().second == daughter;
        }
    }

    return objInDaughters;
}

/*!
 * \brief tells, if a given Object is in a Tracklet
 * \param o the Object to check
 * \return true if the Object is in a Tracklet, false otherwise
 */
bool ImageProvider::cellIsInTracklet(std::shared_ptr<Object> o) {
    return o->isInTracklet();
}

/*!
 * \brief returns the line color for a given object
 * \param o the Object whose line color should be returned
 * \return the line color for this object
 */
QColor ImageProvider::getCellLineColor(std::shared_ptr<Object> o) {
    QColor lineColor;

    if (cellIsSelected(o)) {
        lineColor = CTSettings::value("drawing/selected_linecolor").value<QColor>();
    } else {
        lineColor = CTSettings::value("drawing/unselected_linecolor").value<QColor>();
    }

    return lineColor;
}

/*!
 * \brief returns the line width for a given object
 * \param o the Object whose line width should be returned
 * \return the line width for this object
 */
qreal ImageProvider::getCellLineWidth(std::shared_ptr<Object> o) {
    qreal lineWidth;

    if (cellIsSelected(o)) {
        lineWidth = CTSettings::value("drawing/selected_linewidth").toReal();
    } else {
        lineWidth = CTSettings::value("drawing/default_linewidth").toReal();
    }

    return lineWidth;
}

/*!
 * \brief tells, if the given Object is related to the currently selected Object
 * \param o the Object to check
 * \return true if it is, false otherwise
 *
 * \warning this function seems to be quite buggy.
 */
bool ImageProvider::cellIsRelated(std::shared_ptr<Object> o) {
    std::shared_ptr<Tracklet> selected = GUIState::getInstance()->getSelectedTrack();
    int currentFrame = GUIState::getInstance()->getCurrentFrame();

    QList<std::shared_ptr<Tracklet>> openList;
    QList<std::shared_ptr<Tracklet>> closedList;

    if (!o || !selected)
        return false;

    openList.push_back(selected);
    while (true) {
        QList<std::shared_ptr<TrackEvent<Tracklet>>> eventList;

        if (openList.isEmpty())
            return false;

        std::shared_ptr<Tracklet> currTracklet = openList.takeFirst();
        closedList.push_back(currTracklet);

        if (currTracklet->hasObjectAt(o->getId(), currentFrame))
            return true;

        /* get events */
        std::shared_ptr<TrackEvent<Tracklet>> prev = currTracklet->getPrev();
        std::shared_ptr<TrackEvent<Tracklet>> next = currTracklet->getNext();

        if (prev)
            eventList.push_back(prev);
        if (next)
            eventList.push_back(next);

        for (std::shared_ptr<TrackEvent<Tracklet>> currEv : eventList) {
            switch (currEv->getType()) {
            case TrackEvent<Tracklet>::EVENT_TYPE_DIVISION: {
                std::shared_ptr<TrackEventDivision<Tracklet>> ev = std::static_pointer_cast<TrackEventDivision<Tracklet>>(currEv);
                std::shared_ptr<Tracklet> prev = ev->getPrev();
                std::shared_ptr<QList<std::shared_ptr<Tracklet>>> next = ev->getNext();
                if (prev && !openList.contains(prev) && !closedList.contains(prev))
                    openList.push_back(prev);
                for (std::shared_ptr<Tracklet> t: *next)
                    if (!openList.contains(t) && !closedList.contains(t))
                        openList.push_back(t);
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_MERGE: {
                std::shared_ptr<TrackEventMerge<Tracklet>> ev = std::static_pointer_cast<TrackEventMerge<Tracklet>>(currEv);
                std::shared_ptr<QList<std::shared_ptr<Tracklet>>> prev = ev->getPrev();
                std::shared_ptr<Tracklet> next = ev->getNext();
                for (std::shared_ptr<Tracklet> t: *prev)
                    if (!openList.contains(t) && !closedList.contains(t))
                        openList.push_back(t);
                if (next && !openList.contains(next) && !closedList.contains(next))
                    openList.push_back(next);
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_UNMERGE: {
                std::shared_ptr<TrackEventUnmerge<Tracklet>> ev = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(currEv);
                std::shared_ptr<Tracklet> prev = ev->getPrev();
                std::shared_ptr<QList<std::shared_ptr<Tracklet>>> next = ev->getNext();
                if (prev && !openList.contains(prev) && !closedList.contains(prev))
                    openList.push_back(prev);
                for (std::shared_ptr<Tracklet> t: *next)
                    if (!openList.contains(t) && !closedList.contains(t))
                        openList.push_back(t);
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_LOST: {
                std::shared_ptr<TrackEventLost<Tracklet>> ev = std::static_pointer_cast<TrackEventLost<Tracklet>>(currEv);
                std::shared_ptr<Tracklet> prev = ev->getPrev();
                if (prev && !openList.contains(prev) && !closedList.contains(prev))
                    openList.push_back(prev);
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_DEAD: {
                std::shared_ptr<TrackEventDead<Tracklet>> ev = std::static_pointer_cast<TrackEventDead<Tracklet>>(currEv);
                std::shared_ptr<Tracklet> prev = ev->getPrev().lock();
                if (prev && !openList.contains(prev) && !closedList.contains(prev))
                    openList.push_back(prev);
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_ENDOFMOVIE: {
                std::shared_ptr<TrackEventEndOfMovie<Tracklet>> ev = std::static_pointer_cast<TrackEventEndOfMovie<Tracklet>>(currEv);
                std::shared_ptr<Tracklet> prev = ev->getPrev().lock();
                if (prev && !openList.contains(prev) && !closedList.contains(prev))
                    openList.push_back(prev);
                break; }
            }
        }
    }
}

/*!
 * \brief returns the brush style to use for drawing the given Object
 * \param o used to decide which brush style to return
 * \param outline (unused) may be used to decide which brush style to return
 * \param mousePos (unused) may be used to decide which brush style to return
 * \return the brush style for this objects
 */
Qt::BrushStyle ImageProvider::getCellBrushStyle(std::shared_ptr<Object> o, QPolygonF &outline, QPointF &mousePos)
{
    Q_UNUSED(outline)
    Q_UNUSED(mousePos)

    Qt::BrushStyle style;
    /* maybe do something else here */
    if (cellIsRelated(o)) {
        style = Qt::CrossPattern;
    } else
        style = Qt::SolidPattern;

    return style;
}

/*!
 * \brief returns the background color to use for drawing the given Object
 * \param o the Object for which the background color should be returned
 * \return the background color that should be used for drawing this Object
 */
QColor ImageProvider::getCellBgColor(std::shared_ptr<Object> o)
{
    QColor bgColor;

    if (cellIsHovered(o)) {
        bgColor = CTSettings::value("drawing/active_cell").value<QColor>();
    } else if (cellIsRelated(o)) {
        bgColor = Qt::black;
    } else if (cellIsInDaughters(o)) {
        bgColor = CTSettings::value("drawing/merge_cell").value<QColor>();
    } else if (cellIsInTracklet(o)) {
        bgColor = CTSettings::value("drawing/finished_cell").value<QColor>();
    } else if (cellAutoTrackletIsSelected(o)) {
        bgColor = CTSettings::value("drawing/selected_track").value<QColor>();
    } else {
        bgColor = CTSettings::value("drawing/default_cell").value<QColor>();
    }

    return bgColor;
}

/*!
 * \brief draws the Polygon on the Painter using the given color and style
 * \param painter the Painter to draw to
 * \param poly the Polygon to draw
 * \param col the color in which to draw
 * \param style the style to use when drawing
 */
void ImageProvider::drawPolygon(QPainter &painter, QPolygonF &poly, QColor col, Qt::BrushStyle style) {
    QBrush brush(col, style);
    brush.setColor(col);

    /* draws very nicely, but is slow */
    QPainterPath path;
    path.addPolygon(poly);
    painter.setOpacity(CTSettings::value("drawing/cell_opacity").toReal());
    painter.drawPath(path);
    painter.fillPath(path, brush);
}

/*!
 * \brief draws all Outlines of Objects in the given Frame
 * \param image the Image to draw to
 * \param frame the current Frame
 * \param scaleFactor the scaleFactor to use
 */
void ImageProvider::drawOutlines(QImage &image, int frame, double scaleFactor) {
    /* set up painting equipment */
    QPainter painter(&image);
    if (!painter.isActive())
        return;

    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    if (!proj)
        return;

    /* collect the polygons we want to draw */
    QList<std::shared_ptr<Object>> allObjects;
    for (std::shared_ptr<Slice> s : proj->getMovie()->getFrame(frame)->getSlices())
        for (std::shared_ptr<Channel> c : s->getChannels().values())
            allObjects.append(c->getObjects().values());

    /* the transformation to apply to the points of the polygons */
    QTransform trans;
    trans = trans.scale(scaleFactor, scaleFactor);

    for (std::shared_ptr<Object> o : allObjects) {
        QPolygonF curr = trans.map(*o->getOutline());

        QPointF mousePos(GUIState::getInstance()->getMouseX(),
                         GUIState::getInstance()->getMouseY());

        QColor lineColor = getCellLineColor(o);
        qreal lineWidth = getCellLineWidth(o);

        QPen pen(lineColor, lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);

        QColor bgColor = getCellBgColor(o);
        Qt::BrushStyle bStyle = getCellBrushStyle(o, curr, mousePos);
        drawPolygon(painter, curr, bgColor, bStyle);
    }
}

/*!
 * \brief draws additional information about all Objects in a given Frame
 * \param image the Image to draw to
 * \param frame the current Frame
 * \param scaleFactor the scaleFactor to use
 * \param drawTrackletIDs whether Tracklet-IDs should be drawn
 * \param drawAnnotationInfo whether information about Annotation%s should be drawn
 */
void ImageProvider::drawObjectInfo(QImage &image, int frame, double scaleFactor, bool drawTrackletIDs, bool drawAnnotationInfo) {
    if (!drawTrackletIDs && !drawAnnotationInfo)
        return;

    /* set up painting equipment */
    QPainter painter(&image);
    if (!painter.isActive())
        return;

    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    if (!proj)
        return;

    /* collect the polygons we want to draw */
    QList<std::shared_ptr<Object>> allObjects;
    for (std::shared_ptr<Slice> s : proj->getMovie()->getFrame(frame)->getSlices())
        for (std::shared_ptr<Channel> c : s->getChannels().values())
            allObjects.append(c->getObjects().values());

    for (std::shared_ptr<Object> o : allObjects) {
        /* draw the trackid */
        std::string text = "";
        if (drawTrackletIDs && o && o->isInTracklet())
            text = std::to_string(o->getTrackId());

        if (text.length() != 0) {
            QFont font = painter.font();
            font.setPointSize(CTSettings::value("text/trackid_fontsize").toInt());
            font.setBold(true);
            painter.setFont(font);
            QPen pen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            QColor col = CTSettings::value("text/trackid_color").value<QColor>();
            pen.setColor(col);
            painter.setPen(pen);
            painter.setOpacity(1);
            painter.drawText(o->getBoundingBox()->center() * scaleFactor,QString(text.c_str()));
        }

        if (drawAnnotationInfo && o) {
            std::shared_ptr<Tracklet> t = GUIState::getInstance()->getProj()->getGenealogy()->getTracklet(o->getTrackId());
            QImage objectAnnotationImage(":/icons/object_annotation.svg");
            QImage trackletAnnotationImage(":/icons/tracklet_annotation.svg");
            QPointF imageDims(14, 21);
            QPointF spacing(2, 0);
            if (o->isAnnotated()) {
                QPointF br = o->getBoundingBox()->center() * scaleFactor - spacing;
                QPointF tl = br - imageDims;
                QRectF rect(tl,br);
                painter.drawImage(rect, objectAnnotationImage);
            }
            if (t && t->isAnnotated()) {;
                QPointF br = o->getBoundingBox()->center() * scaleFactor - spacing;
                if (o->isAnnotated()) br = br - QPointF(imageDims.x(), 0) - spacing;
                QPointF tl = br - imageDims;
                QRectF rect(tl,br);
                painter.drawImage(rect, trackletAnnotationImage);
            }
        }
    }
}

/*!
 * \brief returns a default image for use in absence of a Project
 * \param size the size of the image that is returned
 * \param requestedSize the requested image size
 * \return the default image (currently displaying "CellTracker")
 */
QImage ImageProvider::defaultImage(QSize *size, const QSize &requestedSize = QSize(600,600)) {
    QImage defaultImage(requestedSize.width(),requestedSize.height(),QImage::Format_RGB32);
    defaultImage.fill(Qt::white);
    size->setHeight(defaultImage.height());
    size->setWidth(defaultImage.width());

    QPainter painter(&defaultImage);

    if (!painter.isActive())
        return defaultImage;

    int w = defaultImage.width(), h = defaultImage.height();
    painter.setFont(QFont("DejaVu Serif", 64));
    painter.setPen(QPen(QColor(63,191,0)));
    painter.drawText(QRect(0,0,w,h),"CellTracker", QTextOption(Qt::AlignHCenter|Qt::AlignVCenter));
    painter.setFont(QFont("DejaVu Serif", 26));
    painter.drawText(QRect(w-50,h-50,50,50), "α", QTextOption(Qt::AlignHCenter|Qt::AlignVCenter));
    painter.setFont(QFont("DejaVu Sans", 10));
    painter.drawText(QRect(w-200,0,200,20), "version: " + QString(GIT_VERSION), QTextOption(Qt::AlignRight|Qt::AlignBottom));

    return defaultImage;
}

void ImageProvider::drawCutLine(QImage &image) {
    int startX = GUIState::getInstance()->getStartX();
    int startY = GUIState::getInstance()->getStartY();
    int endX = GUIState::getInstance()->getMouseX();
    int endY = GUIState::getInstance()->getMouseY();

    QLine line(startX, startY, endX, endY);
    QPainter painter(&image);
    painter.drawLine(line);
}

/*!
 * \brief Loads an image and draws the outlines of the cells.
 * \param id is an unused variable
 * \param size is an unused variable
 * \param requestedSize is an unused variable
 * \return a new QImage
  */
QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id);
    QImage newImage;

    int frame = GUIState::getInstance()->getCurrentFrame();
    QString path = GUIState::getInstance()->getProjPath();

    if (requestedSize.height() <= 0 || requestedSize.width() <= 0)
        return defaultImage(size);
    if (path.isEmpty() || frame < 0 || frame > GUIState::getInstance()->getMaximumFrame())
        return defaultImage(size, requestedSize);

    /* some caching, so we don't always re-request the image */
    if (frame == cachedFrame && path == cachedPath) {
        newImage = cachedImage;
    } else {
        newImage = DataProvider::getInstance()->requestImage(path, frame);
        cachedImage = newImage;
        cachedPath = path;
        cachedFrame = frame;
    }

    if (!requestedSize.isValid())
        return newImage;

    double oldWidth = newImage.width();
    newImage = newImage.scaled(requestedSize,Qt::KeepAspectRatio);
    double newWidth = newImage.width();
    double scaleFactor = newWidth/oldWidth;
    DataProvider::getInstance()->setScaleFactor(scaleFactor);

    /* draw the outlines over the given image if drawOutlines is enabled */
    bool drawingOutlines = GUIState::getInstance()->getDrawOutlines();
    bool drawingTrackletIDs = GUIState::getInstance()->getDrawTrackletIDs();
    bool drawingAnnotationInfo = GUIState::getInstance()->getDrawAnnotationInfo();
    bool drawingCutLine = GUIState::getInstance()->getDrawCutLine();

    if (drawingOutlines)
        drawOutlines(newImage, frame, scaleFactor);
    if (drawingTrackletIDs || drawingAnnotationInfo)
        drawObjectInfo(newImage, frame, scaleFactor, drawingTrackletIDs, drawingAnnotationInfo);
    if (drawingCutLine)
        drawCutLine(newImage);

    size->setHeight(newImage.height());
    size->setWidth(newImage.width());

    return newImage;
}
