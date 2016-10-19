#include "ctsettings.h"

#include <QColor>
#include <QDebug>

namespace CellTracker {

#define CT_CONFIG_VERSION 1

#define setDefault(name, type, val, changeable, cname, desc)                                               \
    {                                                                                                \
        if (!instance->contains((name)) || (instance->value("version").toInt() < CT_CONFIG_VERSION)) \
                instance->QSettings::setValue((name), (val));                                        \
        CTOption *o = new CTOption(name, type, changeable, cname, desc);                             \
        options.push_back(o);                                                                        \
    }

CTSettings *CTSettings::instance = nullptr;

/*!
 * \brief constructor for CTSettings
 *
 * This constructor is private, please use CellTracker::getInstance to obtain an instance of CTSettings
 */
CTSettings::CTSettings() :
    QSettings("IMB", "CellTracker") {}

/*!
 * \brief sets the default values for configuration variables
 *
 * to add a new dafault value, use the setDefault-macro
 */
void CTSettings::setDefaults(){
    setDefault("version", "number", CT_CONFIG_VERSION, false,
               "Version",
               "Version of the CellTracker-Configuration");
    setDefault("drawing/default_cell", "color", QColor(Qt::white), true,
               "Default Cell Fill Color",
               "Default Color for Cells without any special properties");
    setDefault("drawing/selected_cell", "color", QColor(Qt::red), true,
               "Selected Cell Fill Color",
               "Color for Cells that are selected (by hovering)");
    setDefault("drawing/active_cell", "color", QColor(Qt::green), true,
               "Active Cell Fill Color",
               "Color for Cells that are active (by clicking on it)");
    setDefault("drawing/finished_cell", "color", QColor(Qt::yellow), true,
               "Finished Cell Fill Color",
               "Color for Cells that are already assigned to a Tracklet");
    setDefault("drawing/merge_cell", "color", QColor(Qt::blue), true,
               "Merge Cell Fill Color",
               "Color for Cells that are daughter cells of the selected Cell");
    setDefault("drawing/selected_track", "color", QColor(255, 127, 0), true,
               "Selected Tracklet Fill Color",
               "Color for cells that are in the currently selected tracklet");
    setDefault("drawing/cell_opacity", "percent", 0.5, true,
               "Cell Fill Opacity",
               "Opacity used for the Cells");
    setDefault("drawing/unselected_linecolor", "color", QColor(Qt::black), true,
               "Unselected Cell Linecolor",
               "Linecolor for Cells that are not selected");
    setDefault("drawing/selected_linecolor", "color", QColor(Qt::red), true,
               "Selected Cell Linecolor",
               "Linecolor for the selected Cell");
    setDefault("drawing/selected_linewidth", "number", 4, true,
               "Selected Linewidth",
               "Linewidth used for drawing the selected Cell");
    setDefault("drawing/default_linewidth", "number", 2, true,
               "Default Linewidth",
               "Linewidth used for drawing Cells that are not selected");
    setDefault("text/status_fontsize", "number", 12, true,
               "Default Font Size",
               "Default Font Size");
    setDefault("text/trackid_fontsize", "number", 12, true,
               "TrackID Font Size",
               "Font Size used for drawing the TrackIDs");
    setDefault("strategies/show_val", "number", 5, true,
               "Default Frames to Show",
               "Number of Frames to show for strategies");
    setDefault("strategies/delay_val", "number", 300, true,
               "Default Frame Delay",
               "Delay in ms between Frames when using Strategies");
    setDefault("scrolling/scroll_factor_x", "number", -30, true,
               "Scroll Factor on X-Axis",
               "How many pixel are scrolled by one keypress (negative values accepted)");
    setDefault("scrolling/scroll_factor_y", "number", -30, true,
               "Scroll Factor on Y-Axis",
               "How many pixel are scrolled by one keypress (negative values accepted)");
    setDefault("text/trackid_color", "color", QColor(Qt::black), true,
               "TrackID Color",
               "Color to use for drawing the TrackID");
    setDefault("graphics/max_pixelmask_percentage", "percent", 0.25, true,
               "Maximum Pixelmask Percentage",
               "The maximum area (relative to image) a pixelmask may fill when using FloodFill");
    instance->sync();
}

QList<QObject *> CTSettings::getConfiguration() {
    if (!instance)
        getInstance();

    return options;
}

/*!
 * \brief returns the value for a given key from the configuration
 * \param key the key for the configuration variable
 * \param defaultValue default parameter to use if the key was not found
 * \return the value for the key or defaultValue, if none was found
 *
 * Also prints a message, if a key was requested, that is not yet contained.
 */
QVariant CTSettings::value(const QString &key, const QVariant &defaultValue){
    if (!instance)
        getInstance();

    if (!instance->contains(key))
        qDebug() << "uncontained value" << key << "please add a default value in " << __FILE__;
    return instance->QSettings::value(key,defaultValue);
}

void CTSettings::setValue(const QString &key, const QVariant &value) {
    if (!instance)
        getInstance();

    if (!instance->contains(key))
        qDebug() << "uncontained value" << key << "please add a default value in " << __FILE__;

    return instance->QSettings::setValue(key, value);
}

/*!
 * \brief returns an instance of CTSettings
 * \return an instance of CTSettings
 */
CTSettings *CTSettings::getInstance() {
    if (!instance) {
        instance = new CTSettings();
        instance->setDefaults();
    }
    return instance;
}

/*!
 * \brief provides an instance of CTSettings for use in QML
 * \param engine (unused)
 * \param scriptEngine (unused)
 * \return a pointer to the instance of CTSettings
 */
QObject *CTSettings::qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return getInstance();
}
}
