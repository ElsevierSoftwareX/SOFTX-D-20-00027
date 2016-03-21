#ifndef MERGE_H
#define MERGE_H

#include <QPolygonF>

class Merge
{
public:
    Merge();
    static QPolygonF &compute(QPolygonF&, QPolygonF&);
};

#endif // MERGE_H
