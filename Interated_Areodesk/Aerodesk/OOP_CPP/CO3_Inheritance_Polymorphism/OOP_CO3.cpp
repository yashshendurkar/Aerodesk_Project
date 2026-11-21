
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <iomanip>
#include <limits>

using namespace std;

// ================= BASE CLASS =================
class Flight {
protected:
    string flightNo;
    string source;
    string destination;
    double distance;
    double fuelRate;

public:
    Flight(string no, string src, string dest,
           double dist, double rate)
        : flightNo(no), source(src), destination(dest),
          distance(dist), fuelRate(rate) {

        if (flightNo.empty() || source.empty() ||
            destination.empty()) {
            throw invalid_argument(
                "Flight number and locations cannot be empty!");
        }

        if (distance <= 0 || fuelRate <= 0) {
            throw invalid_argument(
                "Distance and fuel rate must be positive!");
        }
    }

    virtual double calculateFuel() const {
        return distance * fuelRate;
    }

    virtual string getType() const = 0;

    string getFlightNo() const {
        return flightNo;
    }

    virtual void display() const {
        cout << "\nFlight Number : " << flightNo;
        cout << "\nFlight Type   : " << getType();
        cout << "\nRoute         : " << source
             << " -> " << destination;
        cout << "\nDistance      : " << distance << " km";
        cout << "\nFuel Required : "
             << fixed << setprecision(2)
             << calculateFuel() << " litres\n";
    }

    virtual ~Flight() = default;
};

// ================= INHERITANCE =================
class PassengerFlight : public Flight {
private:
    int passengers;
    int capacity;

public:
    PassengerFlight(string no, string src, string dest,
                    double dist, double rate,
                    int count, int maxCapacity)
        : Flight(no, src, dest, dist, rate),
          passengers(count), capacity(maxCapacity) {

        if (capacity <= 0 || passengers < 0 ||
            passengers > capacity) {
            throw invalid_argument(
                "Invalid passenger count or capacity!");
        }
    }

    string getType() const override {
        return "Passenger Flight";
    }

    void display() const override {
        Flight::display();
        cout << "Passengers    : " << passengers
             << "/" << capacity << "\n";
    }
};

// ================= INHERITANCE =================
class CargoFlight : public Flight {
private:
    double cargoWeight;

public:
    CargoFlight(string no, string src, string dest,
                double dist, double rate, double weight)
        : Flight(no, src, dest, dist, rate),
          cargoWeight(weight) {

        if (cargoWeight < 0) {
            throw invalid_argument(
                "Cargo weight cannot be negative!");
        }
    }

    double calculateFuel() const override {
        return Flight::calculateFuel()
               + cargoWeight * 0.5;
    }

    string getType() const override {
        return "Cargo Flight";
    }

    void display() const override {
        Flight::display();
        cout << "Cargo Weight  : " << cargoWeight
             << " kg\n";
    }
};

// ================= INPUT HELPER =================
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int readInt(const string& prompt) {
    int value;
    cout << prompt;

    if (!(cin >> value)) {
        clearInput();
        throw invalid_argument("Please enter a valid integer!");
    }

    clearInput();
    return value;
}

double readDouble(const string& prompt) {
    double value;
    cout << prompt;

    if (!(cin >> value)) {
        clearInput();
        throw invalid_argument("Please enter a valid number!");
    }

    clearInput();
    return value;
}

string readText(const string& prompt) {
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

// ================= OOP CO3 MODULE =================
void runOOPCO3Module() {
    vector<Flight*> flights;
    map<string, Flight*> flightIndex;

    int choice = 0;

    do {
        cout << "\n====================================";
        cout << "\n    AERODESK - OOP CO3 MODULE";
        cout << "\n====================================";
        cout << "\n1. Add Passenger Flight";
        cout << "\n2. Add Cargo Flight";
        cout << "\n3. Display All Flights";
        cout << "\n4. Search Flight by Number";
        cout << "\n5. Sort Flights by Number";
        cout << "\n6. Demonstrate Exception Handling";
        cout << "\n7. Back to Main Menu";
        cout << "\nEnter choice: ";

        if (!(cin >> choice)) {
            clearInput();
            cout << "\nInvalid input! Enter a number.\n";
            continue;
        }
        clearInput();

        try {
            if (choice == 1 || choice == 2) {
                string no = readText("Enter flight number: ");
                string src = readText("Enter source: ");
                string dest = readText("Enter destination: ");

                if (flightIndex.count(no)) {
                    throw invalid_argument(
                        "Flight number already exists!");
                }

                double dist = readDouble("Enter distance (km): ");
                double rate = readDouble(
                    "Enter fuel rate (litres/km): ");

                Flight* flight = nullptr;

                if (choice == 1) {
                    int count = readInt("Enter passengers: ");
                    int capacity = readInt(
                        "Enter maximum capacity: ");

                    flight = new PassengerFlight(
                        no, src, dest, dist, rate,
                        count, capacity);
                } else {
                    double weight = readDouble(
                        "Enter cargo weight (kg): ");

                    flight = new CargoFlight(
                        no, src, dest, dist, rate, weight);
                }

                flights.push_back(flight);
                flightIndex[no] = flight;

                cout << "\nFlight added successfully!\n";
            }

            else if (choice == 3) {
                if (flights.empty()) {
                    cout << "\nNo flights available.\n";
                } else {
                    cout << "\n===== ALL FLIGHTS =====\n";

                    // Runtime polymorphism
                    for (const Flight* flight : flights) {
                        flight->display();
                        cout << "------------------------\n";
                    }
                }
            }

            else if (choice == 4) {
                string no = readText(
                    "Enter flight number to search: ");

                auto it = flightIndex.find(no);

                if (it != flightIndex.end()) {
                    cout << "\nFlight found:\n";
                    it->second->display();
                } else {
                    cout << "\nFlight not found.\n";
                }
            }

            else if (choice == 5) {
                sort(flights.begin(), flights.end(),
                    [](const Flight* a, const Flight* b) {
                        return a->getFlightNo()
                             < b->getFlightNo();
                    });

                cout << "\nFlights sorted by flight number.\n";

                for (const Flight* flight : flights) {
                    flight->display();
                    cout << "------------------------\n";
                }
            }

            else if (choice == 6) {
                cout << "\nTesting exception handling...\n";

                try {
                    PassengerFlight invalidFlight(
                        "TEST", "Pune", "Delhi",
                        1000, 5, 250, 180);

                    invalidFlight.display();
                }
                catch (const exception& e) {
                    cout << "Exception caught: "
                         << e.what() << "\n";
                }

                cout << "Program continues safely.\n";
            }

            else if (choice == 7) {
                cout << "\nReturning to main menu...\n";
            }

            else {
                cout << "\nInvalid choice! Try again.\n";
            }
        }
        catch (const exception& e) {
            cout << "\nError: " << e.what() << "\n";
        }

    } while (choice != 7);

    // Release dynamically allocated objects
    for (Flight* flight : flights) {
        delete flight;
    }

    flights.clear();
    flightIndex.clear();
}

// Standalone testing only.
// Disabled when compiled with the Aerodesk main project.
#ifndef AERODESK_INTEGRATION
int main() {
    runOOPCO3Module();
    return 0;
}
#endif
