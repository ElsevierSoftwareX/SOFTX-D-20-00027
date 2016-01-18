#include "ctsettings.h"

#include <QColor>
#include <QDebug>

namespace CellTracker {

#define CT_CONFIG_VERSION 1

#define setDefault(name, val) \
    { \
        if (!instance->contains((name)) || (instance->value("version").toInt() < CT_CONFIG_VERSION)) \
                instance->setValue((name), (val)); \
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
    setDefault("version", CT_CONFIG_VERSION);
    setDefault("drawing/default_cell", QColor(Qt::white));
    setDefault("drawing/selected_cell", QColor(Qt::red));
    setDefault("drawing/active_cell", QColor(Qt::green));
    setDefault("drawing/finished_cell", QColor(Qt::yellow));
    setDefault("drawing/merge_cell", QColor(Qt::blue));
    setDefault("drawing/selected_track", QColor(255, 127, 0));
    setDefault("drawing/cell_opacity", 0.5);
    setDefault("drawing/unselected_linecolor", QColor(Qt::black));
    setDefault("drawing/selected_linecolor", QColor(Qt::red));
    setDefault("drawing/selected_linewidth", 4);
    setDefault("drawing/default_linewidth", 2);
    setDefault("text/status_fontsize", 12);
    setDefault("text/trackid_fontsize", 12);
    setDefault("strategies/show_val", 5);
    setDefault("strategies/delay_val", 300);

    instance->sync();
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

/*!
 * \brief returns an instance of CTSettings
 * \return an instance of CTSettings
 */
CTSettings *CTSettings::getInstance() {
    if (!instance)
        instance = new CTSettings();
    instance->setDefaults();
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
