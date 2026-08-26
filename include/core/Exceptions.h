#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

class TravelException : public std::runtime_error {
public:
    explicit TravelException(const std::string& message)
        : std::runtime_error(message) {}
};

class LocationNotFoundException : public TravelException {
public:
    explicit LocationNotFoundException(const std::string& locName)
        : TravelException("Location not found in network: " + locName) {}
};

class RouteNotFoundException : public TravelException {
public:
    RouteNotFoundException(const std::string& from, const std::string& to)
        : TravelException("No viable travel route found between " + from + " and " + to) {}
};

class AuthenticationException : public TravelException {
public:
    explicit AuthenticationException(const std::string& msg)
        : TravelException("Authentication Error: " + msg) {}
};

class UserAlreadyExistsException : public TravelException {
public:
    explicit UserAlreadyExistsException(const std::string& username)
        : TravelException("User already registered with username: " + username) {}
};

class InsufficientBalanceException : public TravelException {
public:
    InsufficientBalanceException(double current, double required)
        : TravelException("Insufficient wallet balance. Current: " + std::to_string(static_cast<int>(current)) +
                          " BDT, Required: " + std::to_string(static_cast<int>(required)) + " BDT") {}
};

class BookingNotFoundException : public TravelException {
public:
    explicit BookingNotFoundException(const std::string& bookingId)
        : TravelException("Booking ID not found: " + bookingId) {}
};

class DataPersistenceException : public TravelException {
public:
    explicit DataPersistenceException(const std::string& msg)
        : TravelException("Data Storage Error: " + msg) {}
};

#endif // EXCEPTIONS_H
