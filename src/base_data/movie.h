#ifndef MOVIE_H
#define MOVIE_H

#include <string>
#include <QList>

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
    void addFrame(Frame *);
    Frame *getFrame(uint32_t);

private:
    std::string inputFiles;
    std::string timeOfConversion;
    std::string trackingAlgorithm;
    std::string ilastikVersion;
    std::string timeOfTracking;
    QList<Frame*> frames;
    friend std::ostream& operator<< (std::ostream&, const Movie&);
};

std::ostream& operator<<(std::ostream&, const Movie&);

#endif // MOVIE_H
