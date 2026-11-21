
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <iomanip>

using namespace std;

// ==========================================
// AERODESK OOP CO3
// Inheritance, Polymorphism, Exceptions, STL
// ==========================================

// BASE CLASS
class Flight
{
protected:
    string flightNo;
    string source;
    string destination;
    double distance;
    double fuelRate;

public:
    Flight(string no, string src, string dest,
           double km, double rate)
        : flightNo(no), source(src), destination(dest),
          distance(km), fuelRate(rate)
    {
        if (flightNo.empty())
            throw invalid_argument("Flight number cannot be empty.");

        if (distance <= 0)
            throw invalid_argument("Distance must be greater than zero.");

        if (fuelRate <= 0)
            throw invalid_argument("Fuel rate must be greater than zero.");
    }

    virtual ~Flight() = default;

    string getFlightNo() const
    {
        return flightNo;
    }

    virtual double calculateFuel() const
    {
        return distance * fuelRate;
    }

    virtual string getType() const = 0;

    virtual void display() const
    {
        cout << "\nFlight Number : " << flightNo
             << "\nFlight Type   : " << getType()
             << "\nRoute         : " << source << " -> " << destination
             << "\nDistance      : " << distance << " km"
             << "\nFuel Required : " << calculateFuel() << " units\n";
    }
};

// DERIVED CLASS 1: INHERITANCE
class PassengerFlight : public Flight
{
private:
    int passengerCount;
    int capacity;

public:
    PassengerFlight(string no, string src, string dest,
                    double km, double rate,
                    int passengers, int maxCapacity)
        : Flight(no, src, dest, km, rate),
          passengerCount(passengers), capacity(maxCapacity)
    {
        if (capacity <= 0)
            throw invalid_argument("Passenger capacity must be positive.");

        if (passengerCount < 0 || passengerCount > capacity)
            throw out_of_range("Passenger count exceeds valid capacity.");
    }

    string getType() const override
    {
        return "Passenger Flight";
    }

    void display() const override
    {
        Flight::display();
        cout << "Passengers    : " << passengerCount
             << "/" << capacity << '\n';
    }
};

// DERIVED CLASS 2: INHERITANCE
class CargoFlight : public Flight
{
private:
    double cargoWeight;

public:
    CargoFlight(string no, string src, string dest,
                double km, double rate, double weight)
        : Flight(no, src, dest, km, rate),
          cargoWeight(weight)
    {
        if (cargoWeight < 0)
            throw invalid_argument("Cargo weight cannot be negative.");
    }

    // Polymorphism: cargo flights need extra fuel.
    double calculateFuel() const override
    {
        return Flight::calculateFuel() + cargoWeight * 0.5;
    }

    string getType() const override
    {
        return "Cargo Flight";
    }

    void display() const override
    {
        Flight::display();
        cout << "Cargo Weight  : " << cargoWeight << " kg\n";
    }
};

// ==========================================
// MODULE FUNCTION
// ==========================================

void runOOPCO3Module()
{
    // STL vector stores different flight objects.
    vector<Flight*> flights;

    // STL map provides flight number lookup.
    map<string, Flight*> flightIndex;

    int choice = 0;

    do
    {
        cout << "\n========================================\n";
        cout << " AERODESK OOP CO3: FLIGHT MANAGEMENT\n";
        cout << "========================================\n";
        cout << "1. Add Passenger Flight\n";
        cout << "2. Add Cargo Flight\n";
        cout << "3. Display All Flights\n";
        cout << "4. Search Flight by Number\n";
        cout << "5. Sort Flights by Flight Number\n";
        cout << "6. Demonstrate Exception Handling\n";
        cout << "7. Back to Main Menu\n";
        cout << "Enter choice: ";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Enter a number.\n";
            continue;
        }

        try
        {
            if (choice == 1 || choice == 2)
            {
                string no, src, dest;
                double km, rate;

                cout << "Flight number: ";
                cin >> no;

                if (flightIndex.count(no))
                    throw invalid_argument("Flight number already exists.");

                cout << "Source: ";
                cin >> src;

                cout << "Destination: ";
                cin >> dest;

                cout << "Distance (km): ";
                if (!(cin >> km))
                    throw invalid_argument("Distance must be numeric.");

                cout << "Fuel rate (units/km): ";
                if (!(cin >> rate))
                    throw invalid_argument("Fuel rate must be numeric.");

                Flight* flight = nullptr;

                if (choice == 1)
                {
                    int passengers, capacity;

                    cout << "Number of passengers: ";
                    if (!(cin >> passengers))
                        throw invalid_argument("Passenger count must be an integer.");

                    cout << "Passenger capacity: ";
                    if (!(cin >> capacity))
                        throw invalid_argument("Capacity must be an integer.");

                    flight = new PassengerFlight(
                        no, src, dest, km, rate,
                        passengers, capacity
                    );
                }
                else
                {
                    double weight;

                    cout << "Cargo weight (kg): ";
                    if (!(cin >> weight))
                        throw invalid_argument("Cargo weight must be numeric.");

                    flight = new CargoFlight(
                        no, src, dest, km, rate, weight
                    );
                }

                flights.push_back(flight);
                flightIndex[no] = flight;

                cout << "\nFlight added successfully!\n";
            }
            else if (choice == 3)
            {
                if (flights.empty())
                {
                    cout << "\nNo flights available.\n";
                }
                else
                {
                    cout << fixed << setprecision(2);

                    // Runtime polymorphism
                    for (const Flight* flight : flights)
                    {
                        flight->display();
                        cout << "----------------------------------------\n";
                    }
                }
            }
            else if (choice == 4)
            {
                string no;
                cout << "Enter flight number: ";
                cin >> no;

                auto it = flightIndex.find(no);

                if (it == flightIndex.end())
                    cout << "Flight not found.\n";
                else
                    it->second->display();
            }
            else if (choice == 5)
            {
                sort(
                    flights.begin(), flights.end(),
                    [](const Flight* a, const Flight* b)
                    {
                        return a->getFlightNo() < b->getFlightNo();
                    }
                );

                cout << "\nFlights sorted by flight number.\n";

                for (const Flight* flight : flights)
                    cout << flight->getFlightNo() << '\n';
            }
            else if (choice == 6)
            {
                cout << "\nTesting invalid flight details...\n";

                try
                {
                    PassengerFlight invalidFlight(
                        "TEST", "Pune", "Delhi",
                        1200, 5, 250, 180
                    );
                }
                catch (const exception& e)
                {
                    cout << "Exception caught: " << e.what() << '\n';
                }

                cout << "Exception handling test completed.\n";
            }
            else if (choice != 7)
            {
                cout << "Invalid choice. Select 1 to 7.\n";
            }
        }
        catch (const exception& e)
        {
            cout << "\nERROR: " << e.what() << '\n';
        }

    } while (choice != 7);

    // Release all dynamically allocated objects.
    for (Flight* flight : flights)
        delete flight;

    flights.clear();
    flightIndex.clear();

    cout << "\nReturning to AeroDesk Main Menu...\n";
}

// Standalone testing only. Disabled in integrated build.
#ifndef AERODESK_INTEGRATION
int main()
{
    runOOPCO3Module();
    return 0;
}
#endif
