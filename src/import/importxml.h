#ifndef IMPORTXML_H
#define IMPORTXML_H


#include "import.h"


class ImportXML : public Import
{
public:
    ImportXML();
    std::shared_ptr<Movie> load(QString);
};

#endif // IMPORTXML_H
