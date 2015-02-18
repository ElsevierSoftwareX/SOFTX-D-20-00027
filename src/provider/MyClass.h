#ifndef MYCLASS_H
#define MYCLASS_H

#include <QObject>
#include <QString>
#include <QUrl>

#include "src/import/importhdf5.h"

class MyClass : public QObject
{
    Q_OBJECT
    //Q_PROPERTY(QString myProperty READ prop NOTIFY propChanged)

public:
    //MyClass(QObject * parent = 0) : QObject(parent) {}
    //QString prop() { return QString("Hello from MyClass"); }
    Q_INVOKABLE void loadHDF5(QString fileName);
    Q_INVOKABLE QImage requestImage(QString fileName, int imageNumber);

private:
    CellTracker::ImportHDF5 MyImport;
};

#endif // MYCLASS_H
