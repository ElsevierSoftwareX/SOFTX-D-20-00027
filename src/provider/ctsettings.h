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
    CTOption(QString name, bool modifiable, QString cName, QString desc)
        : QObject(0), name(name), modifiable(modifiable), cName(cName), desc(desc) {}
    ~CTOption() {}

    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    QString getName() { return name; }
    void setName(QString val) { if (name != val) emit nameChanged(name = val); }

    Q_PROPERTY(bool modifiable READ getModifiable WRITE setModifiable NOTIFY modifiableChanged)
    bool getModifiable() { return modifiable; }
    void setModifiable(bool val) { if (modifiable != val) emit modifiableChanged(modifiable = val); }

    Q_PROPERTY(QString cName READ getCName WRITE setCName NOTIFY cNameChanged)
    QString getCName() { return cName; }
    void setCName(QString val) { if (cName != val) emit cNameChanged(cName = val); }

    Q_PROPERTY(QString desc READ getDesc WRITE setDesc NOTIFY descChanged)
    QString getDesc() { return desc; }
    void setDesc(QString val) { if (desc != val) emit descChanged(desc = val); }

signals:
    void nameChanged(QString);
    void modifiableChanged(bool);
    void cNameChanged(QString);
    void descChanged(QString);
private:
    QString name;
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
    void setDefaults();
    static CTSettings *instance;
    QList<QObject *> options;
};

}
#endif // CTSETTINGS_H
