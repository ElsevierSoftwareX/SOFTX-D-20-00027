#ifndef MOVIE_H
#define MOVIE_H

#include <string>
#include <QList>

#include "frame.h"

class Movie
{
public:
    Movie();

private:
    std::string inputFiles;
    std::string timeOfConversion;
    std::string trackingAlgorithm;
    std::string ilastikVersion;
    std::string timeOfTracking;
    QList<Frame> frames;
};

#endif // MOVIE_H
