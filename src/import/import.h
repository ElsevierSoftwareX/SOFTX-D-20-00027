#ifndef IMPORT_H
#define IMPORT_H

#include <QString>
#include "../project.h"

#define DEFAULT_CHANNEL 0
#define DEFAULT_SLICE 0

class Import
{
public:
    Import();
    virtual std::shared_ptr<Project> load(QString) = 0;
};

#endif // IMPORT_H
