#ifndef CTSETTINGS_H
#define CTSETTINGS_H

#include <QSettings>

namespace CellTracker {

class CTSettings : public QSettings
{
public:
    static CTSettings *getInstance();
    static QVariant value(const QString &key, const QVariant &defaultValue = QVariant());

private:
    CTSettings();
    void setDefaults();
    static CTSettings *instance;
};

}
#endif // CTSETTINGS_H
