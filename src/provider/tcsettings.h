/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2017, 2016, 2015 Sebastian Wagner
 *
 * TraCurate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TraCurate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with TraCurate.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef TCSETTINGS_H
#define TCSETTINGS_H

#include <tuple>

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include <QSettings>

namespace TraCurate {
class TCOption : public QObject {
    Q_OBJECT
public:
    TCOption() : QObject(0) {}
    TCOption(QString name_, QString type_, bool modifiable_, QString cName_, QString desc_)
        : QObject(0), name(name_), type(type_), modifiable(modifiable_), cName(cName_), desc(desc_) {}
    ~TCOption() {}

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
 * \brief The TCSettings class
 *
 * The TCSettings-Class provides access to configuration values permanently
 * stored on the users system. Via setDefaults() it also provides the means
 * to set default values to be used if those values are absent on the users
 * system.
 *
 * Values stored using TCSettings may be accessed either from C++ or from QML.
 */
class TCSettings : public QSettings
{
    Q_OBJECT
public:
    static TCSettings *getInstance();
    static QObject *qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

    Q_INVOKABLE static QVariant value(const QString &key, const QVariant &defaultValue = QVariant());
    Q_INVOKABLE static void setValue(const QString &key, const QVariant &value);

    Q_INVOKABLE QList<QObject *> getConfiguration();

private:
    explicit TCSettings();
    ~TCSettings() { for (QObject *o : options) delete o; options.clear(); }

    void setDefaults();
    static TCSettings *instance;
    QList<QObject *> options;
};

}
#endif // TCSETTINGS_H
