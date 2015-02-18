#ifndef CTEXCEPTION_H
#define CTEXCEPTION_H

#include <exception>

namespace CellTracker {

class CTException : public std::exception
{
public:
    CTException();
    ~CTException();

    virtual const char *what() const throw();
};

}

#endif // CTEXCEPTION_H
