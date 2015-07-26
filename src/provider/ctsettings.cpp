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

CTSettings::CTSettings() : QSettings("IMB", "CellTracker")
{
}

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

    instance->sync();
}

QVariant CTSettings::value(const QString &key, const QVariant &defaultValue){
    if (!instance)
        getInstance();

    if (!instance->contains(key))
        qDebug() << "uncontained value" << key << "please add a default value in " << __FILE__;
    return instance->QSettings::value(key,defaultValue);
}

CTSettings *CTSettings::getInstance() {
    if (!instance)
        instance = new CTSettings();
    instance->setDefaults();
    return instance;
}

QObject *CTSettings::qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return getInstance();
}
}
