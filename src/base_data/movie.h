#ifndef MOVIE_H
#define MOVIE_H

#include <string>
#include <QHash>

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

    std::shared_ptr<Frame> getFrame(uint32_t) const;

    void addFrame(std::shared_ptr<Frame>);

    friend std::ostream& operator<< (std::ostream&, const Movie&);

private:
//    std::string inputFiles;
//    std::string timeOfConversion;
//    std::string trackingAlgorithm;
//    std::string ilastikVersion;
//    std::string timeOfTracking;
    QHash<uint32_t,std::shared_ptr<Frame>> frames;
};

std::ostream& operator<<(std::ostream&, const Movie&);

#endif // MOVIE_H
