#ifndef CTEXCEPTION_H
#define CTEXCEPTION_H

#include <exception>

#include <string>

namespace CellTracker {

/*!
 * \brief The CTException class
 *
 * Superclass of the other Exceptions.
 */
class CTException : public std::exception
{
    using std::exception::what;

public:
    CTException() = default;

    const char *what() const noexcept;
};

}

#endif // CTEXCEPTION_H
