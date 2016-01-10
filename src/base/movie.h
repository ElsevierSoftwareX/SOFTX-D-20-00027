#ifndef MOVIE_H
#define MOVIE_H

#include <cstdint>
#include <iostream>
#include <memory>

#include <QHash>

#include "frame.h"

namespace CellTracker { class Movie; }
std::ostream& operator<<(std::ostream&, const CellTracker::Movie&);

namespace CellTracker {

/*!
 * \brief The Movie class
 *
 * A Movie object consists of Frame%s which in turn represent certain points in
 * time.
 */
class Movie
{
public:
    Movie();
    ~Movie();

    std::shared_ptr<Frame> getFrame(uint32_t) const;
    QHash<uint32_t,std::shared_ptr<Frame>> getFrames();

    void addFrame(std::shared_ptr<Frame>);

    friend std::ostream& ::operator<< (std::ostream&, const CellTracker::Movie&);

private:
    QHash<uint32_t,std::shared_ptr<Frame>> frames; /*!< The Frames contained in this Movie */
};

}

#endif // MOVIE_H
