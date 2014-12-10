#ifndef IMPORT_H
#define IMPORT_H

#include <QString>
#include "../base_data/movie.h"

class Import
{
public:
    Import();
    virtual Movie *load(QString) = 0;
};

#endif // IMPORT_H
