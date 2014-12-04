#ifndef GENEALOGY_H
#define GENEALOGY_H

#include <QList>

#include "trackelement.h"
#include "annotation.h"

class Genealogy
{
public:
    Genealogy();

private:
    QList<TrackElement> roots;
    QList<Annotation> annotations;
};

#endif // GENEALOGY_H
