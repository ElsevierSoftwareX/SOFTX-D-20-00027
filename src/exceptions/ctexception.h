#ifndef CTEXCEPTION_H
#define CTEXCEPTION_H

#include <exception>

#include <string>

namespace CellTracker {

class CTException : public std::exception
{
    using std::exception::what;

public:
    CTException();

    const char *what() const noexcept;
};

}

#endif // CTEXCEPTION_H
