#include "examples.h"

#include <QDebug>
#include <QList>
#include <QPoint>
#include <QSet>

#include "graphics/floodfill.h"

static uint qHash(const QPoint &val) {
    return qHash(QPair<int, int>(val.x(), val.y()));
}

void exampleTestFloodFill() {
//    QSet<QPoint> a = {QPoint(182,98), QPoint(183,98), QPoint(182,99), QPoint(190,104),
//                   QPoint(183,99), QPoint(182,96), QPoint(183,96), QPoint(191,104),
//                   QPoint(190,105), QPoint(181,98), QPoint(182,97), QPoint(183,97),
//                   QPoint(191,105), QPoint(180,96), QPoint(189,104), QPoint(181,96),
//                   QPoint(181,97), QPoint(190,98), QPoint(191,98), QPoint(190,99),
//                   QPoint(191,99), QPoint(188,98), QPoint(190,96), QPoint(191,96),
//                   QPoint(188,99), QPoint(189,98), QPoint(190,97), QPoint(186,98),
//                   QPoint(191,97), QPoint(188,96), QPoint(189,99), QPoint(190,102),
//                   QPoint(186,99), QPoint(191,102), QPoint(187,98), QPoint(188,97),
//                   QPoint(189,96), QPoint(190,103), QPoint(186,96), QPoint(191,103),
//                   QPoint(187,99), QPoint(188,102), QPoint(184,98), QPoint(190,100),
//                   QPoint(189,97), QPoint(186,97), QPoint(191,100), QPoint(187,96),
//                   QPoint(188,103), QPoint(184,99), QPoint(185,98), QPoint(190,101),
//                   QPoint(189,102), QPoint(186,102), QPoint(191,101), QPoint(187,97),
//                   QPoint(188,100), QPoint(184,96), QPoint(185,99), QPoint(189,103),
//                   QPoint(187,102), QPoint(188,101), QPoint(184,97), QPoint(185,96),
//                   QPoint(189,100), QPoint(186,100), QPoint(189,101), QPoint(185,97),
//                   QPoint(187,103), QPoint(186,101), QPoint(187,100), QPoint(187,101),
//                   QPoint(185,100), QPoint(182,90), QPoint(182,91), QPoint(183,91),
//                   QPoint(180,90), QPoint(180,91), QPoint(181,90), QPoint(182,94),
//                   QPoint(178,90), QPoint(181,91), QPoint(182,95), QPoint(178,91),
//                   QPoint(183,94), QPoint(179,90), QPoint(182,92), QPoint(183,95),
//                   QPoint(179,91), QPoint(180,94), QPoint(182,93), QPoint(178,89),
//                   QPoint(181,94), QPoint(183,92), QPoint(180,95), QPoint(177,90),
//                   QPoint(178,94), QPoint(181,95), QPoint(183,93), QPoint(179,89),
//                   QPoint(180,92), QPoint(177,91), QPoint(181,92), QPoint(179,94),
//                   QPoint(180,93), QPoint(178,92), QPoint(179,95), QPoint(181,93),
//                   QPoint(177,89), QPoint(178,93), QPoint(179,92), QPoint(179,93),
//                   QPoint(190,95), QPoint(188,94), QPoint(188,95), QPoint(184,91),
//                   QPoint(189,95), QPoint(186,94), QPoint(186,95), QPoint(187,94),
//                   QPoint(187,95), QPoint(184,94), QPoint(185,94), QPoint(186,93),
//                   QPoint(184,95), QPoint(185,95), QPoint(187,93), QPoint(184,92),
//                   QPoint(185,92), QPoint(184,93), QPoint(185,93), QPoint(198,106),
//                   QPoint(198,107), QPoint(199,106), QPoint(196,106), QPoint(198,104),
//                   QPoint(199,107), QPoint(196,107), QPoint(197,106), QPoint(198,105),
//                   QPoint(196,104), QPoint(197,107), QPoint(194,106), QPoint(195,106),
//                   QPoint(196,105), QPoint(197,104), QPoint(194,107), QPoint(195,107),
//                   QPoint(192,106), QPoint(197,105), QPoint(194,104), QPoint(198,108),
//                   QPoint(195,104), QPoint(194,105), QPoint(193,106), QPoint(198,109),
//                   QPoint(199,108), QPoint(195,105), QPoint(196,108), QPoint(192,104),
//                   QPoint(199,109), QPoint(192,105), QPoint(197,108), QPoint(193,104),
//                   QPoint(193,105), QPoint(194,99), QPoint(196,102), QPoint(192,98),
//                   QPoint(196,103), QPoint(192,99), QPoint(193,98), QPoint(197,103),
//                   QPoint(194,102), QPoint(193,99), QPoint(202,110), QPoint(195,102),
//                   QPoint(194,103), QPoint(192,97), QPoint(202,111), QPoint(195,103),
//                   QPoint(192,102), QPoint(194,100), QPoint(203,111), QPoint(195,100),
//                   QPoint(192,103), QPoint(193,102), QPoint(194,101), QPoint(201,110),
//                   QPoint(195,101), QPoint(193,103), QPoint(192,100), QPoint(201,111),
//                   QPoint(193,100), QPoint(192,101), QPoint(200,109), QPoint(193,101),
//                   QPoint(202,112), QPoint(203,112)};

//    QSet<QPoint> a = {QPoint(190,95), QPoint(189,94), QPoint(188,94), QPoint(189,95), QPoint(187,93), QPoint(186,92), QPoint(188,95), QPoint(187,94), QPoint(186,93), QPoint(185,92), QPoint(184,92), QPoint(185,93), QPoint(187,95), QPoint(186,94), QPoint(184,93), QPoint(183,92), QPoint(185,94), QPoint(186,95), QPoint(184,94), QPoint(183,93), QPoint(182,92), QPoint(185,95), QPoint(181,92), QPoint(184,95), QPoint(183,94), QPoint(182,93), QPoint(181,93), QPoint(183,95), QPoint(182,94), QPoint(180,92), QPoint(181,94), QPoint(182,95), QPoint(180,93), QPoint(179,92), QPoint(181,95), QPoint(180,94), QPoint(178,92), QPoint(179,93), QPoint(184,91), QPoint(180,95), QPoint(178,93), QPoint(177,92), QPoint(179,94), QPoint(183,91), QPoint(182,90), QPoint(178,94), QPoint(179,95), QPoint(182,91), QPoint(181,90), QPoint(180,89), QPoint(181,91), QPoint(180,90), QPoint(179,89), QPoint(180,91), QPoint(178,89), QPoint(177,88), QPoint(179,90), QPoint(178,90), QPoint(177,89), QPoint(176,88), QPoint(179,91), QPoint(178,91), QPoint(177,90), QPoint(176,89), QPoint(175,88), QPoint(177,91), QPoint(176,90), QPoint(191,100), QPoint(191,101), QPoint(190,100), QPoint(191,102), QPoint(190,101), QPoint(189,100), QPoint(189,101), QPoint(188,100), QPoint(191,103), QPoint(190,102), QPoint(189,102), QPoint(188,101), QPoint(191,96), QPoint(187,100), QPoint(190,103), QPoint(189,103), QPoint(188,102), QPoint(191,97), QPoint(187,101), QPoint(186,100), QPoint(190,96), QPoint(189,96), QPoint(185,100), QPoint(188,103), QPoint(191,98), QPoint(187,102), QPoint(186,101), QPoint(190,97), QPoint(186,102), QPoint(189,97), QPoint(185,101), QPoint(188,96), QPoint(191,99), QPoint(187,103), QPoint(184,100), QPoint(190,98), QPoint(189,98), QPoint(188,97), QPoint(187,96), QPoint(190,99), QPoint(186,96), QPoint(189,99), QPoint(188,98), QPoint(187,97), QPoint(186,97), QPoint(188,99), QPoint(187,98), QPoint(185,96), QPoint(186,98), QPoint(187,99), QPoint(185,97), QPoint(184,96), QPoint(186,99), QPoint(185,98), QPoint(184,97), QPoint(191,104), QPoint(183,96), QPoint(182,96), QPoint(190,104), QPoint(185,99), QPoint(184,98), QPoint(191,105), QPoint(183,97), QPoint(182,97), QPoint(189,104), QPoint(181,96), QPoint(190,105), QPoint(184,99), QPoint(183,98), QPoint(182,98), QPoint(181,97), QPoint(188,104), QPoint(180,96), QPoint(183,99), QPoint(182,99), QPoint(181,98), QPoint(180,97), QPoint(204,112), QPoint(203,112), QPoint(202,112), QPoint(195,100), QPoint(197,102), QPoint(196,101), QPoint(197,103), QPoint(196,102), QPoint(195,101), QPoint(194,100), QPoint(193,100), QPoint(196,103), QPoint(195,102), QPoint(194,101), QPoint(203,110), QPoint(204,111), QPoint(193,101), QPoint(200,108), QPoint(192,100), QPoint(195,103), QPoint(194,102), QPoint(201,109), QPoint(202,110), QPoint(203,111), QPoint(199,108), QPoint(193,102), QPoint(200,109), QPoint(192,101), QPoint(194,103), QPoint(202,111), QPoint(201,110), QPoint(199,109), QPoint(198,108), QPoint(193,103), QPoint(200,110), QPoint(192,102), QPoint(201,111), QPoint(198,109), QPoint(197,108), QPoint(192,103), QPoint(196,108), QPoint(194,98), QPoint(194,99), QPoint(193,98), QPoint(192,97), QPoint(199,105), QPoint(198,104), QPoint(193,99), QPoint(200,106), QPoint(192,98), QPoint(199,106), QPoint(198,105), QPoint(197,104), QPoint(192,99), QPoint(199,107), QPoint(198,106), QPoint(197,105), QPoint(196,104), QPoint(198,107), QPoint(197,106), QPoint(196,105), QPoint(195,104), QPoint(194,104), QPoint(197,107), QPoint(196,106), QPoint(195,105), QPoint(194,105), QPoint(193,104), QPoint(196,107), QPoint(195,106), QPoint(192,104), QPoint(194,106), QPoint(193,105), QPoint(195,107), QPoint(192,105), QPoint(194,107), QPoint(193,106), QPoint(192,106)};

//    QSet<QPoint> a = { QPoint(1,1), QPoint(1,2), QPoint(2,1), QPoint(2,2) };
//    QSet<QPoint> a = { QPoint(1,1), QPoint(1,2), QPoint(2,1), QPoint(2,2), QPoint(3,1), QPoint(4,1), QPoint(5,1), QPoint(6,1) };
    QSet<QPoint> a = { QPoint(1,1), QPoint(2,2), QPoint(3,2), QPoint(2,3), QPoint(3,3) };

    QImage i;
    FloodFill f(i, 1);

    QPolygonF p = f.maskToPoly3(a.toList());

    qDebug() << "polygon" << p;

    exit(0);
}
