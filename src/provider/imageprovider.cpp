#include <QtDebug>
#include <QPainter>

#include "imageprovider.h"
#include "graphics/base.h"
#include "graphics/merge.h"
#include "graphics/separate.h"
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
#include "version.h"

#ifndef GIT_REVISION
#define GIT_REVISION "unknown"
#endif
#ifndef GIT_COMMIT
#define GIT_COMMIT "unknown"
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
bool ImageProvider::cellIsSelected(std::shared_ptr<Object> const &o) {
    std::shared_ptr<Object> selected = GUIState::getInstance()->getSelectedCell().lock();

    return (selected
            && selected->getId() == o->getId()
            && selected->getFrameId() == o->getFrameId());
}

/*!
 * \brief tells, if the AutoTracklet of a given Object is selected
 * \param o the Object to check
 * \return true if the Object%s AutoTracklet is selected, false if not
 */
bool ImageProvider::cellAutoTrackletIsSelected(std::shared_ptr<Object> const &o) {
    std::shared_ptr<AutoTracklet> selected = GUIState::getInstance()->getSelectedAutoTrack().lock();
    return (selected
            && selected->getID() >= 0
            && (uint32_t)selected->getID() == o->getAutoId());
}

/*!
 * \brief tells, if the Object is hovered
 * \param o the Object to check
 * \return true if it is hovered, false if not
 */
bool ImageProvider::cellIsHovered(std::shared_ptr<Object> const &o) {
    std::shared_ptr<Object> hovered = GUIState::getInstance()->getHoveredCell().lock();

    return (hovered
            && hovered->getId() == o->getId()
            && hovered->getFrameId() == o->getFrameId());
}

/*!
 * \brief tells, if the given Object is the beginning of a daughter track of the currently selected Tracklet
 * \param daughter the Object to check
 * \return true if it is, false otherwise
 */
bool ImageProvider::cellIsInDaughters(std::shared_ptr<Object> const &daughter) {
    std::shared_ptr<Tracklet> t = GUIState::getInstance()->getSelectedTrack().lock();

    if(t && t->getNext() && t->getNext()->getType() == TrackEvent<Tracklet>::EVENT_TYPE_DIVISION) {
        std::shared_ptr<TrackEventDivision<Tracklet>> ev = std::static_pointer_cast<TrackEventDivision<Tracklet>>(t->getNext());
        for (std::weak_ptr<Tracklet> dt: *ev->getNext()) {
            std::shared_ptr<Tracklet> daughterT = dt.lock();
            if (!daughterT)
                continue;
            if (daughterT->getStart().second == daughter)
                return true;
        }
    }

    return false;
}

/*!
 * \brief tells, if a given Object is in a Tracklet
 * \param o the Object to check
 * \return true if the Object is in a Tracklet, false otherwise
 */
bool ImageProvider::cellIsInTracklet(std::shared_ptr<Object> const &o) {
    return o->isInTracklet();
}

/*!
 * \brief returns the line color for a given object
 * \param o the Object whose line color should be returned
 * \return the line color for this object
 */
