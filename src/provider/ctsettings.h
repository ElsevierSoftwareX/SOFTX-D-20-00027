#ifndef CTSETTINGS_H
#define CTSETTINGS_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include <QSettings>

namespace CellTracker {

/*!
 * \brief The CTSettings class
 *
 * The CTSettings-Class provides access to configuration values permanently
 * stored on the users system. Via setDefaults() it also provides the means
 * to set default values to be used it those values are absent on the users
 * system.
 *
 * Values stored using CTSettings may be accessed either from C++ or from QML.
 */
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
