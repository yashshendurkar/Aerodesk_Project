// =====================================================
// AeroException.h  -  AeroDesk EXCEPTION HANDLING HEADER
// Header-only (no .cpp, no extra linking step): put this file in
// the SAME folder as AeroDesk.cpp and add
//
//        #include "AeroException.h"
//
// near the other includes. Every class below derives from the
// matching std:: exception type (invalid_argument / out_of_range /
// runtime_error), so every existing catch (const invalid_argument&)
// / catch (const runtime_error&) in AeroDesk keeps working
// unchanged - the new classes only give the errors proper names.
//
//   InvalidFlightDataException   : invalid_argument  (flight data)
//   InvalidPassengerIDException  : invalid_argument  (bad ID text)
//   PassengerNotFoundException   : out_of_range      (unknown ID)
//   QueueOverflowException       : runtime_error     (queue full)
//   QueueUnderflowException      : runtime_error     (queue empty)
//   EmergencyQueueEmptyException : runtime_error     (priority_queue)
//   FileException                : runtime_error     (fstream errors)
// =====================================================

#ifndef AERODESK_EXCEPTION_H
#define AERODESK_EXCEPTION_H

#include <stdexcept>
#include <string>
#include <cctype>

// ---------------- FLIGHT DATA ----------------
// Message format: "<who>: <reason>"
//   who = "Flight AI101" or "PassengerFlight AI101"
class InvalidFlightDataException : public std::invalid_argument
{
public:
    InvalidFlightDataException(const std::string& who,
                               const std::string& reason)
        : std::invalid_argument(who + ": " + reason) {}
};

// ---------------- PASSENGER ID ----------------
class InvalidPassengerIDException : public std::invalid_argument
{
public:
    explicit InvalidPassengerIDException(const std::string& message)
        : std::invalid_argument(message) {}
};

class PassengerNotFoundException : public std::out_of_range
{
public:
    explicit PassengerNotFoundException(int id)
        : std::out_of_range("Passenger not found: " + std::to_string(id)) {}
};

// ---------------- WAITING QUEUE ----------------
class QueueOverflowException : public std::runtime_error
{
public:
    QueueOverflowException(const std::string& flightID, int capacity)
        : std::runtime_error(
              "Queue Overflow! Cannot add " + flightID +
              " - the waiting queue is full (" + std::to_string(capacity) +
              "/" + std::to_string(capacity) + ").") {}
};

class QueueUnderflowException : public std::runtime_error
{
public:
    QueueUnderflowException()
        : std::runtime_error(
              "Queue Underflow! No flights are waiting to depart.") {}
};

class EmergencyQueueEmptyException : public std::runtime_error
{
public:
    EmergencyQueueEmptyException()
        : std::runtime_error(
              "Emergency queue underflow: nothing to depart.") {}
};

// ---------------- FILE HANDLING ----------------
class FileException : public std::runtime_error
{
public:
    explicit FileException(const std::string& message)
        : std::runtime_error(message) {}

    // convenience: FileException::cannotOpen("passengers.txt")
    static FileException cannotOpen(const std::string& name)
    {
        return FileException("Cannot open file for writing: " + name);
    }

    static FileException notFound(const std::string& name)
    {
        return FileException("File not found: " + name);
    }

    static FileException writeFailed(const std::string& name)
    {
        return FileException("Write failed for file: " + name);
    }

    static FileException corrupt(const std::string& name,
                                 const std::string& detail)
    {
        return FileException("Corrupt file " + name + ": " + detail);
    }
};

// =====================================================
// VALIDATION HELPERS (throw the classes above)
// =====================================================

// Passenger ID typed by the user: not empty, digits only.
inline void validatePassengerIDText(const std::string& text)
{
    if (text.empty())
        throw InvalidPassengerIDException("Please enter a Passenger ID.");

    for (size_t i = 0; i < text.length(); i++)
    {
        if (!std::isdigit((unsigned char)text[i]))
            throw InvalidPassengerIDException(
                "Invalid Passenger ID format: \"" + text + "\" (digits only).");
    }
}

// Same checks as the FlightBase constructor.
inline void validateFlightNumbers(const std::string& flightID,
                                  int speed, int fuel)
{
    if (speed < 0)
        throw InvalidFlightDataException(
            "Flight " + flightID,
            "speed cannot be negative (" + std::to_string(speed) + ").");

    if (fuel > 100)
        throw InvalidFlightDataException(
            "Flight " + flightID,
            "fuel cannot exceed 100% (got " + std::to_string(fuel) + ").");
}

// Same check as the PassengerFlight constructor.
inline void validatePassengerCapacity(const std::string& flightID,
                                      int passengers, int capacity)
{
    if (passengers > capacity)
        throw InvalidFlightDataException(
            "PassengerFlight " + flightID,
            "passengers (" + std::to_string(passengers) +
            ") exceed capacity (" + std::to_string(capacity) + ").");
}

#endif // AERODESK_EXCEPTION_H