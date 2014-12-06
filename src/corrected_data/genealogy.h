#ifndef GENEALOGY_H
#define GENEALOGY_H

#include <QList>

#include "trackelement.h"
#include "annotation.h"

/*!
 * \brief The Genealogy class
 *
 * This Class represents the generated genealogy. It holds the roots of all the
 * trees generated.
 */
class Genealogy
{
public:
    Genealogy();

private:
    QList<TrackElement> roots;
    QList<Annotation> annotations;
};

#endif // GENEALOGY_H
