#ifndef CHANNEL_H
#define CHANNEL_H

#include <iostream>
#include <string>
#include <memory>

#include <QImage>

namespace CellTracker { class Channel; }
std::ostream &operator<<(std::ostream&, const CellTracker::Channel&);

namespace CellTracker {

/*!
 * \brief The Channel class
 *
 * A Channel actually contains the QImage to display. As there can be more than
 * one Channel, either one QImage%s should be selected or more QImage%s should
 * be composed into one. Each Channel holds a list of Object%s that are present
 * in the QImage.
 */
class Channel
{
public:
    Channel();
    Channel(int);
    ~Channel();

    int getChanId() const;
    std::shared_ptr<QImage> getImage() const;

    void setImage(const std::shared_ptr<QImage> &);

    friend std::ostream& ::operator<<(std::ostream&, const Channel&);

private:
    int id;
    std::shared_ptr<QImage> image;
};

}


#endif // CHANNEL_H
