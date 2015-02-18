#include <QtDebug>

#include "MyClass.h"

void MyClass::loadHDF5(QString fileName)
{
    QUrl url(fileName);
    std::shared_ptr<CellTracker::Project> proj;
    proj = MyImport.load(url.toLocalFile());
    //std::cerr << *proj;
}

QImage MyClass::requestImage(QString fileName, int imageNumber)
{
    QUrl url(fileName);
    std::shared_ptr<QImage> img;
    img = MyImport.requestImage(url.toLocalFile(), imageNumber, 0, 0);
    return *img.get();
}
