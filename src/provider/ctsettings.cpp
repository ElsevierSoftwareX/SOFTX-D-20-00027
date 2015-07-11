#include "ctsettings.h"

#include <QColor>
#include <QDebug>

namespace CellTracker {

#define setDefault(name, value) \
    { \
        if (!instance->contains((name))) \
                instance->setValue((name), (value)); \
    }

CTSettings *CTSettings::instance = nullptr;

CTSettings::CTSettings() : QSettings("IMB", "CellTracker")
{
}

void CTSettings::setDefaults(){
    setDefault("colors/default_cell", QColor(Qt::white));
    setDefault("colors/active_cell", QColor(Qt::green));
    setDefault("colors/finished_cell", QColor(Qt::yellow));
    setDefault("colors/merge_cell", QColor(Qt::blue));
    setDefault("colors/cell_opacity", 0.5);
    setDefault("colors/unselected_linecolor", QColor(Qt::black));
    setDefault("colors/selected_linecolor", QColor(Qt::red));
    setDefault("text/status_size", 12);

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
