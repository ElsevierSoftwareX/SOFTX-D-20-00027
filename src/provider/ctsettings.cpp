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
    setDefault("tracking/display/cell/default", QColor(Qt::white));
    setDefault("tracking/display/cell/active", QColor(Qt::green));
    setDefault("tracking/display/cell/finished", QColor(Qt::yellow));
    setDefault("tracking/display/cell/merge", QColor(Qt::blue));
    setDefault("tracking/display/cell/border", QColor(Qt::black));
    setDefault("tracking/display/cell/opacity", 0.5);

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

}
