#ifndef PROJECT_H
#define PROJECT_H

#include <memory>

#include "base_data/movie.h"
#include "auto_tracklets/autotracklet.h"
#include "corrected_data/genealogy.h"

class Project
{
    friend std::ostream& operator<< (std::ostream&, Project&);

public:
    Project();
    std::shared_ptr<Movie> getMovie() const;
    QList<std::shared_ptr<AutoTracklet>> getAutoTracklets() const;
    std::shared_ptr<Genealogy> getGenealogy() const;
    void setMovie(const std::shared_ptr<Movie> &value);
    void setGenealogy(const std::shared_ptr<Genealogy> &value);
    void addAutoTracklet(const std::shared_ptr<AutoTracklet> &value);

private:
    std::shared_ptr<Movie> movie;
    QList<std::shared_ptr<AutoTracklet>> autoTracklets;
    std::shared_ptr<Genealogy> genealogy;
};

std::ostream& operator<< (std::ostream&, Project&);

#endif // PROJECT_H