QColor ImageProvider::getCellLineColor(std::shared_ptr<Object> const &o) {
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
qreal ImageProvider::getCellLineWidth(std::shared_ptr<Object> const &o) {
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
bool ImageProvider::cellIsRelated(std::shared_ptr<Object> const &o) {
    std::shared_ptr<Tracklet> selected = GUIState::getInstance()->getSelectedTrack().lock();
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

        if (!currTracklet)
            continue;

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
                std::shared_ptr<Tracklet> prev = ev->getPrev().lock();
                std::shared_ptr<QList<std::weak_ptr<Tracklet>>> next = ev->getNext();
                if (prev && !openList.contains(prev) && !closedList.contains(prev))
                    openList.push_back(prev);
                for (std::weak_ptr<Tracklet> t: *next)
                    if (!openList.contains(t.lock()) && !closedList.contains(t.lock()))
                        openList.push_back(t.lock());
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_MERGE: {
                std::shared_ptr<TrackEventMerge<Tracklet>> ev = std::static_pointer_cast<TrackEventMerge<Tracklet>>(currEv);
                std::shared_ptr<QList<std::weak_ptr<Tracklet>>> prev = ev->getPrev();
                std::shared_ptr<Tracklet> next = ev->getNext().lock();
                for (std::weak_ptr<Tracklet> t: *prev)
                    if (!openList.contains(t.lock()) && !closedList.contains(t.lock()))
                        openList.push_back(t.lock());
                if (next && !openList.contains(next) && !closedList.contains(next))
                    openList.push_back(next);
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_UNMERGE: {
                std::shared_ptr<TrackEventUnmerge<Tracklet>> ev = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(currEv);
                std::shared_ptr<Tracklet> prev = ev->getPrev().lock();
                std::shared_ptr<QList<std::weak_ptr<Tracklet>>> next = ev->getNext();
                if (prev && !openList.contains(prev) && !closedList.contains(prev))
                    openList.push_back(prev);
                for (std::weak_ptr<Tracklet> t: *next)
                    if (!openList.contains(t.lock()) && !closedList.contains(t.lock()))
                        openList.push_back(t.lock());
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_LOST: {
                std::shared_ptr<TrackEventLost<Tracklet>> ev = std::static_pointer_cast<TrackEventLost<Tracklet>>(currEv);
                std::shared_ptr<Tracklet> prev = ev->getPrev().lock();
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
Qt::BrushStyle ImageProvider::getCellBrushStyle(std::shared_ptr<Object> const &o, QPolygonF const &outline, QPointF const &mousePos)
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
QColor ImageProvider::getCellBgColor(std::shared_ptr<Object> const &o)
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

    painter.setOpacity(CTSettings::value("drawing/cell_opacity").toReal());
    painter.setBrush(brush);

    /* XXX: this is undefined behaviour if the polygon isn't actually convex */
    painter.drawConvexPolygon(poly);
}

/*!
 * \brief draws all Outlines of Objects in the given Frame
 * \param image the Image to draw to
 * \param frame the current Frame
 * \param scaleFactor the scaleFactor to use
 */
void ImageProvider::drawOutlines(QImage &image, int frame, double scaleFactor, bool regular, bool separation, bool aggregation) {
    /* set up painting equipment */
    QPainter painter(&image);
    QPainter::RenderHints rh = 0;
    painter.setRenderHints(rh);
    GUIState *gs = GUIState::getInstance();

    if (!painter.isActive())
        return;

    std::shared_ptr<Project> proj = gs->getProj();
    if (!proj)
        return;

    /* collect the polygons we want to draw */
    QList<std::shared_ptr<Object>> allObjects;

    if (regular)
        for (std::shared_ptr<Slice> s : proj->getMovie()->getFrame(frame)->getSlices())
            for (std::shared_ptr<Channel> c : s->getChannels().values())
                allObjects.append(c->getObjects().values());

    QList<QPolygonF> addObjects;
    QPointF start(gs->getStartX(), gs->getStartY());
    QPointF end(gs->getEndX(), gs->getEndY());
    if (separation) {
        QLineF line(start, end);
        /* find object to remove */
        std::shared_ptr<Object> cuttee = Base::objectCutByLine(line);

        if (cuttee) {
            /* calculate new objects */
            QLineF cutLine(start/scaleFactor, end/scaleFactor);
            auto newOutlines = Separate::compute(*cuttee->getOutline(), cutLine);
            if (!newOutlines.first.isEmpty() && !newOutlines.second.isEmpty()) {
                addObjects.append(newOutlines.first);
                addObjects.append(newOutlines.second);
                allObjects.removeAll(cuttee);
            }
        }
    }

    if (aggregation) {
        /* find objects to remove */
        std::shared_ptr<Object> first = DataProvider::getInstance()->cellAt(start.x(), start.y());
        std::shared_ptr<Object> second = DataProvider::getInstance()->cellAt(end.x(), end.y());

        if (first && second) {
            /* calculate new obejct */
            QPolygonF merge = Merge::compute(*first->getOutline(), *second->getOutline());
            if (!merge.isEmpty()) {
                addObjects.append(merge);
                allObjects.removeAll(first);
                allObjects.removeAll(second);
            }
        } else {
            if (first) {
                addObjects.append(*first->getOutline());
                allObjects.removeAll(first);
            }
            if (second) {
                addObjects.append(*second->getOutline());
                allObjects.removeAll(second);
            }
        }
    }

    /* the transformation to apply to the points of the polygons */
    QTransform trans;
    trans = trans.scale(scaleFactor, scaleFactor);

    for (std::shared_ptr<Object> o : allObjects) {
        QPolygonF curr = trans.map(*o->getOutline());

        QPointF mousePos(gs->getMouseX(),
                         gs->getMouseY());

        QColor lineColor = getCellLineColor(o);
        qreal lineWidth = getCellLineWidth(o);

        QPen pen(lineColor, lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);

        QColor bgColor = getCellBgColor(o);
        Qt::BrushStyle bStyle = getCellBrushStyle(o, curr, mousePos);
        drawPolygon(painter, curr, bgColor, bStyle);
    }

    for (QPolygonF &p : addObjects) {
        QPolygonF curr = trans.map(p);
        QPen pen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);

        drawPolygon(painter, curr, Qt::white, Qt::SolidPattern);
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
    QImage objectAnnotationImage;
    QImage trackletAnnotationImage;
    GUIState *gs = GUIState::getInstance();

    if (drawAnnotationInfo) {
        objectAnnotationImage = QImage(":/icons/object_annotation.svg");
        trackletAnnotationImage = QImage(":/icons/tracklet_annotation.svg");
    }

    if (!drawTrackletIDs && !drawAnnotationInfo)
        return;

    /* set up painting equipment */
    QPainter painter(&image);
    if (!painter.isActive())
        return;
    QPainter::RenderHints rh = 0;
    painter.setRenderHints(rh);

    std::shared_ptr<Project> proj = gs->getProj();
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
            std::shared_ptr<Tracklet> t = gs->getProj()->getGenealogy()->getTracklet(o->getTrackId());
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
    painter.drawText(QRect(w-500,0,500,20),
                     "version: " + QString(GIT_COMMIT) +
                     " (rev " + QString(GIT_REVISION) +
                     " " + QString(GIT_BRANCH) + ")" ,
                     QTextOption(Qt::AlignRight|Qt::AlignBottom));

    return defaultImage;
}

void ImageProvider::drawCutLine(QImage &image) {
    GUIState *gs = GUIState::getInstance();
    int startX, startY, endX, endY;

    startX = gs->getStartX();
    startY = gs->getStartY();
    if (gs->getDrawSeparation()) {
        endX = gs->getEndX();
        endY = gs->getEndY();
    } else {
        endX = gs->getMouseX();
        endY = gs->getMouseY();
    }

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
    GUIState *gs = GUIState::getInstance();

    int frame = gs->getCurrentFrame();
    QString path = gs->getProjPath();

    if (requestedSize.height() <= 0 || requestedSize.width() <= 0)
        return defaultImage(size);
    if (path.isEmpty() || frame < 0 || frame > gs->getMaximumFrame())
        return defaultImage(size, requestedSize);

    /* some caching, so we don't always re-request the image */
    if (frame == cachedFrame && path == cachedPath) {
        newImage = cachedImage;
    } else {
        QImage tmpImage = DataProvider::getInstance()->requestImage(path, frame);
        /* Image may be imported in another format, so convert it to ARGB32 for drawing in color on it */
        newImage = tmpImage.convertToFormat(QImage::Format_ARGB32_Premultiplied);
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
    bool drawingOutlines       = gs->getDrawOutlines();
    bool drawingTrackletIDs    = gs->getDrawTrackletIDs();
    bool drawingAnnotationInfo = gs->getDrawAnnotationInfo();
    bool drawingCutLine        = gs->getDrawCutLine();
    bool drawingSeparation     = gs->getDrawSeparation();
    bool drawingAggregation    = gs->getDrawAggregation();

    if (drawingOutlines || drawingAggregation || drawingSeparation)
        drawOutlines(newImage, frame, scaleFactor, drawingOutlines, drawingSeparation, drawingAggregation);
    if (drawingTrackletIDs || drawingAnnotationInfo)
        drawObjectInfo(newImage, frame, scaleFactor, drawingTrackletIDs, drawingAnnotationInfo);
    if (drawingCutLine)
        drawCutLine(newImage);

    size->setHeight(newImage.height());
    size->setWidth(newImage.width());

    return newImage;
}
