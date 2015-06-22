#ifndef CTSETTINGS_H
#define CTSETTINGS_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include <QSettings>

namespace CellTracker {

class CTSettings : public QSettings
{
    Q_OBJECT
public:
    static CTSettings *getInstance();
    static QObject *qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine);
    Q_INVOKABLE static QVariant value(const QString &key, const QVariant &defaultValue = QVariant());

private:
    explicit CTSettings();
    void setDefaults();
    static CTSettings *instance;
};

}
#endif // CTSETTINGS_H
