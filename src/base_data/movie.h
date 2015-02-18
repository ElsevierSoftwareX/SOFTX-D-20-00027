#ifndef MOVIE_H
#define MOVIE_H

#include <cstdint>
#include <iostream>
#include <memory>

#include <QHash>

#include "frame.h"

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

    void addFrame(std::shared_ptr<Frame>);

    friend std::ostream& operator<< (std::ostream&, const Movie&);

private:
    QHash<uint32_t,std::shared_ptr<Frame>> frames;
};

std::ostream& operator<<(std::ostream&, const Movie&);

#endif // MOVIE_H
