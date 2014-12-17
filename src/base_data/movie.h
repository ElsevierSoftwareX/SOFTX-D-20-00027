#ifndef MOVIE_H
#define MOVIE_H

#include <string>
#include <QList>

#include "frame.h"
#include "../auto_tracklets/autotracklet.h"

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
    void addFrame(std::shared_ptr<Frame>);
    std::shared_ptr<Frame> getFrame(uint32_t);
    friend std::ostream& operator<< (std::ostream&, const Movie&);

private:
    std::string inputFiles;
    std::string timeOfConversion;
    std::string trackingAlgorithm;
    std::string ilastikVersion;
    std::string timeOfTracking;
    QList<std::shared_ptr<Frame>> frames;
};

std::ostream& operator<<(std::ostream&, const Movie&);

#endif // MOVIE_H
