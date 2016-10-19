#include "floodfill.h"

#include <QPolygonF>
#include <QList>
#include <QPoint>
#include <QPainterPath>
#include <QSet>
#include <QImage>

#include <QDebug>

template <typename T>
class ListNode
{
public:
    ListNode(T data, ListNode *next) : next(next), data(data) {}
    ListNode<T> *next;
    T data;
};

template <class T>
class CircularLinkedList
{
public:
    CircularLinkedList() { }
    virtual ~CircularLinkedList() {
        ListNode<T> *next;
        actualElement = head;

        while(!isEmpty()) {
            next = actualElement->next;
            delete actualElement;
            --numberOfElements;
            actualElement = next;
        }
    }

    bool isEmpty() {
        return (numberOfElements == 0);
    }

    int size() {
        return numberOfElements;
    }

    void addLast(T data) {
        if (isEmpty()) {
            ListNode<T> *listNode = new ListNode<T>(data, head);
            head = listNode;
            tail = listNode;
            numberOfElements++;
            actualElement = head;
        } else {
            ListNode<T> *listNode = new ListNode<T>(data, nullptr);
            tail->next = listNode;
            tail = listNode;
            numberOfElements++;
        }
    }

    T getActualElementData() {
        if (!isEmpty()) {
            if (actualElement == nullptr) {
                actualElement = head;
            }
            return actualElement->data;
        }
        return T{};
    }

    T getNext() {
        if (isEmpty())
            return T{};
        index = (index + 1) % numberOfElements;
        if (index == 0)
            actualElement = head;
        else
            actualElement = actualElement->next;
        return actualElement->data;
    }

    bool setActualElement(T data) {
        int tmpIndex = index;
        if (data == actualElement->data)
            return true;
        index = (index + 1) % numberOfElements;
        if (index == 0)
            actualElement = head;
        else
            actualElement = actualElement->next;
        while ((index != tmpIndex)) {
            if (data == actualElement->data)
                return true;
            index = (index + 1) % numberOfElements;
            if (index == 0)
                actualElement = head;
            else
                actualElement = actualElement->next;
        }
        return false;
    }

private:
    ListNode<T> *head = nullptr;
    ListNode<T> *tail = nullptr;
    ListNode<T> *actualElement = nullptr;
    int numberOfElements = 0;
    int index = 0;
};


QPolygonF FloodFill::maskToPoly(QList<QPoint> points)
{
    QPolygon outline;

    std::sort(points.begin(), points.end(), [](QPoint &a, QPoint &b){return (a.x() == b.x())?a.y() < b.y():a.x() < b.x();});

    if (!points.empty()) {
        if(points.size() == 1) {
            //prevents function from infinite loop when the mask only contains one pixel
            outline.push_back(points.front());
            return outline;
        }
        CircularLinkedList<QPoint> neighbours;
        QPoint b, c, b0, b1, tmp, c1, previous, current;

        QSet<QPoint> set = points.toSet();

        neighbours.addLast(QPoint(-1, 0));
        neighbours.addLast(QPoint(-1, -1));
        neighbours.addLast(QPoint(0, -1));
        neighbours.addLast(QPoint(1, -1));
        neighbours.addLast(QPoint(1, 0));
        neighbours.addLast(QPoint(1, 1));
        neighbours.addLast(QPoint(0, 1));
        neighbours.addLast(QPoint(-1, 1));
        neighbours.setActualElement(QPoint(-1, 0));

        b0 = points[0];

        for (int i = 1; i < neighbours.size(); i++) {
            previous = neighbours.getActualElementData();
            current = neighbours.getNext();
            tmp = b0 + current;

            if (set.contains(tmp)) {
                b1 = tmp;
                c1 = b0 + previous;
                outline.push_back(b0);
                outline.push_back(b1);
                break;
            }
        }

        b = b1;
        c = c1;

        while (true) {
            neighbours.setActualElement(c - b);

            for (int i = 1; i < 8; i++) {
                previous = neighbours.getActualElementData();
                current = neighbours.getNext();
                tmp = b + current;

                if (set.contains(tmp)) {
                    c = b + previous;
                    b = tmp;
                    outline.push_back(b);
                    break;
                }
            }
            if (b == b0)
                break;
        }
    }
    return outline;
}


QSet<QPoint> FloodFill::neighbors(QPoint &p, mode connectMode, QSize &imgSize)
{
    QSet<QPoint> ret;
    int x = p.x();
    int y = p.y();
    int imgHeight = imgSize.height();
    int imgWidth = imgSize.width();

    switch (connectMode) {
    case C4:
        /* 4 Neighbor */
        if (y - 1 > 0        ) ret.insert(QPoint(x    , y - 1));
        if (x + 1 < imgWidth ) ret.insert(QPoint(x + 1, y    ));
        if (y + 1 < imgHeight) ret.insert(QPoint(x    , y + 1));
        if (x - 1 > 0        ) ret.insert(QPoint(x - 1, y    ));
        break;
    case C8:
        /* 8 Neighbor */
        if (x - 1 > 0        && y - 1 > 0        ) ret.insert(QPoint(x - 1, y - 1));
        if (                    y - 1 > 0        ) ret.insert(QPoint(x    , y - 1));
        if (x + 1 < imgWidth && y - 1 > 0        ) ret.insert(QPoint(x + 1, y - 1));
        if (x + 1 < imgWidth                     ) ret.insert(QPoint(x + 1, y    ));
        if (x + 1 < imgWidth && y + 1 < imgHeight) ret.insert(QPoint(x + 1, y + 1));
        if (                    y + 1 < imgHeight) ret.insert(QPoint(x    , y + 1));
        if (x - 1 > 0        && y + 1 < imgHeight) ret.insert(QPoint(x - 1, y + 1));
        if (x - 1 > 0                            ) ret.insert(QPoint(x - 1, y    ));
        break;
    }

    return ret;
}

uint qHash(const QPoint &val) {
    return qHash(QPair<int, int>(val.x(), val.y()));
}

QSet<QPoint> FloodFill::calculateMask(QImage &img, QPoint &p, int thresh, mode connectMode)
{
    QSize size = img.size();
    QSet<QPoint> stack;
    QSet<QPoint> mask;
    QSet<QPoint> rejected;

    stack.insert(p);

    QRgb colP = img.pixel(p);
    int grayP = qGray(colP);

    int i = 0;
    while (!stack.empty()) {
        i++;
        QPoint curr = *stack.begin();
        stack.remove(curr);
        mask.insert(curr);

        for (const QPoint &n : neighbors(curr, connectMode, size)) {
            if (mask.contains(n))
                continue;

            QRgb colN = img.pixel(n);
            int grayN = qGray(colN);
            int diff = abs(grayP - grayN);
            if (diff < thresh)
                stack.insert(n);
            else
                rejected.insert(n);
        }

        if (mask.size() > (img.height() * img.width() * 0.25))
            break; /* stop this, if the mask gets to big */
    }

    return mask;
}

QPolygonF FloodFill::compute(QPoint p, int thresh) {
    QSet<QPoint> mask;
    QPolygonF outline;

    QTransform trans;
    trans = trans.scale(1/scaleFactor, 1/scaleFactor);
    QImage img = image.transformed(trans);

    mask = calculateMask(img, p, thresh, connectMode);
    outline = maskToPoly(mask.toList());

    return outline;
}
