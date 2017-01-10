#ifndef CTSETTINGS_H
#define CTSETTINGS_H

#include <tuple>

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include <QSettings>

namespace CellTracker {
class CTOption : public QObject {
    Q_OBJECT
public:
    CTOption() : QObject(0) {}
    CTOption(QString name_, QString type_, bool modifiable_, QString cName_, QString desc_)
        : QObject(0), name(name_), type(type_), modifiable(modifiable_), cName(cName_), desc(desc_) {}
    ~CTOption() {}

    Q_PROPERTY(QString name READ getName NOTIFY nameChanged)
    QString getName() { return name; }

    Q_PROPERTY(QString type READ getType NOTIFY typeChanged)
    QString getType() { return type; }

    Q_PROPERTY(bool modifiable READ getModifiable NOTIFY modifiableChanged)
    bool getModifiable() { return modifiable; }

    Q_PROPERTY(QString cName READ getCName NOTIFY cNameChanged)
    QString getCName() { return cName; }

    Q_PROPERTY(QString desc READ getDesc NOTIFY descChanged)
    QString getDesc() { return desc; }

signals:
    void nameChanged(QString);
    void typeChanged(QString);
    void modifiableChanged(bool);
    void cNameChanged(QString);
    void descChanged(QString);
private:
    QString name;
    QString type;
    bool modifiable;
    QString cName;
    QString desc;
};

/*!
 * \brief The CTSettings class
 *
 * The CTSettings-Class provides access to configuration values permanently
 * stored on the users system. Via setDefaults() it also provides the means
 * to set default values to be used if those values are absent on the users
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
    Q_INVOKABLE static void setValue(const QString &key, const QVariant &value);

    Q_INVOKABLE QList<QObject *> getConfiguration();

private:
    explicit CTSettings();
    ~CTSettings() { for (QObject *o : options) delete o; options.clear(); }

    void setDefaults();
    static CTSettings *instance;
    QList<QObject *> options;
};

}
#endif // CTSETTINGS_H
