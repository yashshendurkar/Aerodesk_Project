#include <GL/freeglut.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <cctype>
// ===== EXCEPTION HANDLING START =====
// invalid_argument / out_of_range / runtime_error all live here.
#include <stdexcept>
// Separate exception header (same folder as this file): custom exception
// classes + validation helpers. Header-only, so no extra linking is needed.
#include "AeroException.h"
// ===== EXCEPTION HANDLING END =====
// ===== STL START =====
// vector<FlightBase*>/vector<Passenger> (dynamic arrays), map<int,
// Passenger*> (O(log n) ID lookup), sort() + lambda (flight list
// sorting), and queue<string>/stack<string> (STL mirrors of the
// project's own hand-written stack/queue, for comparison).
#include <vector>
#include <map>
#include <algorithm>
#include <queue>
#include <stack>
#include <set>
#include <unordered_map>
#include <chrono>
#include <sstream>
// ===== STL END =====

using namespace std;

// =====================================================
// STACK MODULE
// =====================================================

#define STACK_SIZE 10

string operationStack[STACK_SIZE];
int stackTop = -1;

// ===== STL START =====
// STL mirror of the manual array-based stack above: every push/pop
// below also happens on this std::stack<string>, purely so the two
// can be compared side by side (see displayStackHistory() further
// down). The manual array is still the ONE source of truth driving
// the actual program (getLatestOperation() etc.) - this is purely
// an additive comparison structure, nothing reads from it.
stack<string> stlOperationStack;
// ===== STL END =====

void pushOperation(string operation)
{
    if (stackTop == STACK_SIZE - 1)
    {
        cout << "\nStack Overflow!\n";
        return;
    }

    operationStack[++stackTop] = operation;

    // ===== STL START =====
    stlOperationStack.push(operation);
    // ===== STL END =====

    cout << "\n[STACK] Operation Added: "
         << operation << endl;
}

void popOperation()
{
    if (stackTop == -1)
    {
        cout << "\nStack Underflow!\n";
        return;
    }

    cout << "\n[STACK] Removed: "
         << operationStack[stackTop] << endl;

    stackTop--;

    // ===== STL START =====
    if (!stlOperationStack.empty())
        stlOperationStack.pop();
    // ===== STL END =====
}

string getLatestOperation()
{
    if (stackTop == -1)
        return "No Operation";

    return operationStack[stackTop];
}

void displayStackHistory()
{
    cout << "\n====================================\n";
    cout << "       AERODESK STACK HISTORY\n";
    cout << "====================================\n";

    if (stackTop == -1)
    {
        cout << "Stack is empty.\n";
    }
    else
    {
        for (int i = stackTop; i >= 0; i--)
        {
            cout << stackTop - i + 1
                 << ". "
                 << operationStack[i]
                 << endl;
        }
    }

    // ===== STL START =====
    // Same history from the std::stack<string> mirror (copied so
    // popping to print doesn't destroy the real mirror).
    cout << "--- STL stack<string> version ---\n";
    cout << "size() = " << stlOperationStack.size();

    if (!stlOperationStack.empty())
        cout << " | top() = " << stlOperationStack.top();

    cout << endl;

    stack<string> copy = stlOperationStack;
    int n = 1;

    while (!copy.empty())
    {
        cout << n++ << ". " << copy.top() << endl;
        copy.pop();
    }
    // ===== STL END =====

    cout << "====================================\n";
}


// =====================================================
// QUEUE MODULE
// =====================================================

#define QUEUE_SIZE 5

string flightQueue[QUEUE_SIZE];
int queueFront = 0;
int queueRear = -1;
int queueCount = 0;

// ===== STL START =====
// STL mirror of the manual circular-array queue above. Mirrored on
// every successful enqueue/dequeue/remove so both can be compared
// side by side in displayQueue(); the manual array stays the one
// source of truth for the real program.
queue<string> stlFlightQueue;
// ===== STL END =====

bool isFlightInQueue(const string& flightID)
{
    for (int i = 0; i < queueCount; i++)
    {
        int index = (queueFront + i) % QUEUE_SIZE;

        if (flightQueue[index] == flightID)
            return true;
    }

    return false;
}

// ===== EXCEPTION HANDLING START =====
// Queue Overflow used to just cout a message and return false.
// Now it THROWS runtime_error instead - every call site below
// (toggleWaiting, addCurrentFlightToQueue, the 'E' key demo) is
// responsible for wrapping its call in try/catch. The "already
// waiting" case is still normal, expected business logic (not a
// real error), so it still just returns false, no exception.
// ===== EXCEPTION HANDLING END =====
bool enqueueFlight(const string& flightID)
{
    if (queueCount == QUEUE_SIZE)
    {
        throw QueueOverflowException(flightID, QUEUE_SIZE);   // AeroException.h
    }

    if (isFlightInQueue(flightID))
    {
        cout << "\n[QUEUE] " << flightID
             << " is already waiting.\n";
        return false;
    }

    queueRear = (queueRear + 1) % QUEUE_SIZE;
    flightQueue[queueRear] = flightID;
    queueCount++;

    // ===== STL START =====
    stlFlightQueue.push(flightID);
    // ===== STL END =====

    cout << "\n[QUEUE] Flight Added: "
         << flightID << endl;

    return true;
}

// ===== EXCEPTION HANDLING START =====
// Queue Underflow: same change as enqueueFlight() above - throws
// runtime_error instead of printing + returning an empty string.
// Every caller (departNextFlight(), the 'E' key demo) must catch it.
// ===== EXCEPTION HANDLING END =====
string dequeueFlight()
{
    if (queueCount == 0)
    {
        throw QueueUnderflowException();   // AeroException.h
    }

    string flightID = flightQueue[queueFront];

    queueFront = (queueFront + 1) % QUEUE_SIZE;
    queueCount--;

    if (queueCount == 0)
    {
        queueFront = 0;
        queueRear = -1;
    }

    // ===== STL START =====
    if (!stlFlightQueue.empty())
        stlFlightQueue.pop();
    // ===== STL END =====

    cout << "\n[QUEUE] Flight Departed: "
         << flightID << endl;

    return flightID;
}

void removeFlightFromQueue(const string& flightID)
{
    if (queueCount == 0)
        return;

    string tempQueue[QUEUE_SIZE];
    int newCount = 0;

    for (int i = 0; i < queueCount; i++)
    {
        int index = (queueFront + i) % QUEUE_SIZE;

        if (flightQueue[index] != flightID)
        {
            tempQueue[newCount] = flightQueue[index];
            newCount++;
        }
    }

    queueFront = 0;
    queueRear = newCount - 1;
    queueCount = newCount;

    for (int i = 0; i < queueCount; i++)
        flightQueue[i] = tempQueue[i];

    if (queueCount == 0)
        queueRear = -1;

    // ===== STL START =====
    // Rebuild the STL mirror from the manual queue (std::queue has
    // no remove-from-middle), keeping both structures identical.
    stlFlightQueue = queue<string>();

    for (int i = 0; i < queueCount; i++)
        stlFlightQueue.push(flightQueue[i]);
    // ===== STL END =====
}

string getNextQueuedFlight()
{
    if (queueCount == 0)
        return "None";

    return flightQueue[queueFront];
}

void displayQueue()
{
    cout << "\n====================================\n";
    cout << "       AERODESK WAITING QUEUE\n";
    cout << "====================================\n";

    if (queueCount == 0)
    {
        cout << "Queue is empty.\n";
    }
    else
    {
        for (int i = 0; i < queueCount; i++)
        {
            int index = (queueFront + i) % QUEUE_SIZE;

            cout << i + 1 << ". "
                 << flightQueue[index]
                 << endl;
        }
    }

    // ===== STL START =====
    cout << "--- STL queue<string> version ---\n";
    cout << "size() = " << stlFlightQueue.size();

    if (!stlFlightQueue.empty())
        cout << " | front() = " << stlFlightQueue.front()
             << " | back() = " << stlFlightQueue.back();

    cout << endl;
    // ===== STL END =====

    cout << "Queue Size : "
         << queueCount << "/" << QUEUE_SIZE << endl;

    cout << "Next Flight: "
         << getNextQueuedFlight() << endl;

    cout << "====================================\n";
}


// =====================================================
// FLIGHT DATA
// =====================================================

struct Flight
{
    char flightID[20];
    int speed;
    int altitude;
    int fuel;
    int passengers;
};

const int MAX_FLIGHTS = 5;

Flight flights[MAX_FLIGHTS] =
{
    {"AI101", 220, 1000, 85, 100},
    {"AI202", 250, 2000, 100, 120},
    {"AI303", 280, 3000, 72, 150},
    {"AI404", 200, 1500, 92, 80},
    {"AI505", 260, 2500, 90, 110}
};

string destinationNames[MAX_FLIGHTS] =
{
    "Delhi",
    "Mumbai",
    "Bangalore",
    "Chennai",
    "Kolkata"
};


// =====================================================
// LINKED LIST
// =====================================================

struct FlightNode
{
    Flight data;
    FlightNode* next;
};

FlightNode* head = NULL;
FlightNode* currentNode = NULL;


// =====================================================
// OOP - INHERITANCE (BASE + DERIVED CLASSES)
// =====================================================

// ---------------- BASE CLASS ----------------
// NOTE: struct Flight (linked list data) already exists above,
// so the OOP base class is named FlightBase to avoid a conflict.
class FlightBase
{
private:
    string flightID;
    int speed;
    int altitude;
    int fuel;
    int passengers;

public:
    // DEFAULT CONSTRUCTOR
    FlightBase()
    {
        flightID = "NONE";
        speed = 0;
        altitude = 0;
        fuel = 0;
        passengers = 0;

        cout << "[OOP] FlightBase default constructor called." << endl;
    }

    // PARAMETERIZED CONSTRUCTOR
    // ===== EXCEPTION HANDLING START =====
    // Constructor validation: a Flight object is rejected at the
    // moment it is created if its basic numbers don't make sense,
    // instead of the old behaviour of silently clamping them in
    // setSpeed()/setFuel(). This throws invalid_argument, which
    // every "new ...Flight(...)" call site must catch - see
    // initFlightObjects() below, and the 'E' key demo further down.
    FlightBase(string id, int spd, int alt, int fl, int pass)
    {
        validateFlightNumbers(id, spd, fl);   // AeroException.h (speed < 0, fuel > 100)
    // ===== EXCEPTION HANDLING END =====

        flightID = id;
        speed = 0;
        altitude = 0;
        fuel = 0;
        passengers = 0;

        setSpeed(spd);
        setAltitude(alt);
        setFuel(fl);
        setPassengers(pass);

        cout << "[OOP] FlightBase constructor called: "
             << flightID << endl;
    }

    // Virtual destructor - needed when deleting through a base pointer
    virtual ~FlightBase() {}

    void setFlightData(const char* id, int spd, int alt, int fl, int pass)
    {
        flightID = id;
        setSpeed(spd);
        setAltitude(alt);
        setFuel(fl);
        setPassengers(pass);
    }

    // SETTERS (with validation)
    void setSpeed(int value)      { if (value >= 0) speed = value; }
    void setAltitude(int value)   { if (value >= 0) altitude = value; }
    void setPassengers(int value) { if (value >= 0) passengers = value; }

    void setFuel(int value)
    {
        if (value < 0)   value = 0;
        if (value > 100) value = 100;
        fuel = value;
    }

    // GETTERS
    string getFlightID() const { return flightID; }
    int getSpeed() const       { return speed; }
    int getAltitude() const    { return altitude; }
    int getFuel() const        { return fuel; }
    int getPassengers() const  { return passengers; }

    // ===== POLYMORPHISM START =====
    // calculateFuel() and display() are ordinary VIRTUAL functions
    // (not pure virtual): FlightBase gives them a default body, and
    // every derived class below OVERRIDES both with its own
    // behaviour. Calling either one through a FlightBase* pointer
    // (see getCurrentFlightObject(), flightObjects[], and the
    // Flight Details page / OOP panel below) always runs the
    // DERIVED class's version at runtime - that is polymorphism.
    virtual float calculateFuel() const
    {
        // Generic fallback formula, used only if a derived class
        // somehow never overrides this.
        return (float)fuel;
    }

    virtual void display() const
    {
        cout << "Flight " << flightID
             << " [" << getType() << "]"
             << "  Speed: " << speed << " km/h"
             << "  Altitude: " << altitude << " m"
             << "  Fuel: " << fuel << "%"
             << "  Passengers: " << passengers
             << "  Fuel Required: " << calculateFuel() << " units"
             << endl;
    }
    // ===== POLYMORPHISM END =====

    // PURE VIRTUAL - getType() has NO body at all here ("= 0"),
    // which is what makes FlightBase an ABSTRACT class: it can
    // never be instantiated directly ("new FlightBase(...)" would
    // be a compile error), only through a concrete derived class
    // that implements getType().
    virtual string getType() const = 0;
    virtual string getClassName() const = 0;
    virtual string getExtraLabel() const = 0;
    virtual string getExtraValue() const = 0;
};

// ---------------- DERIVED CLASS 1: PASSENGER FLIGHT ----------------
class PassengerFlight : public FlightBase
{
private:
    int capacity;   // total seats

public:
    // ===== EXCEPTION HANDLING START =====
    // Extra validation specific to THIS derived class: passengers
    // can never exceed the aircraft's own seating capacity. This
    // runs after FlightBase's own constructor has already checked
    // speed/fuel, so either check can reject the object.
    PassengerFlight(string id, int spd, int alt, int fl, int pass, int cap)
        : FlightBase(id, spd, alt, fl, pass)
    {
        validatePassengerCapacity(id, pass, cap);   // AeroException.h

        capacity = cap;
        cout << "[OOP] PassengerFlight constructor called (capacity "
             << capacity << ")" << endl;
    }
    // ===== EXCEPTION HANDLING END =====

    void setCapacity(int c) { if (c >= 0) capacity = c; }
    int getCapacity() const { return capacity; }

    // ===== POLYMORPHISM START =====
    // Passenger formula: fuel use scales with how FULL the flight
    // is (passengers / capacity) - a nearly-empty flight needs
    // noticeably less than a fully-booked one.
    float calculateFuel() const override
    {
        float loadFactor = (capacity > 0) ? (float)getPassengers() / (float)capacity : 0.0f;
        return getFuel() * (0.7f + 0.3f * loadFactor);
    }

    void display() const override
    {
        FlightBase::display();
        cout << "   -> Seat Capacity: " << capacity << endl;
    }
    // ===== POLYMORPHISM END =====

    string getType() const override       { return "Passenger"; }
    string getClassName() const override  { return "PassengerFlight"; }
    string getExtraLabel() const override { return "Seat Capacity"; }
    string getExtraValue() const override { return to_string(capacity) + " seats"; }
};

// ---------------- DERIVED CLASS 2: CARGO FLIGHT ----------------
class CargoFlight : public FlightBase
{
private:
    int cargoWeight;   // in kg

public:
    CargoFlight(string id, int spd, int alt, int fl, int pass, int weight)
        : FlightBase(id, spd, alt, fl, pass)
    {
        cargoWeight = weight;
        cout << "[OOP] CargoFlight constructor called (cargo "
             << cargoWeight << " kg)" << endl;
    }

    void setCargoWeight(int w) { if (w >= 0) cargoWeight = w; }
    int getCargoWeight() const { return cargoWeight; }

    // ===== POLYMORPHISM START =====
    // Cargo formula is DELIBERATELY different from Passenger's: it
    // has nothing to do with passenger load, and instead adds a
    // small amount of extra fuel for every kg of cargo on board -
    // the heavier the freight, the more it burns.
    float calculateFuel() const override
    {
        return getFuel() + cargoWeight * 0.002f;
    }

    void display() const override
    {
        FlightBase::display();
        cout << "   -> Cargo Weight: " << cargoWeight << " kg" << endl;
    }
    // ===== POLYMORPHISM END =====

    string getType() const override       { return "Cargo"; }
    string getClassName() const override  { return "CargoFlight"; }
    string getExtraLabel() const override { return "Cargo Weight"; }
    string getExtraValue() const override { return to_string(cargoWeight) + " kg"; }
};

// ---------------- DERIVED CLASS 3: PRIVATE JET ----------------
class PrivateJet : public FlightBase
{
private:
    int vipSeats;
    string ownerName;

public:
    PrivateJet(string id, int spd, int alt, int fl, int pass,
               int seats, string owner)
        : FlightBase(id, spd, alt, fl, pass)
    {
        vipSeats = seats;
        ownerName = owner;
        cout << "[OOP] PrivateJet constructor called (owner "
             << ownerName << ")" << endl;
    }

    void setVipSeats(int s)     { if (s >= 0) vipSeats = s; }
    int getVipSeats() const     { return vipSeats; }
    string getOwnerName() const { return ownerName; }

    // ===== POLYMORPHISM START =====
    // Private jet formula is the lightest of the three: small VIP
    // aircraft with few passengers fly more fuel-efficiently, so
    // this simply scales the base reading down.
    float calculateFuel() const override
    {
        return getFuel() * 0.6f;
    }

    void display() const override
    {
        FlightBase::display();
        cout << "   -> Owner: " << ownerName
             << "  VIP Seats: " << vipSeats << endl;
    }
    // ===== POLYMORPHISM END =====

    string getType() const override       { return "Private Jet"; }
    string getClassName() const override  { return "PrivateJet"; }
    string getExtraLabel() const override { return "Owner / VIP Seats"; }
    string getExtraValue() const override
    {
        return ownerName + " / " + to_string(vipSeats);
    }
};

// ---------------- OBJECT TABLE (one object per flight) ----------------
// Base class pointer array: PassengerFlight, CargoFlight and PrivateJet
// objects are all stored in one array (polymorphism).
FlightBase* flightObjects[MAX_FLIGHTS];

// ===== EXCEPTION HANDLING START =====
// Every FlightBase-derived constructor can now throw
// invalid_argument (negative speed, fuel > 100, or - for
// PassengerFlight - passengers > capacity). Each "new" call here
// is wrapped in its own try/catch: if one flight's data were ever
// invalid, the program prints the reason and falls back to a safe
// zeroed PassengerFlight for that slot instead of leaving a
// dangling pointer or crashing the whole startup.
// ===== EXCEPTION HANDLING END =====
void initFlightObjects()
{
    struct { int index; int capacity; } plan[MAX_FLIGHTS] =
    {
        { 0, 120 }, { 1, 150 }, { 2, 0 }, { 3, 0 }, { 4, 140 }
    };

    for (int i = 0; i < MAX_FLIGHTS; i++)
    {
        try
        {
            if (i == 2)
            {
                flightObjects[i] = new CargoFlight(flights[i].flightID, flights[i].speed,
                    flights[i].altitude, flights[i].fuel, flights[i].passengers, 45000);
            }
            else if (i == 3)
            {
                flightObjects[i] = new PrivateJet(flights[i].flightID, flights[i].speed,
                    flights[i].altitude, flights[i].fuel, flights[i].passengers,
                    12, "Aero Charter");
            }
            else
            {
                flightObjects[i] = new PassengerFlight(flights[i].flightID, flights[i].speed,
                    flights[i].altitude, flights[i].fuel, flights[i].passengers,
                    plan[i].capacity);
            }
        }
        catch (const invalid_argument& e)
        {
            cout << "\n[EXCEPTION] Invalid data for flight "
                 << flights[i].flightID << ": " << e.what()
                 << " -> using a safe placeholder object.\n";

            // Always-valid fallback so flightObjects[i] is never
            // left as a dangling/uninitialized pointer.
            flightObjects[i] = new PassengerFlight(flights[i].flightID, 0, 0, 0, 0, 0);
        }
    }
}

bool showOOPPanel = false;

int currentFlightIndex();   // forward declaration (defined below)

FlightBase* getCurrentFlightObject()
{
    int i = currentFlightIndex();

    if (i < 0 || i >= MAX_FLIGHTS)
        return NULL;

    return flightObjects[i];
}

void updateOOPFlightObject()
{
    if (currentNode == NULL)
        return;

    FlightBase* obj = getCurrentFlightObject();

    if (obj == NULL)
        return;

    obj->setFlightData(
        currentNode->data.flightID,
        currentNode->data.speed,
        currentNode->data.altitude,
        currentNode->data.fuel,
        currentNode->data.passengers
    );
}


// =====================================================
// SIMULATION STATE
// =====================================================

enum FlightPhase
{
    ON_GROUND,
    TAKING_OFF,
    CRUISING,
    LANDING
};

FlightPhase phase = ON_GROUND;

const float START_X = 120.0f;
const float RUNWAY_Y = 205.0f;
const float MAX_FLIGHT_Y = 575.0f;
const float LEFT_EDGE = 60.0f;
const float RIGHT_EDGE = 1130.0f;

float aircraftX = START_X;
float aircraftY = RUNWAY_Y;

float speedValue = 0.0f;
float altitudeValue = 0.0f;
float fuelValue = 100.0f;

bool paused = false;
bool waiting = false;

int selectedFlightIndex = 0;


// =====================================================
// WINDOW / DASHBOARD STATE
// =====================================================

const float MAIN_W = 1300.0f;
const float MAIN_H = 800.0f;
const float DASH_W = 1300.0f;
const float DASH_H = 800.0f;

int mainWindow = 0;
int dashboardWindow = 0;
bool dashboardOpen = false;

enum DashboardPage
{
    DASH_HOME = 0,
    DASH_FLIGHTS = 1,
    DASH_PASSENGERS = 2,
    DASH_SEARCH = 3,
    DASH_FLIGHT_DETAILS = 4,
    DASH_PASSENGER_DETAILS = 5,
    DASH_QUEUE = 6,
    // ===== COA CO4 PAGE START =====
    DASH_COA = 7,
    // ===== COA CO4 PAGE END =====
    // ===== HASHING START =====
    DASH_HASH = 8,
    // ===== HASHING END =====
    // ===== OOP DASHBOARD START =====
    DASH_OOP = 9,
    // ===== OOP DASHBOARD END =====
    // ===== SEARCH SORT START =====
    DASH_SORT = 10,
    // ===== SEARCH SORT END =====
    // ===== CLIPPING START =====
    DASH_CLIP = 11
    // ===== CLIPPING END =====
};

DashboardPage dashboardPage = DASH_HOME;

// ===== OOP DASHBOARD START =====
int oopView = 0;     // 0 overview, 1 inheritance, 2 polymorphism, 3 abstraction, 4 encapsulation
// ===== OOP DASHBOARD END =====

// ===== SEARCH SORT START =====
void sortPageEnter();    // defined with the page (resets both demos)
// ===== SEARCH SORT END =====

// ===== CLIPPING START =====
void clipPageEnter();    // defined with the page (recomputes the clip)
// ===== CLIPPING END =====

const int ROWS_PER_PAGE = 10;

int passengerPage = 0;
int selectedPassengerIndex = -1;
int searchResultIndex = -1;

string searchInput = "";
string searchMessage = "";
string queueMessage = "";

// Redraw both windows (main + secondary dashboard if open)
void redrawAll()
{
    if (mainWindow != 0)
        glutPostWindowRedisplay(mainWindow);

    if (dashboardOpen && dashboardWindow != 0)
        glutPostWindowRedisplay(dashboardWindow);
}


// =====================================================
// PASSENGER DATA + SEARCH SYSTEM
// =====================================================

struct Passenger
{
    int passengerID;
    string name;
    int age;
    string gender;
    string seat;
    string travelClass;
    string destination;
    string ticketStatus;
    string flightID;
};

// 100 + 120 + 150 + 80 + 110 = 560 detailed records.
const int MAX_PASSENGER_RECORDS = 560;

Passenger passengerRecords[MAX_PASSENGER_RECORDS];

int passengerRecordCount = 0;

// ===== PASSENGER DATABASE START =====
// =====================================================
// PASSENGER DATABASE
// AI101's 100 passengers are a hand-made list (name, age, gender,
// travel class). Every other flight gets realistic names from the
// name pools below. Rules for ALL 560 records:
//   - no two passengers share a full name
//   - no two passengers on one flight share a seat
//   - Business sits in rows 1-4, Economy from row 5
//   - IDs stay 101,102,... in flight order (same as before)
// Seat, ticket status and generated fields are fixed formulas, so
// the database is identical on every run.
// =====================================================

struct PassengerSeed
{
    const char* name;
    int age;
    char gender;    // 'M' / 'F'
    char cls;       // 'B' Business, 'E' Economy
};

const int CURATED_PASSENGERS = 100;

// IDs 101..200 = flight AI101
const PassengerSeed curatedPassengers[CURATED_PASSENGERS] =
{
    { "Aarav Sharma",      34, 'M', 'E' },   // 101
    { "Ananya Patil",      29, 'F', 'E' },   // 102
    { "Rohan Kulkarni",    41, 'M', 'B' },   // 103
    { "Priya Joshi",       27, 'F', 'E' },   // 104
    { "Yash Mehta",        22, 'M', 'E' },   // 105
    { "Kavya Nair",        31, 'F', 'E' },
    { "Arjun Reddy",       38, 'M', 'E' },
    { "Meera Iyer",        45, 'F', 'B' },
    { "Vihaan Gupta",      26, 'M', 'E' },
    { "Sneha Deshmukh",    33, 'F', 'E' },
    { "Rahul Verma",       52, 'M', 'B' },
    { "Ishita Banerjee",   28, 'F', 'E' },
    { "Siddharth Menon",   36, 'M', 'E' },
    { "Divya Pillai",      30, 'F', 'E' },
    { "Karan Malhotra",    40, 'M', 'B' },
    { "Pooja Shah",        35, 'F', 'E' },
    { "Aditya Rao",        24, 'M', 'E' },
    { "Neha Kapoor",       32, 'F', 'E' },
    { "Manish Agarwal",    47, 'M', 'E' },
    { "Riya Chopra",       21, 'F', 'E' },
    { "Vikram Singh",      43, 'M', 'B' },
    { "Anjali Trivedi",    37, 'F', 'E' },
    { "Nikhil Bansal",     29, 'M', 'E' },
    { "Shruti Naidu",      26, 'F', 'E' },
    { "Varun Bhatia",      34, 'M', 'E' },
    { "Tanvi Saxena",      23, 'F', 'E' },
    { "Sanjay Mishra",     55, 'M', 'E' },
    { "Aditi Chatterjee",  39, 'F', 'B' },
    { "Deepak Tiwari",     48, 'M', 'E' },
    { "Nisha Pandey",      31, 'F', 'E' },
    { "Amit Yadav",        42, 'M', 'E' },
    { "Swati Chauhan",     36, 'F', 'E' },
    { "Kunal Thakur",      27, 'M', 'E' },
    { "Komal Shinde",      25, 'F', 'E' },
    { "Harsh Pawar",       22, 'M', 'E' },
    { "Radhika Jadhav",    44, 'F', 'E' },
    { "Tejas More",        30, 'M', 'E' },
    { "Simran Arora",      28, 'F', 'E' },
    { "Omkar Kale",        33, 'M', 'E' },
    { "Preeti Sethi",      50, 'F', 'E' },
    { "Pranav Bose",       35, 'M', 'E' },
    { "Ritu Khanna",       46, 'F', 'B' },
    { "Ishaan Das",        19, 'M', 'E' },
    { "Sonal Jain",        32, 'F', 'E' },
    { "Mohit Sen",         38, 'M', 'E' },
    { "Payal Ghosh",       26, 'F', 'E' },
    { "Gaurav Mukherjee",  41, 'M', 'E' },
    { "Madhuri Desai",     53, 'F', 'E' },
    { "Ajay Patel",        49, 'M', 'E' },
    { "Aarti Fernandes",   34, 'F', 'E' },
    { "Sameer Khan",       37, 'M', 'E' },
    { "Bhavna Rao",        29, 'F', 'E' },
    { "Abhishek Nair",     31, 'M', 'E' },
    { "Chitra Menon",      58, 'F', 'E' },
    { "Akash Iyer",        24, 'M', 'E' },
    { "Deepa Reddy",       40, 'F', 'E' },
    { "Raghav Sharma",     45, 'M', 'B' },
    { "Gauri Kulkarni",    27, 'F', 'E' },
    { "Yuvraj Singh",      23, 'M', 'E' },
    { "Hema Pillai",       61, 'F', 'E' },
    { "Dhruv Mehta",       20, 'M', 'E' },
    { "Jyoti Verma",       43, 'F', 'E' },
    { "Naveen Gupta",      39, 'M', 'E' },
    { "Kiran Shah",        35, 'F', 'E' },
    { "Suresh Naidu",      57, 'M', 'E' },
    { "Lata Deshmukh",     64, 'F', 'E' },
    { "Ramesh Patil",      60, 'M', 'E' },
    { "Mansi Joshi",       22, 'F', 'E' },
    { "Anil Kapoor",       51, 'M', 'B' },
    { "Nandini Bhatia",    33, 'F', 'E' },
    { "Prakash Rao",       62, 'M', 'E' },
    { "Pallavi Sethi",     30, 'F', 'E' },
    { "Imran Qureshi",     36, 'M', 'E' },
    { "Rashmi Bansal",     38, 'F', 'E' },
    { "Faisal Ahmed",      42, 'M', 'E' },
    { "Sunita Yadav",      47, 'F', 'E' },
    { "Arman Sheikh",      25, 'M', 'E' },
    { "Trisha Dsouza",     28, 'F', 'E' },
    { "Daniel Fernandes",  34, 'M', 'E' },
    { "Zoya Ansari",       26, 'F', 'E' },
    { "Joseph Thomas",     44, 'M', 'E' },
    { "Fatima Begum",      52, 'F', 'E' },
    { "Thomas Mathew",     48, 'M', 'E' },
    { "Maria Pereira",     31, 'F', 'E' },
    { "Rajesh Chauhan",    54, 'M', 'E' },
    { "Lakshmi Iyer",      59, 'F', 'E' },
    { "Vivek Tiwari",      33, 'M', 'E' },
    { "Shweta Agarwal",    29, 'F', 'E' },
    { "Mahesh Jadhav",     46, 'M', 'E' },
    { "Sakshi Pawar",      24, 'F', 'E' },
    { "Ankit Chopra",      27, 'M', 'E' },
    { "Rupali Shinde",     35, 'F', 'E' },
    { "Tushar More",       30, 'M', 'E' },
    { "Vandana Mishra",    56, 'F', 'E' },
    { "Girish Pandey",     63, 'M', 'E' },
    { "Aryan Malhotra",     8, 'M', 'E' },
    { "Myra Sharma",        6, 'F', 'E' },
    { "Kabir Singh",       12, 'M', 'E' },
    { "Diya Patel",        10, 'F', 'E' },
    { "Rudra Verma",        4, 'M', 'E' }
};

// Name pools for the other flights (AI202 ... AI505)
const int DB_FIRST_NAMES = 40;
const int DB_SURNAMES = 50;

const char* dbMaleNames[DB_FIRST_NAMES] =
{
    "Arjun", "Vihaan", "Aditya", "Karan", "Rahul", "Siddharth", "Rohit", "Manish",
    "Nikhil", "Varun", "Sanjay", "Deepak", "Amit", "Vikram", "Kunal", "Harsh",
    "Tejas", "Omkar", "Pranav", "Ishaan", "Mohit", "Gaurav", "Ajay", "Sameer",
    "Abhishek", "Akash", "Raghav", "Yuvraj", "Dhruv", "Naveen", "Suresh", "Ramesh",
    "Anil", "Prakash", "Imran", "Faisal", "Arman", "Daniel", "Joseph", "Thomas"
};

const char* dbFemaleNames[DB_FIRST_NAMES] =
{
    "Neha", "Pooja", "Sneha", "Kavya", "Riya", "Isha", "Meera", "Anjali",
    "Divya", "Shruti", "Nisha", "Tanvi", "Aditi", "Swati", "Komal", "Radhika",
    "Simran", "Preeti", "Ritu", "Sonal", "Payal", "Madhuri", "Aarti", "Bhavna",
    "Chitra", "Deepa", "Gauri", "Hema", "Jyoti", "Kiran", "Lata", "Mansi",
    "Nandini", "Pallavi", "Rashmi", "Sunita", "Trisha", "Zoya", "Fatima", "Maria"
};

const char* dbSurnames[DB_SURNAMES] =
{
    "Sharma", "Verma", "Patil", "Joshi", "Mehta", "Kulkarni", "Deshmukh", "Gupta",
    "Singh", "Reddy", "Nair", "Iyer", "Menon", "Pillai", "Rao", "Naidu", "Das",
    "Banerjee", "Chatterjee", "Mukherjee", "Ghosh", "Sen", "Bose", "Shah", "Desai",
    "Patel", "Trivedi", "Jain", "Agarwal", "Bansal", "Kapoor", "Malhotra", "Khanna",
    "Chopra", "Bhatia", "Sethi", "Arora", "Saxena", "Mishra", "Tiwari", "Pandey",
    "Yadav", "Chauhan", "Thakur", "Shinde", "Pawar", "More", "Jadhav", "Khan",
    "Fernandes"
};

// generator state (reset by the first record / first passenger of a flight)
set<string> dbUsedNames;
int dbBizCursor = 0;
int dbEcoCursor = 24;

string dbSeatLabel(int n)
{
    return to_string(n / 6 + 1) + string(1, (char)('A' + n % 6));
}

// seats Passenger 101-105 already had in the old data
bool dbIsReservedSeat(const string& flightID, const string& seat)
{
    return flightID == "AI101" &&
           (seat == "12A" || seat == "12B" || seat == "14C" ||
            seat == "15A" || seat == "15B");
}

// Business = rows 1-4 (24 seats), Economy = row 5 onward
string dbNextSeat(const string& flightID, bool business)
{
    if (business)
    {
        while (dbBizCursor < 24)
        {
            string s = dbSeatLabel(dbBizCursor++);

            if (!dbIsReservedSeat(flightID, s))
                return s;
        }
    }

    while (true)
    {
        string s = dbSeatLabel(dbEcoCursor++);

        if (!dbIsReservedSeat(flightID, s))
            return s;
    }
}

void addPassengerRecord(
    int id,
    const string& flightID,
    int index,
    const string& destination)
{
    if (passengerRecordCount >= MAX_PASSENGER_RECORDS)
        return;

    if (passengerRecordCount == 0)
        dbUsedNames.clear();

    if (index == 0)
    {
        dbBizCursor = 0;
        dbEcoCursor = 24;
    }

    Passenger& p = passengerRecords[passengerRecordCount];

    p.passengerID = id;

    // stable pseudo-random number per ID (same on every run)
    unsigned h = (unsigned)id * 2654435761u;
    h ^= h >> 15;
    h *= 2246822519u;
    h ^= h >> 13;

    bool business = false;
    bool curated = (flightID == "AI101" && index < CURATED_PASSENGERS);

    if (curated)
    {
        const PassengerSeed& s = curatedPassengers[index];

        p.name = s.name;
        p.age = s.age;
        p.gender = (s.gender == 'M') ? "Male" : "Female";
        business = (s.cls == 'B');
    }
    else
    {
        bool male = ((h & 1u) == 0u);
        const char* const* pool = male ? dbMaleNames : dbFemaleNames;

        int first = (int)((h >> 3) % DB_FIRST_NAMES);
        int last = (int)((h >> 10) % DB_SURNAMES);
        string name;

        // walk until the full name is unused
        for (int a = 0; a < DB_FIRST_NAMES; a++)
        {
            bool found = false;

            for (int t = 0; t < DB_SURNAMES; t++)
            {
                name = string(pool[(first + a) % DB_FIRST_NAMES]) + " " +
                       dbSurnames[(last + t) % DB_SURNAMES];

                if (dbUsedNames.count(name) == 0)
                {
                    found = true;
                    break;
                }
            }

            if (found)
                break;
        }

        p.name = name;
        p.gender = male ? "Male" : "Female";

        unsigned band = (h >> 7) % 100u;
        unsigned spread = h >> 16;

        if (band < 6)
            p.age = 3 + (int)(spread % 13u);          // children
        else if (band < 90)
            p.age = 18 + (int)(spread % 40u);         // adults
        else
            p.age = 58 + (int)(spread % 20u);         // seniors

        business = (p.age >= 18) && ((h >> 5) % 100u < 12u);
    }

    dbUsedNames.insert(p.name);

    // seat: the five original seats are kept for 101-105
    if (flightID == "AI101" && index < 5)
    {
        static const char* fixedSeats[5] = { "12A", "12B", "14C", "15A", "15B" };
        p.seat = fixedSeats[index];
    }
    else
    {
        p.seat = dbNextSeat(flightID, business);
    }

    p.travelClass = business ? "Business" : "Economy";
    p.destination = destination;

    unsigned st = (h >> 20) % 100u;

    if (st < 55)       p.ticketStatus = "Confirmed";
    else if (st < 80)  p.ticketStatus = "Checked-In";
    else if (st < 92)  p.ticketStatus = "Boarded";
    else               p.ticketStatus = "Waitlisted";

    p.flightID = flightID;

    passengerRecordCount++;
}
// ===== PASSENGER DATABASE END =====

void initializePassengerRecords()
{
    passengerRecordCount = 0;

    int startingID = 101;

    for (int flightIndex = 0;
         flightIndex < MAX_FLIGHTS;
         flightIndex++)
    {
        for (int i = 0;
             i < flights[flightIndex].passengers;
             i++)
        {
            addPassengerRecord(
                startingID + i,
                flights[flightIndex].flightID,
                i,
                destinationNames[flightIndex]
            );
        }

        startingID += flights[flightIndex].passengers;
    }

    cout << "\n[PASSENGER DATABASE] "
         << passengerRecordCount
         << " detailed passenger records loaded ("
         << CURATED_PASSENGERS << " hand-made for AI101)."
         << endl;
}

void toUpperText(string& value)
{
    for (size_t i = 0; i < value.length(); i++)
    {
        value[i] = (char)toupper((unsigned char)value[i]);
    }
}

int findFlightIndexByID(const string& id)
{
    string key = id;
    toUpperText(key);

    for (int i = 0; i < MAX_FLIGHTS; i++)
    {
        string current = flights[i].flightID;

        toUpperText(current);

        if (current == key)
            return i;
    }

    return -1;
}

int findPassengerIndexByID(int id)
{
    for (int i = 0; i < passengerRecordCount; i++)
    {
        if (passengerRecords[i].passengerID == id)
            return i;
    }

    return -1;
}

// ===== STL START =====
// =====================================================
// STL CONTAINERS + ALGORITHMS
// Added ALONGSIDE the existing arrays/linked list (nothing is
// replaced) so the two approaches can be compared in a viva:
//   vector<Passenger>      - dynamic-array copy of passengerRecords[]
//   map<int, Passenger*>   - Passenger ID -> record, O(log n) search
//                            (the old findPassengerIndexByID() above
//                            is a linear O(n) loop)
//   vector<FlightBase*>    - STL twin of the flightObjects[] array
//   sort() + lambda        - sortable Flight List
//   count_if / find_if     - Home page statistic cards
// =====================================================

vector<Passenger> passengerVector;
map<int, Passenger*> passengerMap;

// Builds the vector copy and the ID map once, after
// initializePassengerRecords() has filled passengerRecords[].
// The map stores pointers into the (never-moving) array, so an
// index can be recovered with plain pointer subtraction.
void buildPassengerSTL()
{
    passengerVector.assign(passengerRecords,
                           passengerRecords + passengerRecordCount);

    passengerMap.clear();

    for (int i = 0; i < passengerRecordCount; i++)
        passengerMap[passengerRecords[i].passengerID] = &passengerRecords[i];

    cout << "[STL] vector<Passenger> size = " << passengerVector.size()
         << " | map<int, Passenger*> size = " << passengerMap.size()
         << endl;
}

// O(log n) lookup through the map. Returns the array index of the
// passenger (same value the linear search returned), or -1.
int findPassengerIndexByMap(int id)
{
    map<int, Passenger*>::iterator it = passengerMap.find(id);

    if (it == passengerMap.end())
        return -1;

    return (int)(it->second - passengerRecords);
}

// ---- count_if / find_if statistics (Home page cards) ----

int countBusinessPassengers()
{
    return (int)count_if(passengerVector.begin(), passengerVector.end(),
        [](const Passenger& p) { return p.travelClass == "Business"; });
}

int countLowFuelFlights()
{
    return (int)count_if(flights, flights + MAX_FLIGHTS,
        [](const Flight& f) { return f.fuel < 80; });
}

// find_if: ID of the first flight whose fuel is below 80%
string firstLowFuelFlightID()
{
    const Flight* it = find_if(flights, flights + MAX_FLIGHTS,
        [](const Flight& f) { return f.fuel < 80; });

    if (it == flights + MAX_FLIGHTS)
        return "None";

    return string(it->flightID);
}

// ---- sort() + lambda for the Flight List page ----
// The sorted order is a separate vector of indices into flights[],
// so flights[] / the linked list / flightObjects[] are never
// reordered - only the way the list is DISPLAYED changes.

vector<int> flightSortOrder;
int flightSortMode = 0;   // 0 original, 1 speed, 2 fuel, 3 passengers

void applyFlightSort()
{
    flightSortOrder.resize(MAX_FLIGHTS);

    for (int i = 0; i < MAX_FLIGHTS; i++)
        flightSortOrder[i] = i;

    if (flightSortMode == 1)
        sort(flightSortOrder.begin(), flightSortOrder.end(),
             [](int a, int b) { return flights[a].speed > flights[b].speed; });
    else if (flightSortMode == 2)
        sort(flightSortOrder.begin(), flightSortOrder.end(),
             [](int a, int b) { return flights[a].fuel > flights[b].fuel; });
    else if (flightSortMode == 3)
        sort(flightSortOrder.begin(), flightSortOrder.end(),
             [](int a, int b) { return flights[a].passengers > flights[b].passengers; });
}

void cycleFlightSort()
{
    flightSortMode = (flightSortMode + 1) % 4;
    applyFlightSort();
}

string getFlightSortLabel()
{
    switch (flightSortMode)
    {
        case 1:  return "Speed (high to low)";
        case 2:  return "Fuel (high to low)";
        case 3:  return "Passengers (high to low)";
        default: return "Original order";
    }
}

// STL twin of flightObjects[] - same polymorphic pointers in a
// std::vector. Filled by buildFlightVector() in main().
vector<FlightBase*> flightVector;

void buildFlightVector()
{
    flightVector.assign(flightObjects, flightObjects + MAX_FLIGHTS);
}
// ===== STL END =====

// Passenger records are stored flight by flight,
// so the first record of a flight can be calculated.
int getFlightStartIndex(int flightIndex)
{
    int start = 0;

    for (int i = 0; i < flightIndex && i < MAX_FLIGHTS; i++)
        start += flights[i].passengers;

    return start;
}


// =====================================================
// BASIC DRAWING
// =====================================================

void drawRect(
    float x1,
    float y1,
    float x2,
    float y2)
{
    glBegin(GL_QUADS);

    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);

    glEnd();
}

void drawText(
    float x,
    float y,
    const string& text,
    void* font = GLUT_BITMAP_HELVETICA_18)
{
    glRasterPos2f(x, y);

    for (size_t i = 0; i < text.length(); i++)
    {
        glutBitmapCharacter(
            font,
            text[i]
        );
    }
}

void drawCenteredText(
    float centerX,
    float y,
    const string& text,
    void* font = GLUT_BITMAP_HELVETICA_18)
{
    int width = 0;

    for (size_t i = 0; i < text.length(); i++)
    {
        width += glutBitmapWidth(
            font,
            text[i]
        );
    }

    drawText(
        centerX - width / 2.0f,
        y,
        text,
        font
    );
}

void setDark()  { glColor3f(0.035f, 0.045f, 0.075f); }
void setPanel() { glColor3f(0.075f, 0.095f, 0.14f); }
void setBlue()  { glColor3f(0.08f, 0.42f, 0.78f); }
void setGreen() { glColor3f(0.08f, 0.65f, 0.32f); }
void setWhite() { glColor3f(0.94f, 0.96f, 1.0f); }
void setGray()  { glColor3f(0.65f, 0.68f, 0.74f); }
void setRed()   { glColor3f(0.82f, 0.18f, 0.20f); }


// =====================================================
// BUTTONS (drawing + click detection share one table)
// =====================================================

struct Btn
{
    float x1, y1, x2, y2;
    const char* label;
    float r, g, b;
};

bool inBtn(const Btn& b, float x, float y)
{
    return x >= b.x1 && x <= b.x2 &&
           y >= b.y1 && y <= b.y2;
}

void drawButton(const Btn& b, bool active)
{
    float add = active ? 0.18f : 0.0f;

    glColor3f(
        fmin(1.0f, b.r + add),
        fmin(1.0f, b.g + add),
        fmin(1.0f, b.b + add)
    );

    drawRect(b.x1, b.y1, b.x2, b.y2);

    setWhite();

    drawCenteredText(
        (b.x1 + b.x2) / 2.0f,
        (b.y1 + b.y2) / 2.0f - 6.0f,
        b.label
    );
}

// ---------------- MAIN WINDOW BUTTONS ----------------

const int MAIN_BTN_COUNT = 8;

Btn mainBtns[MAIN_BTN_COUNT] =
{
    {  40, 20,  400, 95, "OPEN MANAGEMENT DASHBOARD", 0.08f, 0.42f, 0.78f },
    { 420, 20,  532, 95, "TAKEOFF", 0.10f, 0.65f, 0.25f },
    { 542, 20,  654, 95, "LAND",    0.80f, 0.30f, 0.20f },
    { 664, 20,  776, 95, "PAUSE",   0.75f, 0.50f, 0.08f },
    { 786, 20,  898, 95, "WAIT",    0.60f, 0.35f, 0.15f },
    { 908, 20, 1020, 95, "RESET",   0.48f, 0.40f, 0.75f },
    {1030, 20, 1142, 95, "PREV",    0.16f, 0.25f, 0.35f },
    {1152, 20, 1264, 95, "NEXT",    0.16f, 0.25f, 0.35f }
};

// ---------------- DASHBOARD SIDEBAR BUTTONS ----------------
// Index 0-6 match the DashboardPage enum, index 7 = close.

// COA CO4 PAGE: one extra button (index 7), so CLOSE moves to index 8.
// HASHING: one more button (index 8), so CLOSE moves to index 9.
// OOP DASHBOARD: one more button (index 9), CLOSE moves to index 10.
// SEARCH SORT: one more button (index 10), CLOSE moves to index 11.
// CLIPPING: one more button (index 11), CLOSE moves to index 12.
const int SIDEBAR_BTN_COUNT = 13;

// array index = DashboardPage value (goToPage(i)); the y coordinates decide
// the ON-SCREEN ORDER. SEARCH & SORT (index 10) is drawn 4th, right after
// PASSENGER LIST.
Btn dashBtns[SIDEBAR_BTN_COUNT] =
{
    { 20, 647, 280, 690, "HOME", 0.08f, 0.30f, 0.60f },
    { 20, 598, 280, 641, "FLIGHT LIST", 0.08f, 0.30f, 0.60f },
    { 20, 549, 280, 592, "PASSENGER LIST", 0.08f, 0.30f, 0.60f },
    { 20, 451, 280, 494, "PASSENGER SEARCH", 0.08f, 0.30f, 0.60f },
    { 20, 402, 280, 445, "FLIGHT DETAILS", 0.08f, 0.30f, 0.60f },
    { 20, 353, 280, 396, "PASSENGER DETAILS", 0.08f, 0.30f, 0.60f },
    { 20, 304, 280, 347, "WAITING QUEUE", 0.08f, 0.30f, 0.60f },
    { 20, 255, 280, 298, "CONTROL / PIPELINE", 0.55f, 0.22f, 0.55f },
    { 20, 206, 280, 249, "HASH TABLE", 0.10f, 0.50f, 0.50f },
    { 20, 157, 280, 200, "OOP CONCEPTS", 0.50f, 0.35f, 0.15f },
    { 20, 500, 280, 543, "SEARCH & SORT", 0.35f, 0.45f, 0.15f },
    // ===== CLIPPING START =====
    { 20, 108, 280, 151, "CLIPPING", 0.20f, 0.40f, 0.45f },
    // ===== CLIPPING END =====
    { 20,  40, 280,  95, "CLOSE DASHBOARD", 0.75f, 0.18f, 0.20f }
};

// ---------------- DASHBOARD CONTENT BUTTONS ----------------

Btn flightsOpenBtn = { 320, 220,  620, 270, "OPEN FLIGHT DETAILS",     0.08f, 0.42f, 0.78f };
// ===== STL START =====
Btn flightsSortBtn = { 640, 220,  940, 270, "SORT: CYCLE SPEED/FUEL/PASS", 0.60f, 0.35f, 0.15f };
// ===== STL END =====

Btn passPrevBtn    = { 320,  50,  470,  95, "PREV PAGE",               0.16f, 0.25f, 0.35f };
Btn passNextBtn    = { 485,  50,  635,  95, "NEXT PAGE",               0.16f, 0.25f, 0.35f };
Btn passOpenBtn    = { 900,  50, 1270,  95, "OPEN PASSENGER DETAILS",  0.08f, 0.42f, 0.78f };

Btn searchBtn      = { 760, 500,  930, 550, "SEARCH",                  0.08f, 0.42f, 0.78f };
Btn searchViewBtn  = { 830, 180, 1090, 230, "VIEW FULL DETAILS",      0.08f, 0.42f, 0.78f };

Btn fdPassBtn      = { 930, 520, 1270, 575, "VIEW PASSENGER LIST",     0.08f, 0.42f, 0.78f };
Btn fdPrevBtn      = { 930, 440, 1090, 490, "PREV FLIGHT",             0.16f, 0.25f, 0.35f };
Btn fdNextBtn      = {1105, 440, 1265, 490, "NEXT FLIGHT",             0.16f, 0.25f, 0.35f };

Btn queueAddBtn    = { 320, 230,  660, 285, "ADD CURRENT FLIGHT TO QUEUE", 0.60f, 0.35f, 0.15f };
Btn queueDepartBtn = { 680, 230, 1020, 285, "DEPART NEXT FLIGHT",          0.10f, 0.65f, 0.25f };


// =====================================================
// CLOUD
// =====================================================

void drawCloud(
    float x,
    float y)
{
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_POLYGON);

    for (int i = 0; i < 360; i += 10)
    {
        float angle = i * 3.14159f / 180.0f;

        glVertex2f(
            x + 70.0f * cos(angle),
            y + 30.0f * sin(angle)
        );
    }

    glEnd();

    glBegin(GL_POLYGON);

    for (int i = 0; i < 360; i += 10)
    {
        float angle = i * 3.14159f / 180.0f;

        glVertex2f(
            x + 35.0f + 45.0f * cos(angle),
            y + 20.0f + 25.0f * sin(angle)
        );
    }

    glEnd();
}


// ===== BRESENHAM SUN START =====
// =====================================================
// SUN (drawn with a manual Bresenham Circle Algorithm
// and a manual Bresenham Line Algorithm, kept separate
// from all flight-management / AeroDesk logic)
// =====================================================

// Animation state for the Sun only. sunRotationDeg spins the
// ray pattern; sunGlowPhase drives a slow brightness "breathing"
// (simple lighting effect) on the glow halo and the core.
float sunRotationDeg = 0.0f;
float sunGlowPhase   = 0.0f;

// Advances the Sun's rotation and glow pulse by one simulation
// tick. Called once per frame from update(), independent of
// flight pause/wait state, so the Sun keeps animating always.
void updateSunAnimation()
{
    sunRotationDeg += 0.6f;

    if (sunRotationDeg >= 360.0f)
        sunRotationDeg -= 360.0f;

    sunGlowPhase += 0.045f;

    if (sunGlowPhase >= 6.2832f)   // wrap after one full 2*PI cycle
        sunGlowPhase -= 6.2832f;
}

// Draws only the circumference of a circle using the
// midpoint (Bresenham) circle algorithm with 8-way symmetry.
// Uses whatever color was set with glColor3f() before calling.
void drawBresenhamCircle(int xc, int yc, int r)
{
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    glBegin(GL_POINTS);

    while (x <= y)
    {
        // 8-way symmetry: one computed point gives 8 points
        glVertex2i(xc + x, yc + y);
        glVertex2i(xc - x, yc + y);
        glVertex2i(xc + x, yc - y);
        glVertex2i(xc - x, yc - y);
        glVertex2i(xc + y, yc + x);
        glVertex2i(xc - y, yc + x);
        glVertex2i(xc + y, yc - x);
        glVertex2i(xc - y, yc - x);

        if (d < 0)
        {
            d += 4 * x + 6;
        }
        else
        {
            d += 4 * (x - y) + 10;
            y--;
        }

        x++;
    }

    glEnd();
}

// Fills a circle by running the SAME Bresenham circle
// algorithm and, for every (x, y) pair it finds, filling a
// horizontal scanline between the symmetric left/right points
// instead of just plotting 8 single pixels.
//
// ===== DASHBOARD TEXTURE START (CG QUALITY FIX) =====
// Each scanline is filled with drawRect() - a real quad with
// geometric height - instead of a raw GL_LINES segment. A
// GL_LINES row is only ever 1 DEVICE pixel thick no matter how
// the window is resized, so when the window is stretched larger
// than the native 1300x800 design size, those 1px rows leave
// visible gaps (grey stripes across the Sun's glow halo). A
// drawRect() quad has real height in ortho units, so it always
// scales with the window and the fill stays solid.
// ===== DASHBOARD TEXTURE END (CG QUALITY FIX) =====
void drawFilledBresenhamCircle(int xc, int yc, int r)
{
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    while (x <= y)
    {
        // top/bottom spans from the (x, y) pair
        drawRect((float)(xc - x), (float)(yc + y), (float)(xc + x), (float)(yc + y + 1));
        drawRect((float)(xc - x), (float)(yc - y), (float)(xc + x), (float)(yc - y + 1));

        // top/bottom spans from the mirrored (y, x) pair
        drawRect((float)(xc - y), (float)(yc + x), (float)(xc + y), (float)(yc + x + 1));
        drawRect((float)(xc - y), (float)(yc - x), (float)(xc + y), (float)(yc - x + 1));

        if (d < 0)
        {
            d += 4 * x + 6;
        }
        else
        {
            d += 4 * (x - y) + 10;
            y--;
        }

        x++;
    }
}

// Manual Bresenham Line Algorithm (integer-only, no GL_LINES
// shortcut) used to draw each individual Sun ray pixel by pixel.
void drawBresenhamLine(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int err = dx - dy;

    int x = x1;
    int y = y1;

    glBegin(GL_POINTS);

    while (true)
    {
        glVertex2i(x, y);

        if (x == x2 && y == y2)
            break;

        int e2 = 2 * err;

        if (e2 > -dy)
        {
            err -= dy;
            x += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            y += sy;
        }
    }

    glEnd();
}

// Draws the complete Sun: a soft blended glow halo (lighting
// effect, pulses slowly), rotating alternating-length rays, then
// a filled disk built from three concentric Bresenham circles
// (orange outer, yellow-orange middle, bright yellow center),
// then a bright outline circle.
void drawSun()
{
    const int SUN_X = 950;
    const int SUN_Y = 600;
    const int SUN_R = 46;

    const int   RAY_COUNT    = 12;
    const float RAY_INNER    = SUN_R + 6.0f;
    const float RAY_OUTER_A  = SUN_R + 34.0f;  // long rays
    const float RAY_OUTER_B  = SUN_R + 20.0f;  // short rays (alternate)

    // Brightness "breathing" factor, oscillating between 0.0 and 1.0.
    float pulse = 0.5f + 0.5f * sin(sunGlowPhase);

    // ---------- GLOW / LIGHTING HALO ----------
    // ADDITIVE blending (GL_SRC_ALPHA, GL_ONE) brightens the sky
    // underneath like real light. GL_BLEND itself stays enabled
    // (turned on once in initOrtho()); only the blend FUNCTION is
    // changed here and restored afterwards.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glColor4f(1.0f, 0.80f, 0.35f, 0.05f + 0.04f * pulse);
    drawFilledBresenhamCircle(SUN_X, SUN_Y, (int)(SUN_R * 1.5f));

    glColor4f(1.0f, 0.78f, 0.30f, 0.08f + 0.06f * pulse);
    drawFilledBresenhamCircle(SUN_X, SUN_Y, (int)(SUN_R * 1.2f));

    // Restore normal alpha blending for the rest of the scene.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ---------- RAYS (rotating, alternating length) ----------
    glColor3f(1.0f, 0.55f, 0.0f);
    glPointSize(3.0f);

    for (int i = 0; i < RAY_COUNT; i++)
    {
        float baseAngle = i * (360.0f / RAY_COUNT) + sunRotationDeg;
        float angle = baseAngle * 3.14159f / 180.0f;

        // Rays alternate between a long and a short reach
        float rayOuter = (i % 2 == 0) ? RAY_OUTER_A : RAY_OUTER_B;

        int x1 = SUN_X + (int)(RAY_INNER * cos(angle));
        int y1 = SUN_Y + (int)(RAY_INNER * sin(angle));

        int x2 = SUN_X + (int)(rayOuter * cos(angle));
        int y2 = SUN_Y + (int)(rayOuter * sin(angle));

        drawBresenhamLine(x1, y1, x2, y2);
    }

    glPointSize(1.0f);

    // ---------- SUN DISK (concentric Bresenham circles) ----------
    // Outer ring - orange
    glColor3f(1.0f, 0.45f, 0.0f);
    drawFilledBresenhamCircle(SUN_X, SUN_Y, SUN_R);

    // Inner ring - yellow-orange
    glColor3f(1.0f, 0.70f, 0.02f);
    drawFilledBresenhamCircle(SUN_X, SUN_Y, (int)(SUN_R * 0.68f));

    // Center - bright yellow core, brightness pulses slightly
    glColor3f(1.0f, 0.78f + 0.10f * pulse, 0.05f + 0.10f * pulse);
    drawFilledBresenhamCircle(SUN_X, SUN_Y, (int)(SUN_R * 0.40f));

    // Crisp bright outline on the outer edge for definition
    glPointSize(2.0f);
    glColor3f(1.0f, 0.95f, 0.55f);
    drawBresenhamCircle(SUN_X, SUN_Y, SUN_R);
    glPointSize(1.0f);
}
// ===== BRESENHAM SUN END =====


// ===== SUN MOON SWITCH START =====
// =====================================================
// MOON
// Shown instead of the Sun once the day/night cycle has turned
// dark. Built with the same technique as the Sun - the existing
// drawFilledBresenhamCircle() for the disk and craters,
// drawBresenhamCircle() for the outline, and alpha blending for
// a soft night-glow halo. No image/texture file is used.
// =====================================================

void drawMoon()
{
    // Same sky position the Sun uses, so the switch is seamless.
    const int MOON_X = 950;
    const int MOON_Y = 600;
    const int MOON_R = 40;

    // ---------- SOFT NIGHT-GLOW HALO ----------
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glColor4f(0.75f, 0.80f, 0.95f, 0.06f);
    drawFilledBresenhamCircle(MOON_X, MOON_Y, (int)(MOON_R * 1.5f));

    glColor4f(0.80f, 0.85f, 1.00f, 0.10f);
    drawFilledBresenhamCircle(MOON_X, MOON_Y, (int)(MOON_R * 1.2f));

    // Restore normal alpha blending for the rest of the scene.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ---------- MOON DISK (concentric Bresenham circles) ----------
    glColor3f(0.80f, 0.82f, 0.88f);
    drawFilledBresenhamCircle(MOON_X, MOON_Y, MOON_R);

    glColor3f(0.88f, 0.90f, 0.95f);
    drawFilledBresenhamCircle(MOON_X, MOON_Y, (int)(MOON_R * 0.75f));

    // ---------- CRATERS ----------
    glColor3f(0.65f, 0.68f, 0.74f);
    drawFilledBresenhamCircle(MOON_X - 14, MOON_Y + 10, 6);
    drawFilledBresenhamCircle(MOON_X + 10, MOON_Y - 8,  8);
    drawFilledBresenhamCircle(MOON_X + 2,  MOON_Y + 16, 4);
    drawFilledBresenhamCircle(MOON_X - 6,  MOON_Y - 14, 5);

    // ---------- CRISP OUTLINE ----------
    glPointSize(2.0f);
    glColor3f(0.95f, 0.96f, 1.0f);
    drawBresenhamCircle(MOON_X, MOON_Y, MOON_R);
    glPointSize(1.0f);
}
// ===== SUN MOON SWITCH END =====


// ===== FLYING BIRDS START =====
// =====================================================
// FLYING BIRDS
// Simple animated birds that cross the sky left to right.
// Wings are drawn with the EXISTING drawBresenhamLine()
// function - no duplicate line-drawing algorithm is created.
// =====================================================

struct Bird
{
    float x;            // current X position
    float y;            // current Y position (stays in the sky band)
    float speed;        // horizontal speed, different per bird
    float wingPhase;    // animation phase used for wing flapping
};

const int BIRD_COUNT = 6;

// Sky band the birds fly in: below the AERODESK header and above
// the terminal roof / control tower.
const float BIRD_MIN_Y = 530.0f;
const float BIRD_MAX_Y = 680.0f;

Bird birds[BIRD_COUNT];

// Gives every bird a starting X (staggered), a Y inside the sky
// band, and its own speed.
void initializeBirds()
{
    float startX[BIRD_COUNT]   = { 100.0f, 350.0f, 560.0f, 780.0f, 980.0f, 1180.0f };
    float startY[BIRD_COUNT]   = { 650.0f, 560.0f, 620.0f, 590.0f, 660.0f, 540.0f };
    float startSpd[BIRD_COUNT] = { 0.9f,   1.6f,   1.2f,   2.0f,   1.4f,   1.8f };

    for (int i = 0; i < BIRD_COUNT; i++)
    {
        birds[i].x = startX[i];
        birds[i].y = startY[i];
        birds[i].speed = startSpd[i];
        birds[i].wingPhase = (float)i * 1.3f;   // different phase per bird
    }
}

// Draws one bird as two wing strokes meeting at (x, y), using
// the existing Bresenham line routine. wingOffset (driven by sine)
// raises/lowers the wingtips each frame = flapping motion.
void drawBird(float x, float y, float wingOffset)
{
    const float WING_SPAN = 22.0f;
    const float WING_BASE = 9.0f;

    glColor3f(0.0f, 0.0f, 0.0f);     // solid black - strong contrast on blue sky
    glPointSize(3.5f);

    // Left wing
    drawBresenhamLine(
        (int)x, (int)y,
        (int)(x - WING_SPAN), (int)(y + WING_BASE + wingOffset)
    );

    // Right wing
    drawBresenhamLine(
        (int)x, (int)y,
        (int)(x + WING_SPAN), (int)(y + WING_BASE + wingOffset)
    );

    // Second, 1px-offset pass so the wing reads as a solid stroke
    drawBresenhamLine(
        (int)x, (int)(y + 1),
        (int)(x - WING_SPAN), (int)(y + WING_BASE + wingOffset + 1)
    );

    drawBresenhamLine(
        (int)x, (int)(y + 1),
        (int)(x + WING_SPAN), (int)(y + WING_BASE + wingOffset + 1)
    );

    glPointSize(1.0f);

    // Small round body at the hinge point
    drawFilledBresenhamCircle((int)x, (int)y, 3);
}

void drawAllBirds()
{
    for (int i = 0; i < BIRD_COUNT; i++)
    {
        float flap = sin(birds[i].wingPhase) * 8.0f;   // -8..+8 px swing
        drawBird(birds[i].x, birds[i].y, flap);
    }
}

// Moves every bird right by its own speed, advances its wing-flap
// phase, and wraps it back to the left (with a fresh Y) once it
// flies off the right edge. Called from the existing update() timer.
void updateBirds()
{
    for (int i = 0; i < BIRD_COUNT; i++)
    {
        birds[i].x += birds[i].speed;

        birds[i].wingPhase += 0.25f;

        if (birds[i].wingPhase > 6.2832f)
            birds[i].wingPhase -= 6.2832f;

        // Off the right edge -> respawn on the left with a new Y
        if (birds[i].x > MAIN_W + 50.0f)
        {
            birds[i].x = -50.0f;
            birds[i].y = BIRD_MIN_Y + (float)(rand() % (int)(BIRD_MAX_Y - BIRD_MIN_Y));
        }
    }
}
// ===== FLYING BIRDS END =====


// ===== PARALLAX CLOUDS START =====
// =====================================================
// PARALLAX CLOUDS
// A second, animated layer of clouds on top of the two original
// fixed-position ones. These drift continuously left -> right,
// each at its own speed and size (far = small + slow, near =
// big + fast). Reuses the existing drawCloud() shape; only
// glTranslatef()/glScalef() position and resize each copy.
// =====================================================

struct ParallaxCloud
{
    float x, y;       // current position
    float speed;      // drift speed - lower = feels farther away
    float scale;      // size multiplier - smaller = feels farther away
};

const int PARALLAX_CLOUD_COUNT = 6;
ParallaxCloud parallaxClouds[PARALLAX_CLOUD_COUNT];

void initParallaxClouds()
{
    // Far layer - small and slow
    parallaxClouds[0] = { 150.0f, 665.0f, 0.15f, 0.55f };
    parallaxClouds[1] = { 700.0f, 675.0f, 0.15f, 0.55f };

    // Mid layer
    parallaxClouds[2] = { 400.0f, 635.0f, 0.35f, 0.80f };
    parallaxClouds[3] = { 950.0f, 615.0f, 0.35f, 0.80f };

    // Near layer - large and fast
    parallaxClouds[4] = { 100.0f, 590.0f, 0.60f, 1.10f };
    parallaxClouds[5] = { 780.0f, 560.0f, 0.60f, 1.10f };
}

// Drifts every cloud right at its own speed and wraps it back to
// the left once it has gone fully off-screen.
void updateParallaxClouds()
{
    for (int i = 0; i < PARALLAX_CLOUD_COUNT; i++)
    {
        parallaxClouds[i].x += parallaxClouds[i].speed;

        if (parallaxClouds[i].x > MAIN_W + 150.0f)
            parallaxClouds[i].x = -150.0f;
    }
}

// Draws one parallax cloud: push the matrix, move/scale it,
// draw the existing cloud shape at the local origin, pop back.
void drawParallaxCloud(const ParallaxCloud& c)
{
    glPushMatrix();

    glTranslatef(c.x, c.y, 0.0f);
    glScalef(c.scale, c.scale, 1.0f);

    drawCloud(0.0f, 0.0f);

    glPopMatrix();
}

void drawAllParallaxClouds()
{
    for (int i = 0; i < PARALLAX_CLOUD_COUNT; i++)
        drawParallaxCloud(parallaxClouds[i]);
}
// ===== PARALLAX CLOUDS END =====


// ===== DAY NIGHT CYCLE START =====
// =====================================================
// DAY / NIGHT CYCLE
// Drives a slow, continuous day -> sunset -> night -> sunset ->
// day loop that the sky gradient and the terminal windows react
// to. updateDayNightCycle() is called once per tick from update().
// =====================================================

float dayPhase = 0.0f;              // 0..2*PI, position in the cycle

// Radians the cycle advances per tick (timer ticks every 40 ms).
// Smaller = slower, longer day.
const float DAY_NIGHT_SPEED = 0.006f;

// Linear interpolation: a at t = 0, b at t = 1.
float lerpF(float a, float b, float t)
{
    return a + (b - a) * t;
}

// Converts dayPhase into a 0..1 "darkness" factor using a cosine:
//   0.0 = full day, 0.5 = sunset / dusk, 1.0 = full night
float getDayNightFactor()
{
    return (1.0f - cos(dayPhase)) / 2.0f;
}

// Advances the cycle by one tick (independent of pause/wait).
void updateDayNightCycle()
{
    dayPhase += DAY_NIGHT_SPEED;

    if (dayPhase >= 6.2832f)   // wrap after one full 2*PI cycle
        dayPhase -= 6.2832f;
}

// Terminal window glass color: cool blue by day, warm yellow
// "interior lights on" glow from dusk through the night.
void getTerminalWindowColor(float outColor[3])
{
    float dayGlass[3]  = { 0.0f, 0.65f, 0.85f };   // original window color
    float nightGlow[3] = { 1.0f, 0.82f, 0.30f };   // warm lit-window yellow

    float t = getDayNightFactor();

    // Lights only switch on from dusk onward.
    float glowT = (t - 0.45f) / 0.55f;

    if (glowT < 0.0f) glowT = 0.0f;
    if (glowT > 1.0f) glowT = 1.0f;

    outColor[0] = lerpF(dayGlass[0], nightGlow[0], glowT);
    outColor[1] = lerpF(dayGlass[1], nightGlow[1], glowT);
    outColor[2] = lerpF(dayGlass[2], nightGlow[2], glowT);
}
// ===== DAY NIGHT CYCLE END =====


// ===== STAR FIELD START =====
// =====================================================
// STAR FIELD
// Small white points that fade in as dusk falls and fade out at
// dawn, driven by the SAME getDayNightFactor(). Each star also
// twinkles (brightness pulses with sin()). Drawn with
// glPointSize() + GL_POINTS.
// =====================================================

struct Star
{
    float x;             // fixed X position in the sky
    float y;             // fixed Y position in the sky
    float twinklePhase;  // own phase so stars don't blink together
    float twinkleSpeed;  // slightly different pulse speed per star
    float size;          // point size, varies star to star
};

const int STAR_COUNT = 55;

// Stars stay in the open sky band, below the header bar and above
// the terminal/tower skyline.
const float STAR_MIN_Y = 260.0f;
const float STAR_MAX_Y = 695.0f;

Star stars[STAR_COUNT];

void initStars()
{
    for (int i = 0; i < STAR_COUNT; i++)
    {
        stars[i].x = (float)(rand() % (int)MAIN_W);
        stars[i].y = STAR_MIN_Y + (float)(rand() % (int)(STAR_MAX_Y - STAR_MIN_Y));
        stars[i].twinklePhase = (float)(rand() % 628) / 100.0f;   // 0..2*PI
        stars[i].twinkleSpeed = 0.03f + (float)(rand() % 40) / 1000.0f;
        stars[i].size = 1.2f + (float)(rand() % 10) / 10.0f;       // 1.2..2.1
    }
}

void updateStars()
{
    for (int i = 0; i < STAR_COUNT; i++)
    {
        stars[i].twinklePhase += stars[i].twinkleSpeed;

        if (stars[i].twinklePhase > 6.2832f)
            stars[i].twinklePhase -= 6.2832f;
    }
}

// Visibility is tied to the day-night factor: invisible through
// the day, fading in from dusk onward, with each star's own
// twinkle layered on top.
void drawAllStars()
{
    float t = getDayNightFactor();

    float nightT = (t - 0.45f) / 0.55f;
    if (nightT < 0.0f) nightT = 0.0f;
    if (nightT > 1.0f) nightT = 1.0f;

    if (nightT <= 0.0f)
        return;   // broad daylight - nothing to draw

    for (int i = 0; i < STAR_COUNT; i++)
    {
        float twinkle = 0.5f + 0.5f * sin(stars[i].twinklePhase);  // 0..1
        float alpha = nightT * (0.35f + 0.65f * twinkle);

        glColor4f(1.0f, 1.0f, 1.0f, alpha);
        glPointSize(stars[i].size);

        glBegin(GL_POINTS);
        glVertex2f(stars[i].x, stars[i].y);
        glEnd();
    }

    glPointSize(1.0f);
}
// ===== STAR FIELD END =====


// ===== CAMERA TRANSFORM START =====
// =====================================================
// CAMERA ZOOM / PAN
// While the aircraft is TAKING_OFF, the "world" layer is zoomed
// in and panned toward the aircraft using real OpenGL matrix
// transformations (glTranslatef / glScalef). The Sky and the
// fixed header/button-panel HUD are drawn OUTSIDE this transform.
// =====================================================

float cameraZoom = 1.0f;
float cameraPanX = 0.0f;
float cameraPanY = 0.0f;

// Pivot the zoom scales around: middle of the visible world area.
const float CAMERA_PIVOT_X = 650.0f;
const float CAMERA_PIVOT_Y = 400.0f;

// Eases the camera's zoom/pan toward its target every tick.
void updateCamera()
{
    float targetZoom = 1.0f;
    float targetPanX = 0.0f;
    float targetPanY = 0.0f;

    if (phase == TAKING_OFF)
    {
        targetZoom = 1.15f;

        targetPanX = -(aircraftX - START_X) * 0.18f;
        targetPanY = -(aircraftY - RUNWAY_Y) * 0.12f;

        // Clamp the pan so the zoomed world never reveals empty space.
        if (targetPanX >  80.0f) targetPanX =  80.0f;
        if (targetPanX < -80.0f) targetPanX = -80.0f;
        if (targetPanY >  50.0f) targetPanY =  50.0f;
        if (targetPanY < -50.0f) targetPanY = -50.0f;
    }

    const float EASE = 0.05f;   // smaller = slower, smoother camera

    cameraZoom = lerpF(cameraZoom, targetZoom, EASE);
    cameraPanX = lerpF(cameraPanX, targetPanX, EASE);
    cameraPanY = lerpF(cameraPanY, targetPanY, EASE);
}

// Applies the current camera zoom/pan to the modelview matrix.
void applyCameraTransform()
{
    glTranslatef(cameraPanX, cameraPanY, 0.0f);

    glTranslatef(CAMERA_PIVOT_X, CAMERA_PIVOT_Y, 0.0f);
    glScalef(cameraZoom, cameraZoom, 1.0f);
    glTranslatef(-CAMERA_PIVOT_X, -CAMERA_PIVOT_Y, 0.0f);
}
// ===== CAMERA TRANSFORM END =====


// ===== ENGINE SMOKE START =====
// =====================================================
// ENGINE EXHAUST / SMOKE
// Small particle system: while the aircraft is TAKING_OFF, puffs
// are spawned at the engine position; each puff drifts backward,
// grows, and fades out. Reuses drawFilledBresenhamCircle().
// =====================================================

struct SmokeParticle
{
    float x, y;       // current position
    float size;       // current puff radius (grows over time)
    float alpha;      // current opacity (shrinks over time)
    bool  active;     // false = free slot, ready to be reused
};

const int SMOKE_COUNT = 120;
SmokeParticle smokeParticles[SMOKE_COUNT];

// Puffs released on every tick while taking off.
const int SMOKE_SPAWN_PER_TICK = 3;

void initEngineSmoke()
{
    for (int i = 0; i < SMOKE_COUNT; i++)
        smokeParticles[i].active = false;
}

// Activates one free slot at the engine position (with a small
// random jitter). If every slot is in use, the call is skipped.
void spawnSmokeParticle(float engineX, float engineY)
{
    for (int i = 0; i < SMOKE_COUNT; i++)
    {
        if (!smokeParticles[i].active)
        {
            smokeParticles[i].x = engineX + (float)(rand() % 7 - 3);
            smokeParticles[i].y = engineY + (float)(rand() % 7 - 3);
            smokeParticles[i].size = 4.0f;
            smokeParticles[i].alpha = 0.70f;
            smokeParticles[i].active = true;
            return;
        }
    }
}

void spawnSmokeBurst(float engineX, float engineY)
{
    for (int i = 0; i < SMOKE_SPAWN_PER_TICK; i++)
        spawnSmokeParticle(engineX, engineY);
}

// Drifts each puff behind/below the aircraft, grows it, fades it.
// A fully faded puff is deactivated so its slot can be reused.
void updateEngineSmoke()
{
    for (int i = 0; i < SMOKE_COUNT; i++)
    {
        if (!smokeParticles[i].active)
            continue;

        smokeParticles[i].x -= 1.1f;
        smokeParticles[i].y -= 0.35f;
        smokeParticles[i].size += 0.12f;
        smokeParticles[i].alpha -= 0.009f;

        if (smokeParticles[i].alpha <= 0.0f)
            smokeParticles[i].active = false;
    }
}

void drawEngineSmoke()
{
    for (int i = 0; i < SMOKE_COUNT; i++)
    {
        if (!smokeParticles[i].active)
            continue;

        glColor4f(0.60f, 0.60f, 0.64f, smokeParticles[i].alpha);

        drawFilledBresenhamCircle(
            (int)smokeParticles[i].x,
            (int)smokeParticles[i].y,
            (int)smokeParticles[i].size
        );
    }
}
// ===== ENGINE SMOKE END =====


// ===== DASHBOARD TEXTURE START =====
// =====================================================
// DASHBOARD TEXTURE
// Procedural (shape-based) texturing - no external image files:
//   - a banded sky gradient instead of one flat sky color
//   - a speckled asphalt pattern on the runway
//   - panel/seam lines on the terminal and control tower
// =====================================================

const int SKY_BAND_COUNT = 12;

// Draws the sky as stacked horizontal bands that fade from a
// deeper color at the top to a lighter color at the horizon.
//
// ===== DAY NIGHT CYCLE START =====
// The two band colors (top/zenith and horizon) are picked by
// blending between a day, a sunset and a night palette using
// getDayNightFactor() (0 = day, 0.5 = sunset, 1 = night).
void drawSkyGradient()
{
    float dayTop[3]        = { 0.16f, 0.42f, 0.74f };
    float dayHorizon[3]    = { 0.62f, 0.83f, 0.95f };

    float sunsetTop[3]     = { 0.22f, 0.16f, 0.34f };
    float sunsetHorizon[3] = { 1.00f, 0.55f, 0.25f };

    float nightTop[3]      = { 0.02f, 0.03f, 0.09f };
    float nightHorizon[3]  = { 0.06f, 0.09f, 0.20f };

    float t = getDayNightFactor();   // 0 = day .. 0.5 = sunset .. 1 = night

    float topColor[3];
    float horizonColor[3];

    if (t <= 0.5f)
    {
        // Day -> sunset half of the cycle
        float localT = t / 0.5f;

        for (int i = 0; i < 3; i++)
        {
            topColor[i]     = lerpF(dayTop[i],     sunsetTop[i],     localT);
            horizonColor[i] = lerpF(dayHorizon[i], sunsetHorizon[i], localT);
        }
    }
    else
    {
        // Sunset -> night half of the cycle
        float localT = (t - 0.5f) / 0.5f;

        for (int i = 0; i < 3; i++)
        {
            topColor[i]     = lerpF(sunsetTop[i],     nightTop[i],     localT);
            horizonColor[i] = lerpF(sunsetHorizon[i], nightHorizon[i], localT);
        }
    }
    // ===== DAY NIGHT CYCLE END =====

    float bandHeight = MAIN_H / (float)SKY_BAND_COUNT;

    for (int i = 0; i < SKY_BAND_COUNT; i++)
    {
        // i = 0 is the bottom band (horizon), top band = zenith.
        float t = (float)i / (float)(SKY_BAND_COUNT - 1);

        float r = horizonColor[0] + (topColor[0] - horizonColor[0]) * t;
        float g = horizonColor[1] + (topColor[1] - horizonColor[1]) * t;
        float b = horizonColor[2] + (topColor[2] - horizonColor[2]) * t;

        glColor3f(r, g, b);

        float y1 = i * bandHeight;
        float y2 = y1 + bandHeight + 1.0f;   // +1 avoids thin seam gaps

        drawRect(0, y1, MAIN_W, y2);
    }
}

// ---------------- Runway asphalt speckle texture ----------------

struct RunwaySpeckle
{
    float x, y;
    bool light;    // true = lighter fleck, false = darker fleck
};

const int RUNWAY_SPECKLE_COUNT = 180;
RunwaySpeckle runwaySpeckles[RUNWAY_SPECKLE_COUNT];

// Pre-computes every speckle ONCE (called from main()) so the
// texture stays stable instead of flickering on every redraw.
void initRunwayTexture()
{
    for (int i = 0; i < RUNWAY_SPECKLE_COUNT; i++)
    {
        runwaySpeckles[i].x = 25.0f + (float)(rand() % 1245);
        runwaySpeckles[i].y = 174.0f + (float)(rand() % 68);
        runwaySpeckles[i].light = (rand() % 2 == 0);
    }
}

void drawRunwayTexture()
{
    for (int i = 0; i < RUNWAY_SPECKLE_COUNT; i++)
    {
        if (runwaySpeckles[i].light)
            glColor3f(0.34f, 0.34f, 0.36f);
        else
            glColor3f(0.10f, 0.10f, 0.12f);

        drawRect(
            runwaySpeckles[i].x,
            runwaySpeckles[i].y,
            runwaySpeckles[i].x + 3.0f,
            runwaySpeckles[i].y + 2.0f
        );
    }
}

// ---------------- Terminal / control tower panel texture ----------------

void drawTerminalTexture()
{
    // (Terminal facade mullions/seams now come from the real texture:
    //  see drawTexturedTerminal() in the TEXTURE MAPPING block.)

    // Control tower shaft - horizontal banding
    glColor3f(0.50f, 0.50f, 0.58f);

    for (float ty = 255.0f; ty < 470.0f; ty += 30.0f)
        drawRect(1010.0f, ty, 1070.0f, ty + 2.0f);

    // Control tower cabin - vertical glass mullions
    for (float tx = 985.0f; tx < 1105.0f; tx += 20.0f)
        drawRect(tx, 470.0f, tx + 2.0f, 520.0f);
}
// ===== DASHBOARD TEXTURE END =====


// ===== TEXTURE MAPPING START =====
// =====================================================
// REAL TEXTURE MAPPING (glTexImage2D + glTexCoord2f)
// Three textures are generated in code (no image files) and
// mapped onto GL_QUADS:
//   texAsphalt : noisy dark grey          -> RUNWAY
//   texFacade  : grey panels + seam lines -> TERMINAL BODY
//   texChecker : checkerboard             -> APRON
// GL_MODULATE multiplies the texture with the current colour, so
// the textures get darker at night (same getDayNightFactor()).
//
// PASTE THIS WHOLE BLOCK in AeroDesk right after the
// "DASHBOARD TEXTURE END" block that follows drawTerminalTexture()
// (it needs lerpF() and getDayNightFactor(), defined earlier).
// =====================================================

const int TEX_N = 64;                       // every texture is 64 x 64

GLubyte imgAsphalt[TEX_N][TEX_N][3];
GLubyte imgFacade[TEX_N][TEX_N][3];
GLubyte imgChecker[TEX_N][TEX_N][3];

GLuint texAsphalt = 0;
GLuint texFacade  = 0;
GLuint texChecker = 0;

void texSetPixel(GLubyte img[TEX_N][TEX_N][3], int i, int j, int r, int g, int b)
{
    img[i][j][0] = (GLubyte)r;
    img[i][j][1] = (GLubyte)g;
    img[i][j][2] = (GLubyte)b;
}

void makeTextureImages()
{
    for (int i = 0; i < TEX_N; i++)
    {
        for (int j = 0; j < TEX_N; j++)
        {
            // asphalt: dark grey with random light / dark flecks
            int n = rand() % 100;
            int base = 50 + rand() % 14;

            if (n < 6)       base += 40;
            else if (n < 12) base -= 25;

            texSetPixel(imgAsphalt, i, j, base, base, base + 3);

            // facade: light panel, darker seam on the first 2 texels
            // of every row and column (looks like panel joints)
            bool seam = (i < 2) || (j < 2);

            if (seam)
                texSetPixel(imgFacade, i, j, 120, 124, 138);
            else
                texSetPixel(imgFacade, i, j, 190, 198, 220);

            // checkerboard: 2 x 2 squares of 32 x 32 texels
            bool light = (((i / 32) + (j / 32)) % 2 == 0);

            if (light)
                texSetPixel(imgChecker, i, j, 150, 150, 158);
            else
                texSetPixel(imgChecker, i, j, 60, 60, 68);
        }
    }
}

GLuint uploadTexture(GLubyte img[TEX_N][TEX_N][3])
{
    GLuint id = 0;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_N, TEX_N, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, img);

    return id;
}

// Call ONCE from main(), right after initOrtho() of the MAIN window
// (an OpenGL context must exist before glGenTextures()).
void initTextures()
{
    makeTextureImages();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    texAsphalt = uploadTexture(imgAsphalt);
    texFacade  = uploadTexture(imgFacade);
    texChecker = uploadTexture(imgChecker);

    cout << "[TEXTURE] 3 textures created (asphalt, facade, checker)." << endl;
}

// One textured quad. sRep / tRep = how many times the texture
// repeats along the quad (texture coordinates 0..sRep, 0..tRep).
void drawTexturedRect(GLuint tex, float x1, float y1, float x2, float y2,
                      float sRep, float tRep)
{
    // night makes the textures darker (1.0 = day, 0.45 = night)
    float shade = lerpF(1.0f, 0.45f, getDayNightFactor());

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glColor3f(shade, shade, shade);

    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);   glVertex2f(x1, y1);
    glTexCoord2f(sRep, 0.0f);   glVertex2f(x2, y1);
    glTexCoord2f(sRep, tRep);   glVertex2f(x2, y2);
    glTexCoord2f(0.0f, tRep);   glVertex2f(x1, y2);

    glEnd();

    glDisable(GL_TEXTURE_2D);   // text and plain shapes must stay untextured
}

void drawTexturedRunway()   { drawTexturedRect(texAsphalt, 20, 170, 1280, 245, 19.7f, 1.17f); }
void drawTexturedTerminal() { drawTexturedRect(texFacade, 170, 245, 930, 430, 19.0f, 4.1f); }
void drawTexturedApron()    { drawTexturedRect(texChecker, 170, 172, 930, 243, 10.7f, 1.0f); }
// ===== TEXTURE MAPPING END =====



// ===== LANDING GEAR START =====
// =====================================================
// LANDING GEAR
// Wheel-and-strut assemblies under the fuselage that extend on
// the ground / near takeoff and landing, and retract once
// airborne. Extension is computed live from phase / altitudeValue.
// =====================================================

// 1.0 = gear fully down, 0.0 = fully retracted.
float getGearExtension()
{
    if (currentNode == NULL || phase == ON_GROUND)
        return 1.0f;

    float targetAltitude = (float)currentNode->data.altitude;

    if (targetAltitude <= 0.0f)
        targetAltitude = 1.0f;   // guard against divide-by-zero

    if (phase == TAKING_OFF)
    {
        // Gear retracts during the first 20% of the climb.
        float ratio = altitudeValue / (targetAltitude * 0.20f);
        float ext = 1.0f - ratio;

        if (ext < 0.0f) ext = 0.0f;
        if (ext > 1.0f) ext = 1.0f;

        return ext;
    }

    if (phase == CRUISING)
        return 0.0f;

    if (phase == LANDING)
    {
        // Gear extends again during the last 25% of the descent.
        float ratio = 1.0f - (altitudeValue / (targetAltitude * 0.25f));

        if (ratio < 0.0f) ratio = 0.0f;
        if (ratio > 1.0f) ratio = 1.0f;

        return ratio;
    }

    return 1.0f;
}

void drawOneWheel(float baseX, float baseY, float extension)
{
    const float STRUT_LEN = 16.0f;

    float len = STRUT_LEN * extension;
    float wheelY = baseY - len;

    // Strut
    glColor3f(0.20f, 0.20f, 0.23f);
    drawRect(baseX - 1.5f, wheelY, baseX + 1.5f, baseY);

    // Wheel (existing Bresenham circle fill + outline)
    glColor3f(0.05f, 0.05f, 0.07f);
    drawFilledBresenhamCircle((int)baseX, (int)wheelY, 5);

    glColor3f(0.45f, 0.45f, 0.48f);
    drawBresenhamCircle((int)baseX, (int)wheelY, 5);
}

// Nose wheel + two main wheels under the fuselage.
void drawLandingGear(float x, float y)
{
    float ext = getGearExtension();

    if (ext <= 0.02f)
        return;   // fully retracted - nothing to draw

    drawOneWheel(x + 138.0f, y + 1.0f, ext);   // nose wheel
    drawOneWheel(x + 50.0f, y + 1.0f, ext);    // main gear
    drawOneWheel(x + 72.0f, y + 1.0f, ext);
}
// ===== LANDING GEAR END =====


// ===== ENGINE TURBINE START =====
// =====================================================
// ENGINE TURBINE / PROPELLER-BLUR ILLUSION
// Bresenham lines spinning fast inside the engine intake. Spin
// speed is tied to speedValue.
// =====================================================

float engineRotationDeg = 0.0f;

void updateEngineRotation()
{
    engineRotationDeg += 6.0f + speedValue * 0.9f;

    if (engineRotationDeg >= 360.0f)
        engineRotationDeg -= 360.0f;
}

void drawEngineTurbine(float cx, float cy)
{
    const int   BLADE_COUNT = 6;
    const float BLADE_LEN   = 5.0f;

    glColor3f(0.55f, 0.55f, 0.60f);   // light grey = spinning blur look

    for (int i = 0; i < BLADE_COUNT; i++)
    {
        float angle = (engineRotationDeg + i * (360.0f / BLADE_COUNT)) *
                      3.14159f / 180.0f;

        int x2 = (int)(cx + BLADE_LEN * cos(angle));
        int y2 = (int)(cy + BLADE_LEN * sin(angle));

        drawBresenhamLine((int)cx, (int)cy, x2, y2);
    }
}
// ===== ENGINE TURBINE END =====


// ===== AIRPORT BEACON START =====
// =====================================================
// AIRPORT BEACON
// Rotating/blinking red-white light on top of the control tower.
// Blink = sine-driven red/white swap; rotation = two Bresenham
// beams sweeping around the beacon.
// =====================================================

float beaconRotationDeg = 0.0f;
float beaconBlinkPhase  = 0.0f;

void updateBeacon()
{
    beaconRotationDeg += 5.0f;

    if (beaconRotationDeg >= 360.0f)
        beaconRotationDeg -= 360.0f;

    beaconBlinkPhase += 0.15f;

    if (beaconBlinkPhase >= 6.2832f)
        beaconBlinkPhase -= 6.2832f;
}

void drawAirportBeacon()
{
    const int BEACON_X = 1040;
    const int BEACON_Y = 530;

    // Short pole from the tower cabin roof up to the beacon
    glColor3f(0.20f, 0.20f, 0.23f);
    drawRect(BEACON_X - 1.0f, 520.0f, BEACON_X + 1.0f, BEACON_Y - 4.0f);

    float blink = (sin(beaconBlinkPhase) + 1.0f) / 2.0f;   // 0..1

    float r, g, b;

    if (blink > 0.5f)
    {
        r = 1.0f; g = 0.10f; b = 0.10f;   // red
    }
    else
    {
        r = 1.0f; g = 1.0f; b = 1.0f;     // white
    }

    // Rotating sweep beams (two opposite beams)
    glColor3f(r, g, b);

    for (int i = 0; i < 2; i++)
    {
        float angle = (beaconRotationDeg + i * 180.0f) * 3.14159f / 180.0f;

        int x2 = BEACON_X + (int)(40.0f * cos(angle));
        int y2 = BEACON_Y + (int)(12.0f * sin(angle));

        drawBresenhamLine(BEACON_X, BEACON_Y, x2, y2);
    }

    // Lamp housing itself
    drawFilledBresenhamCircle(BEACON_X, BEACON_Y, 5);

    glColor3f(0.15f, 0.15f, 0.17f);
    drawBresenhamCircle(BEACON_X, BEACON_Y, 5);
}
// ===== AIRPORT BEACON END =====


// ===== APRON MARKINGS START =====
// =====================================================
// TAXIWAY / APRON MARKINGS
// A lighter concrete apron patch in front of the terminal, with
// yellow boundary and guide lines.
// =====================================================

void drawApronMarkings()
{
    // Apron panel - lighter concrete color than the runway
    // ===== TEXTURE MAPPING USED: checkerboard texture on apron =====
    drawTexturedApron();

    // Yellow boundary lines (top and bottom edge of the apron)
    glColor3f(1.0f, 0.85f, 0.0f);
    drawRect(170.0f, 240.0f, 930.0f, 243.0f);
    drawRect(170.0f, 172.0f, 930.0f, 175.0f);

    // Yellow guide line down the middle of the apron
    drawRect(548.0f, 175.0f, 552.0f, 240.0f);
}
// ===== APRON MARKINGS END =====


// ===== REALISTIC AIRCRAFT START =====
// =====================================================
// DRAW AIRCRAFT
// Airliner-like silhouette: tapered fuselage, shaded belly,
// cockpit windshield, round cabin windows, cheatline stripe,
// swept wings with engine pod and winglet, tail fin + stabilizer.
// =====================================================

void drawAircraft(
    float x,
    float y)
{
    // ---- MAIN WING (far side) ----
    glColor3f(0.20f, 0.25f, 0.35f);
    glBegin(GL_POLYGON);
    glVertex2f(x + 55, y + 14);
    glVertex2f(x + 15, y - 38);
    glVertex2f(x + 35, y - 38);
    glVertex2f(x + 90, y + 14);
    glEnd();

    // Wingtip winglet
    glColor3f(0.15f, 0.20f, 0.30f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x + 15, y - 38);
    glVertex2f(x + 8,  y - 52);
    glVertex2f(x + 26, y - 44);
    glEnd();

    // Engine nacelle hanging under the main wing
    glColor3f(0.12f, 0.12f, 0.15f);
    drawRect(x + 35, y - 30, x + 65, y - 18);
    glColor3f(0.30f, 0.30f, 0.34f);
    drawFilledBresenhamCircle((int)(x + 35), (int)(y - 24), 6);
    glColor3f(0.65f, 0.65f, 0.68f);
    drawBresenhamCircle((int)(x + 35), (int)(y - 24), 6);

    // ===== ENGINE TURBINE START =====
    drawEngineTurbine(x + 35.0f, y - 24.0f);
    // ===== ENGINE TURBINE END =====

    // ---- TOP WING (near side) ----
    glColor3f(0.17f, 0.22f, 0.32f);
    glBegin(GL_POLYGON);
    glVertex2f(x + 78,  y + 16);
    glVertex2f(x + 100, y + 58);
    glVertex2f(x + 112, y + 58);
    glVertex2f(x + 96,  y + 16);
    glEnd();

    // ---- TAIL (vertical fin) ----
    glColor3f(0.10f, 0.20f, 0.55f);
    glBegin(GL_POLYGON);
    glVertex2f(x - 8,  y + 16);
    glVertex2f(x - 25, y + 68);
    glVertex2f(x + 5,  y + 70);
    glVertex2f(x + 22, y + 24);
    glEnd();

    // Tail fin accent tip
    glColor3f(1.0f, 0.55f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 25, y + 68);
    glVertex2f(x - 18, y + 70);
    glVertex2f(x - 10, y + 60);
    glEnd();

    // Horizontal stabilizer
    glColor3f(0.15f, 0.22f, 0.32f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 2,  y + 14);
    glVertex2f(x - 22, y + 2);
    glVertex2f(x + 10, y + 6);
    glEnd();

    // ---- FUSELAGE ----
    glColor3f(0.93f, 0.94f, 0.97f);
    glBegin(GL_POLYGON);
    glVertex2f(x - 10, y + 14);
    glVertex2f(x + 15, y + 26);
    glVertex2f(x + 70, y + 29);
    glVertex2f(x + 130, y + 25);
    glVertex2f(x + 172, y + 15);
    glVertex2f(x + 130, y + 5);
    glVertex2f(x + 70,  y + 1);
    glVertex2f(x + 15,  y + 4);
    glEnd();

    // Belly shading
    glColor3f(0.74f, 0.76f, 0.81f);
    glBegin(GL_POLYGON);
    glVertex2f(x - 10, y + 14);
    glVertex2f(x + 15, y + 9);
    glVertex2f(x + 70, y + 7);
    glVertex2f(x + 130, y + 8);
    glVertex2f(x + 172, y + 15);
    glVertex2f(x + 130, y + 5);
    glVertex2f(x + 70,  y + 1);
    glVertex2f(x + 15,  y + 4);
    glEnd();

    // Cheatline stripe
    glColor3f(1.0f, 0.55f, 0.0f);
    drawRect(x - 6, y + 10, x + 150, y + 13);

    // Cockpit windshield wedge
    glColor3f(0.05f, 0.10f, 0.18f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x + 145, y + 23);
    glVertex2f(x + 168, y + 16);
    glVertex2f(x + 145, y + 9);
    glEnd();

    // Nose-cone highlight
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x + 160, y + 19);
    glVertex2f(x + 172, y + 15);
    glVertex2f(x + 160, y + 12);
    glEnd();

    // ---- CABIN WINDOWS (round, Bresenham) ----
    for (float wx = x + 20.0f; wx <= x + 128.0f; wx += 11.0f)
    {
        glColor3f(0.75f, 0.92f, 1.0f);
        drawFilledBresenhamCircle((int)wx, (int)(y + 17), 3);

        glColor3f(0.05f, 0.05f, 0.08f);
        drawBresenhamCircle((int)wx, (int)(y + 17), 3);
    }

    // ===== LANDING GEAR START =====
    drawLandingGear(x, y);
    // ===== LANDING GEAR END =====
}
// ===== REALISTIC AIRCRAFT END =====


// =====================================================
// LINKED LIST FUNCTIONS
// =====================================================

FlightNode* createNode(Flight data)
{
    FlightNode* newNode = new FlightNode;

    newNode->data = data;
    newNode->next = NULL;

    return newNode;
}

void insertFlight(Flight data)
{
    FlightNode* newNode = createNode(data);

    if (head == NULL)
    {
        head = newNode;
        return;
    }

    FlightNode* temp = head;

    while (temp->next != NULL)
        temp = temp->next;

    temp->next = newNode;
}

void buildFlightList()
{
    head = NULL;

    for (int i = 0; i < MAX_FLIGHTS; i++)
        insertFlight(flights[i]);

    currentNode = head;
    selectedFlightIndex = 0;

    updateOOPFlightObject();
}

void displayFlightList()
{
    cout << "\n====================================\n";
    cout << "       AERODESK FLIGHT LINKED LIST\n";
    cout << "====================================\n";

    FlightNode* temp = head;

    while (temp != NULL)
    {
        cout << temp->data.flightID << " -> ";
        temp = temp->next;
    }

    cout << "NULL\n";
    cout << "====================================\n";
}

// Returns position of current node (1 to MAX_FLIGHTS)
int getCurrentIndex()
{
    FlightNode* temp = head;
    int index = 1;

    while (temp != NULL)
    {
        if (temp == currentNode)
            return index;

        temp = temp->next;
        index++;
    }

    return 1;
}

// Returns position of current node (0 to MAX_FLIGHTS - 1)
int currentFlightIndex()
{
    return getCurrentIndex() - 1;
}

string getStatusString()
{
    if (paused)
        return "PAUSED";

    if (waiting)
        return "WAITING";

    if (phase == TAKING_OFF)
        return "TAKING OFF";

    if (phase == CRUISING)
        return "FLYING";

    if (phase == LANDING)
        return "LANDING";

    return "READY";
}


// =====================================================
// FLIGHT CONTROL FUNCTIONS
// =====================================================

void resetAircraftState()
{
    aircraftX = START_X;
    aircraftY = RUNWAY_Y;

    speedValue = 0.0f;
    altitudeValue = 0.0f;

    if (currentNode != NULL)
        fuelValue = (float)currentNode->data.fuel;

    phase = ON_GROUND;

    paused = false;

    // A flight is "waiting" only if it is really inside the queue.
    waiting = (currentNode != NULL &&
               isFlightInQueue(currentNode->data.flightID));

    passengerPage = 0;
}

void resetFlightSimulation()
{
    if (currentNode == NULL)
        return;

    removeFlightFromQueue(currentNode->data.flightID);

    resetAircraftState();

    pushOperation(
        string("RESET - ") +
        currentNode->data.flightID
    );
}

// Select a flight by its position in the linked list.
// The queue is NOT touched here, so many flights can wait together.
void selectFlightByIndex(int index)
{
    if (index < 0 || index >= MAX_FLIGHTS)
        return;

    FlightNode* temp = head;
    int i = 0;

    while (temp != NULL && i < index)
    {
        temp = temp->next;
        i++;
    }

    if (temp == NULL || temp == currentNode)
        return;

    currentNode = temp;
    selectedFlightIndex = index;

    resetAircraftState();
    updateOOPFlightObject();

    cout << "\nCurrent Flight: "
         << currentNode->data.flightID
         << endl;

    redrawAll();
}

void nextFlight()
{
    if (currentFlightIndex() + 1 < MAX_FLIGHTS)
        selectFlightByIndex(currentFlightIndex() + 1);
    else
        cout << "\nAlready at last flight.\n";

    redrawAll();
}

void previousFlight()
{
    if (currentFlightIndex() > 0)
        selectFlightByIndex(currentFlightIndex() - 1);
    else
        cout << "\nAlready at first flight.\n";

    redrawAll();
}

void startTakeoff()
{
    if (currentNode == NULL)
        return;

    if (phase == ON_GROUND)
    {
        if (waiting)
        {
            removeFlightFromQueue(currentNode->data.flightID);
            waiting = false;
        }

        phase = TAKING_OFF;

        pushOperation(
            string("TAKEOFF - ") +
            currentNode->data.flightID
        );

        paused = false;
        speedValue = 20.0f;

        cout << "\n====================================\n";
        cout << "          TAKEOFF STARTED\n";
        cout << "====================================\n";
        cout << "Flight: " << currentNode->data.flightID << endl;
    }
}

void startLanding()
{
    if (currentNode == NULL)
        return;

    if (phase == TAKING_OFF || phase == CRUISING)
    {
        phase = LANDING;

        pushOperation(
            string("LANDING - ") +
            currentNode->data.flightID
        );

        paused = false;

        removeFlightFromQueue(currentNode->data.flightID);
        waiting = false;

        cout << "\nLANDING STARTED\n";
    }
}

void toggleWaiting()
{
    if (currentNode == NULL)
        return;

    if (waiting)
    {
        removeFlightFromQueue(currentNode->data.flightID);

        waiting = false;

        pushOperation(
            string("RESUME - ") +
            currentNode->data.flightID
        );

        cout << "\nFlight removed from waiting queue.\n";
    }
    else
    {
        // ===== EXCEPTION HANDLING START =====
        // enqueueFlight() now THROWS runtime_error on overflow
        // instead of returning false, so this call must be inside
        // a try/catch. queueMessage is set in the catch block so
        // the dashboard (Waiting Queue page) shows the same error
        // even though this action was triggered from the main
        // window's 'W' key, not the dashboard itself.
        try
        {
            if (enqueueFlight(currentNode->data.flightID))
            {
                waiting = true;

                pushOperation(
                    string("WAIT - ") +
                    currentNode->data.flightID
                );

                cout << "\nFlight added to waiting queue.\n";
            }
        }
        catch (const runtime_error& e)
        {
            queueMessage = e.what();
            cout << "\n[EXCEPTION] " << e.what() << endl;
        }
        // ===== EXCEPTION HANDLING END =====
    }
}

// ---------------- QUEUE ACTIONS (used by dashboard) ----------------

void addCurrentFlightToQueue()
{
    if (currentNode == NULL)
        return;

    string id = currentNode->data.flightID;

    if (isFlightInQueue(id))
    {
        queueMessage = id + " is already waiting.";
        return;
    }

    // ===== EXCEPTION HANDLING START =====
    // enqueueFlight() throws runtime_error on overflow instead of
    // returning false - caught here and shown via queueMessage,
    // exactly where it used to be set from the old boolean result.
    try
    {
        if (enqueueFlight(id))
        {
            waiting = true;

            pushOperation("WAIT - " + id);

            queueMessage = id + " added to queue.";
        }
    }
    catch (const runtime_error& e)
    {
        queueMessage = e.what();
    }
    // ===== EXCEPTION HANDLING END =====
}

void departNextFlight()
{
    // ===== EXCEPTION HANDLING START =====
    // dequeueFlight() throws runtime_error on underflow instead of
    // returning "" - caught here and shown via queueMessage, same
    // spot the old "Queue is empty." message used to be set.
    string id;

    try
    {
        id = dequeueFlight();
    }
    catch (const runtime_error& e)
    {
        queueMessage = e.what();
        return;
    }
    // ===== EXCEPTION HANDLING END =====

    pushOperation("DEPART - " + id);

    queueMessage = id + " departed from queue.";

    if (currentNode != NULL &&
        id == string(currentNode->data.flightID))
    {
        waiting = false;
    }
}

void togglePause()
{
    paused = !paused;

    if (currentNode != NULL)
    {
        if (paused)
        {
            pushOperation(
                string("PAUSE - ") +
                currentNode->data.flightID
            );

            cout << "\nSimulation Paused.\n";
        }
        else
        {
            pushOperation(
                string("RESUME - ") +
                currentNode->data.flightID
            );

            cout << "\nSimulation Resumed.\n";
        }
    }
}


// ===== HASHING START =====
// =====================================================
// PASSENGER HASH TABLE (PL CO4 - HASHING)
// The student's hashing module, merged into AeroDesk:
//   hash function  = ID % 10
//   collision      = linear probing
//   delete         = tombstone (status 2, NOT back to Empty, so the
//                    probe chain of later passengers is not broken)
// Renamed so it cannot clash with the dashboard's Passenger struct:
//   Passenger -> HashEntry, hashTable -> hashSlots,
//   status[] -> hashStatus[], searchPassenger() -> hashSearchPassenger() ...
// Every operation also records its PROBE PATH (hashPath[]) so the
// dashboard can animate it: Hash Table page (sidebar / key 9),
// a mini slot strip on Passenger Search, a box on Passenger Details,
// and the original console menu on the main-window H key.
// =====================================================

const int HASH_TABLE_SIZE = 10;

struct HashEntry
{
    int passengerID;
    string passengerName;
};

// hashStatus: 0 = Empty, 1 = Occupied, 2 = Deleted
HashEntry hashSlots[HASH_TABLE_SIZE];
int hashStatus[HASH_TABLE_SIZE] = {0};

// ---- last-operation trace (drawn by the dashboard) ----
int hashPath[HASH_TABLE_SIZE];
int hashPathLen = 0;
int hashResultSlot = -1;     // slot where the operation ended, -1 = none
int hashOpProbes = 0;
int hashOpID = -1;
string hashOpName = "";
int hashAnimTick = 0;        // restarts at 0 for every new operation

string hashInput = "";       // Passenger ID typed on the Hash page
string hashMessage = "";
int hashMessageKind = 0;     // 0 = neutral, 1 = ok (green), 2 = error (red)

void updateHashAnimation()
{
    hashAnimTick++;
}

int hashFunction(int id)
{
    int index = id % HASH_TABLE_SIZE;
    return (index < 0) ? index + HASH_TABLE_SIZE : index;
}

// Linear-probing lookup. Returns the slot or -1 and counts the probes.
// With trace = true the visited slots are stored in hashPath[].
int hashLocate(int id, int& probes, bool trace)
{
    int start = hashFunction(id);
    probes = 0;

    if (trace)
        hashPathLen = 0;

    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        int index = (start + i) % HASH_TABLE_SIZE;

        probes++;

        if (trace)
            hashPath[hashPathLen++] = index;

        // An empty slot means the ID is not present
        if (hashStatus[index] == 0)
            return -1;

        if (hashStatus[index] == 1 &&
            hashSlots[index].passengerID == id)
            return index;
    }

    return -1;
}

int hashSearchPassenger(int id)
{
    int probes = 0;
    return hashLocate(id, probes, false);
}

// Search + record the trace for the animation.
int hashTraceSearch(int id)
{
    int probes = 0;
    int slot = hashLocate(id, probes, true);

    hashOpName = "SEARCH";
    hashOpID = id;
    hashOpProbes = probes;
    hashResultSlot = slot;
    hashAnimTick = 0;

    return slot;
}

// returns 0 = inserted, 1 = duplicate ID, 2 = table full
int hashInsertPassenger(int id, const string& name)
{
    hashOpName = "INSERT";
    hashOpID = id;
    hashAnimTick = 0;

    int probes = 0;
    int existing = hashLocate(id, probes, true);

    if (existing != -1)
    {
        hashResultSlot = existing;
        hashOpProbes = probes;
        return 1;
    }

    int start = hashFunction(id);
    int deletedIndex = -1;

    hashPathLen = 0;
    probes = 0;

    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        int index = (start + i) % HASH_TABLE_SIZE;

        hashPath[hashPathLen++] = index;
        probes++;

        if (hashStatus[index] == 2 && deletedIndex == -1)
            deletedIndex = index;

        if (hashStatus[index] == 0)
        {
            int target = (deletedIndex != -1) ? deletedIndex : index;

            hashSlots[target].passengerID = id;
            hashSlots[target].passengerName = name;
            hashStatus[target] = 1;

            hashResultSlot = target;
            hashOpProbes = probes;
            return 0;
        }
    }

    // Reuse a deleted slot if no empty slot remains
    if (deletedIndex != -1)
    {
        hashSlots[deletedIndex].passengerID = id;
        hashSlots[deletedIndex].passengerName = name;
        hashStatus[deletedIndex] = 1;

        hashResultSlot = deletedIndex;
        hashOpProbes = probes;
        return 0;
    }

    hashResultSlot = -1;
    hashOpProbes = probes;
    return 2;
}

// returns true if the passenger was found and marked Deleted
bool hashDeletePassenger(int id)
{
    int probes = 0;
    int index = hashLocate(id, probes, true);

    hashOpName = "DELETE";
    hashOpID = id;
    hashOpProbes = probes;
    hashResultSlot = index;
    hashAnimTick = 0;

    if (index == -1)
        return false;

    // Do not mark as Empty; preserve the probing chain
    hashStatus[index] = 2;
    return true;
}

int hashCountStatus(int s)
{
    int n = 0;

    for (int i = 0; i < HASH_TABLE_SIZE; i++)
        if (hashStatus[i] == s)
            n++;

    return n;
}

const char* hashStatusName(int s)
{
    if (s == 1) return "Occupied";
    if (s == 2) return "Deleted";
    return "Empty";
}

// Real AeroDesk name if the ID exists in the passenger database.
string hashNameForID(int id)
{
    int idx = findPassengerIndexByMap(id);

    if (idx >= 0)
        return passengerRecords[idx].name;

    return "Guest " + to_string(id);
}

// ---------------- console version (original menu) ----------------

void hashSearchAndDisplay(int id)
{
    int index = hashSearchPassenger(id);

    if (index == -1)
    {
        cout << "Passenger not found!\n";
    }
    else
    {
        cout << "\nPassenger Found!\n";
        cout << "Passenger ID   : "
             << hashSlots[index].passengerID << '\n';
        cout << "Passenger Name : "
             << hashSlots[index].passengerName << '\n';
        cout << "Hash Index     : " << index << '\n';
    }
}

void displayHashTable()
{
    cout << "\n====== AeroDesk Passenger Hash Table ======\n";

    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        cout << "Index " << i << " : ";

        if (hashStatus[i] == 1)
        {
            cout << "ID = " << hashSlots[i].passengerID
                 << ", Name = " << hashSlots[i].passengerName;
        }
        else if (hashStatus[i] == 2)
        {
            cout << "Deleted";
        }
        else
        {
            cout << "Empty";
        }

        cout << '\n';
    }
}

// Main window H key. Same menu as the standalone hashing file; the
// table it edits is the one drawn on the dashboard's Hash Table page.
void runHashingModule()
{
    int choice = 0;
    int id;
    string name;

    do
    {
        cout << "\n====================================\n";
        cout << "     AERODESK PL CO4 - HASHING\n";
        cout << "====================================\n";
        cout << "1. Insert Passenger\n";
        cout << "2. Search Passenger\n";
        cout << "3. Delete Passenger\n";
        cout << "4. Display Hash Table\n";
        cout << "5. Return to AeroDesk\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Returning to AeroDesk.\n";
            return;
        }

        switch (choice)
        {
            case 1:
            {
                cout << "Enter Passenger ID: ";

                if (!(cin >> id))
                {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid ID. Returning to AeroDesk.\n";
                    return;
                }

                cout << "Enter Passenger Name: ";
                cin >> ws;
                getline(cin, name);

                int r = hashInsertPassenger(id, name);

                if (r == 1)
                {
                    cout << "Passenger ID already exists!\n";
                }
                else if (r == 2)
                {
                    cout << "Hash table is full!\n";
                }
                else
                {
                    cout << "Passenger inserted successfully!\n";
                    cout << "Hash Table Index: " << hashResultSlot << '\n';
                }
                break;
            }

            case 2:
                cout << "Enter Passenger ID to search: ";

                if (!(cin >> id))
                {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid ID. Returning to AeroDesk.\n";
                    return;
                }

                hashTraceSearch(id);
                hashSearchAndDisplay(id);
                break;

            case 3:
                cout << "Enter Passenger ID to delete: ";

                if (!(cin >> id))
                {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid ID. Returning to AeroDesk.\n";
                    return;
                }

                if (hashDeletePassenger(id))
                    cout << "Passenger deleted successfully!\n";
                else
                    cout << "Passenger not found!\n";
                break;

            case 4:
                displayHashTable();
                break;

            case 5:
                cout << "Returning to AeroDesk...\n";
                break;

            default:
                cout << "Invalid choice! Please try again.\n";
        }

    } while (choice != 5);
}

// ---------------- dashboard operations ----------------

void hashSetMessage(const string& text, int kind)
{
    hashMessage = text;
    hashMessageKind = kind;
}

string hashInsertMessage(int id, int r)
{
    if (r == 1)
        return "ID " + to_string(id) + " already exists at slot " +
               to_string(hashResultSlot) + ".";

    if (r == 2)
        return "Hash table is full!";

    string text = "Inserted ID " + to_string(id) + " at slot " +
                  to_string(hashResultSlot) + " (" +
                  to_string(hashOpProbes) + " probe" +
                  (hashOpProbes == 1 ? "" : "s") + ").";

    if (hashResultSlot != hashFunction(id))
        text += " COLLISION - home slot " +
                to_string(hashFunction(id)) + " was taken.";

    return text;
}

bool hashReadInput(int& id)
{
    if (hashInput.empty())
    {
        hashSetMessage("Type a Passenger ID first (example: 105).", 2);
        return false;
    }

    id = atoi(hashInput.c_str());
    return true;
}

void hashDashInsert()
{
    int id;

    if (!hashReadInput(id))
        return;

    int r = hashInsertPassenger(id, hashNameForID(id));

    hashSetMessage(hashInsertMessage(id, r), r == 0 ? 1 : 2);
}

void hashDashSearch()
{
    int id;

    if (!hashReadInput(id))
        return;

    int slot = hashTraceSearch(id);

    if (slot >= 0)
        hashSetMessage("Found ID " + to_string(id) + " at slot " +
                       to_string(slot) + " after " +
                       to_string(hashOpProbes) + " probe(s).", 1);
    else
        hashSetMessage("Passenger " + to_string(id) +
                       " not found (" + to_string(hashOpProbes) +
                       " probe(s)).", 2);
}

void hashDashDelete()
{
    int id;

    if (!hashReadInput(id))
        return;

    if (hashDeletePassenger(id))
        hashSetMessage("Deleted ID " + to_string(id) + " - slot " +
                       to_string(hashResultSlot) +
                       " is now a tombstone (Deleted), not Empty.", 1);
    else
        hashSetMessage("Passenger " + to_string(id) + " not found.", 2);
}

void hashClearTable()
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
        hashStatus[i] = 0;

    hashPathLen = 0;
    hashResultSlot = -1;
    hashOpName = "";
    hashOpID = -1;
    hashOpProbes = 0;
    hashAnimTick = 0;
}

// Real AeroDesk passengers; 111 and 121 deliberately collide with 101
// (same home slot 1) so linear probing is visible straight away.
void hashLoadDemo()
{
    static const int demoIDs[6] = { 101, 102, 103, 111, 105, 121 };

    hashClearTable();

    for (int i = 0; i < 6; i++)
        hashInsertPassenger(demoIDs[i], hashNameForID(demoIDs[i]));

    hashPathLen = 0;
    hashResultSlot = -1;
    hashOpName = "";
    hashOpID = -1;
    hashAnimTick = 0;

    hashSetMessage("Demo loaded: 101,102,103,111,105,121 "
                   "(111 and 121 collide with 101 at slot 1).", 1);
}

// Insert a passenger straight from the passenger database.
int hashAddFromRecord(int recordIndex)
{
    int id = passengerRecords[recordIndex].passengerID;

    int r = hashInsertPassenger(id, passengerRecords[recordIndex].name);

    hashSetMessage(hashInsertMessage(id, r), r == 0 ? 1 : 2);

    return r;
}

// ---- buttons ----
Btn hashInsertBtn = { 320, 240,  480, 285, "INSERT (ENTER)", 0.10f, 0.55f, 0.25f };
Btn hashSearchBtn = { 490, 240,  630, 285, "SEARCH (S)",     0.08f, 0.42f, 0.78f };
Btn hashDeleteBtn = { 640, 240,  780, 285, "DELETE (X)",     0.75f, 0.18f, 0.20f };
Btn hashDemoBtn   = { 790, 240,  930, 285, "LOAD DEMO",      0.60f, 0.35f, 0.15f };
Btn hashClearBtn  = { 940, 240, 1060, 285, "CLEAR",          0.16f, 0.25f, 0.35f };

// Passenger Search page / Passenger Details page helper buttons
Btn hashAddSearchBtn  = { 985, 410, 1265, 455, "ADD TO HASH TABLE", 0.10f, 0.55f, 0.25f };
Btn hashAddDetailsBtn = { 985, 215, 1235, 262, "ADD TO HASH TABLE", 0.10f, 0.55f, 0.25f };

// ---- small drawing helpers shared by the three pages ----
void hashOutline(float x1, float y1, float x2, float y2, float t)
{
    drawRect(x1, y1, x2, y1 + t);
    drawRect(x1, y2 - t, x2, y2);
    drawRect(x1, y1, x1 + t, y2);
    drawRect(x2 - t, y1, x2, y2);
}

void hashSlotColor(int s)
{
    if (s == 1)
        glColor3f(0.07f, 0.42f, 0.24f);
    else if (s == 2)
        glColor3f(0.50f, 0.14f, 0.16f);
    else
        glColor3f(0.16f, 0.18f, 0.22f);
}

float hashPulse()
{
    return 0.55f + 0.45f * sinf(hashAnimTick * 0.30f);
}

// Ten small cells. glowSlot pulses; homeSlot gets a blue marker.
void drawHashMiniStrip(float x, float y, int glowSlot, int homeSlot)
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        float cx = x + i * 28.0f;

        hashSlotColor(hashStatus[i]);
        drawRect(cx, y, cx + 25, y + 25);

        if (i == glowSlot)
        {
            float p = hashPulse();
            glColor3f(0.2f * p, 0.9f * p, 1.0f * p);
            hashOutline(cx - 2, y - 2, cx + 27, y + 27, 3);
        }

        if (i == homeSlot)
        {
            glColor3f(0.35f, 0.65f, 1.0f);
            drawRect(cx, y - 7, cx + 25, y - 3);
        }

        setGray();
        drawText(cx + 8, y + 32, to_string(i), GLUT_BITMAP_HELVETICA_12);
    }
}
// ===== HASHING END =====


// =====================================================
// PASSENGER SEARCH (Passenger ID)
// =====================================================

// ===== EXCEPTION HANDLING START =====
// Previously this just set searchMessage directly for every case
// (empty input, invalid characters silently became 0 via atoi(),
// not-found). Now a bad Passenger ID actually THROWS, and
// searchMessage is only ever set from inside the catch blocks -
// exactly what was asked for.
// ===== EXCEPTION HANDLING END =====
void doPassengerSearch()
{
    searchResultIndex = -1;

    try
    {
        // AeroException.h: empty / non-digit input throws
        // InvalidPassengerIDException (an invalid_argument).
        validatePassengerIDText(searchInput);

        int id = atoi(searchInput.c_str());

        // STL: O(log n) lookup through map<int, Passenger*> instead
        // of the linear findPassengerIndexByID() loop.
        int index = findPassengerIndexByMap(id);

        if (index < 0)
            throw PassengerNotFoundException(id);   // AeroException.h

        searchResultIndex = index;
        selectedPassengerIndex = index;

        searchMessage = "Passenger found.";

        // ===== HASHING START =====
        // Same ID goes through the hash table too, so the Hash Table
        // page replays its probe path and the slot glows.
        hashTraceSearch(id);
        // ===== HASHING END =====

        cout << "\n[SEARCH] Passenger found: "
             << passengerRecords[index].passengerID
             << " - "
             << passengerRecords[index].name
             << " - Flight: "
             << passengerRecords[index].flightID
             << endl;
    }
    catch (const invalid_argument& e)
    {
        searchMessage = e.what();
        cout << "\n[EXCEPTION] invalid_argument: " << e.what() << endl;
    }
    catch (const out_of_range& e)
    {
        searchMessage = e.what();
        cout << "\n[EXCEPTION] out_of_range: " << e.what() << endl;
    }
}


// =====================================================
// SECONDARY DASHBOARD - PAGE CHANGE
// =====================================================

void goToPage(int page)
{
    dashboardPage = (DashboardPage)page;

    if (dashboardPage == DASH_SEARCH)
    {
        searchInput = "";
        searchMessage = "";
        searchResultIndex = -1;
    }

    if (dashboardPage == DASH_QUEUE)
        queueMessage = "";

    // ===== OOP DASHBOARD START =====
    if (dashboardPage == DASH_OOP)
        oopView = 0;
    // ===== OOP DASHBOARD END =====

    // ===== SEARCH SORT START =====
    if (dashboardPage == DASH_SORT)
        sortPageEnter();
    // ===== SEARCH SORT END =====

    // ===== CLIPPING START =====
    if (dashboardPage == DASH_CLIP)
        clipPageEnter();
    // ===== CLIPPING END =====

    // ===== HASHING START =====
    if (dashboardPage == DASH_HASH)
    {
        hashInput = "";
        hashAnimTick = 0;
    }
    // ===== HASHING END =====

    redrawAll();
}


// =====================================================
// SECONDARY DASHBOARD - DRAWING
// =====================================================

void drawField(
    float labelX,
    float valueX,
    float y,
    const string& label,
    const string& value)
{
    setGray();
    drawText(labelX, y, label);

    setWhite();
    drawText(valueX, y, value);
}

void drawDashboardHeader()
{
    setDark();
    drawRect(0, 730, DASH_W, DASH_H);

    setWhite();
    drawText(30, 765, "AERODESK MANAGEMENT DASHBOARD",
             GLUT_BITMAP_TIMES_ROMAN_24);

    setGray();
    drawText(30, 740, "Flight Operations  |  Passenger Management");

    if (currentNode != NULL)
    {
        setWhite();
        drawText(1020, 765,
                 string("Current Flight: ") +
                 currentNode->data.flightID);
    }
}

void drawSidebar()
{
    setPanel();
    drawRect(0, 0, 300, 730);

    setGray();
    drawText(25, 705, "MENU");

    for (int i = 0; i < SIDEBAR_BTN_COUNT; i++)
    {
        bool active = (i < 12 && i == (int)dashboardPage);
        drawButton(dashBtns[i], active);
    }
}

// "top" is the card's upper edge; cards are 85 tall so two rows fit
// above the SYSTEM MODULES panel (second row added for the STL cards).
void drawCard(
    float x,
    float top,
    const string& label,
    const string& value)
{
    setPanel();
    drawRect(x, top - 85, x + 225, top);

    setGray();
    drawText(x + 15, top - 28, label);

    setBlue();
    drawText(x + 15, top - 68, value, GLUT_BITMAP_TIMES_ROMAN_24);
}

void drawHomePage()
{
    setWhite();
    drawText(320, 685, "HOME", GLUT_BITMAP_TIMES_ROMAN_24);

    setGray();
    drawText(320, 655, "Overview of the AeroDesk flight and passenger system.");

    string selectedName = "None";

    if (selectedPassengerIndex >= 0)
        selectedName = passengerRecords[selectedPassengerIndex].name;

    drawCard(320,  630, "Total Flights", to_string(MAX_FLIGHTS));
    drawCard(561,  630, "Total Passengers", to_string(passengerRecordCount));
    drawCard(802,  630, "Current Flight",
             currentNode != NULL ? string(currentNode->data.flightID) : string("None"));
    drawCard(1043, 630, "Selected Passenger", selectedName);

    // ===== STL START =====
    // Second card row, computed with STL algorithms:
    //   count_if over vector<Passenger>  -> Business class passengers
    //   count_if over flights[]          -> flights with fuel < 80%
    //   find_if over flights[]           -> first such flight's ID
    drawCard(320, 530, "Business Class Passengers",
             to_string(countBusinessPassengers()));
    drawCard(561, 530, "Flights With Fuel < 80%",
             to_string(countLowFuelFlights()) + " of " + to_string(MAX_FLIGHTS));
    drawCard(802, 530, "First Low-Fuel Flight",
             firstLowFuelFlightID());
    drawCard(1043, 530, "STL Containers",
             "vec " + to_string(passengerVector.size()) +
             " / map " + to_string(passengerMap.size()));
    // ===== STL END =====

    setPanel();
    drawRect(320, 150, 1270, 425);

    setWhite();
    drawText(345, 392, "SYSTEM MODULES");

    setGray();
    drawText(345, 355, "1  HOME - system overview");
    drawText(345, 320, "2  FLIGHT LIST - view, sort and select flights");
    drawText(345, 285, "3  PASSENGER LIST - passengers of the selected flight");
    drawText(345, 250, "4  PASSENGER SEARCH - find a passenger by Passenger ID");
    drawText(345, 215, "5 FLIGHT DETAILS  6 PASSENGER DETAILS  7 WAITING QUEUE  8 CO4 PIPELINE");

    setWhite();
    drawText(345, 175,
             "Latest Operation: " + getLatestOperation() +
             "     Queue: " + to_string(queueCount) + "/" + to_string(QUEUE_SIZE));
}

void drawFlightListPage()
{
    setWhite();
    drawText(320, 685, "FLIGHT LIST", GLUT_BITMAP_TIMES_ROMAN_24);

    setGray();
    drawText(320, 655, "Click a flight row to select it, then open Flight Details.");

    setBlue();
    drawRect(320, 595, 1270, 635);

    // Header (TYPE column added for inheritance)
    setWhite();
    drawText(335, 608, "FLIGHT");
    drawText(440, 608, "SPEED");
    drawText(570, 608, "ALTITUDE");
    drawText(700, 608, "FUEL");
    drawText(780, 608, "PASSENGERS");
    drawText(910, 608, "DESTINATION");
    drawText(1070, 608, "TYPE");

    int selected = currentFlightIndex();

    // ===== STL START =====
    // Rows are drawn in flightSortOrder (a vector<int> sorted with
    // std::sort + lambda); "row" is the screen position, "i" is the
    // real flight index into flights[]/destinationNames[]/flightObjects[].
    if (flightSortOrder.empty())
        applyFlightSort();

    for (int row = 0; row < MAX_FLIGHTS; row++)
    {
        int i = flightSortOrder[row];
        float top = 590.0f - row * 62.0f;
    // ===== STL END =====

        if (i == selected)
            glColor3f(0.10f, 0.30f, 0.55f);
        else
            setPanel();

        drawRect(320, top - 54, 1270, top);

        setWhite();

        drawText(335, top - 34, flights[i].flightID);
        drawText(440, top - 34, to_string(flights[i].speed) + " km/h");
        drawText(570, top - 34, to_string(flights[i].altitude) + " m");
        drawText(700, top - 34, to_string(flights[i].fuel) + "%");
        drawText(780, top - 34, to_string(flights[i].passengers));
        drawText(910, top - 34, destinationNames[i]);
        drawText(1070, top - 34, flightObjects[i]->getType());   // polymorphic call
    }

    drawButton(flightsOpenBtn, false);

    // ===== STL START =====
    drawButton(flightsSortBtn, false);

    setGray();
    drawText(960, 238, "Sorted by: " + getFlightSortLabel());
    // ===== STL END =====
}

void drawPassengerListPage()
{
    int fi = currentFlightIndex();
    int start = getFlightStartIndex(fi);
    int count = flights[fi].passengers;
    int pages = (count + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;

    if (passengerPage >= pages)
        passengerPage = pages - 1;

    if (passengerPage < 0)
        passengerPage = 0;

    setWhite();
    drawText(320, 685, "PASSENGER LIST", GLUT_BITMAP_TIMES_ROMAN_24);

    setGray();
    drawText(320, 655,
             string("Flight ") + flights[fi].flightID +
             " - " + destinationNames[fi] +
             " - " + to_string(count) +
             " passengers. Click a row to select, then open Passenger Details.");

    setBlue();
    drawRect(320, 595, 1270, 635);

    setWhite();
    drawText(335, 608, "ID");
    drawText(415, 608, "NAME");
    drawText(640, 608, "FLIGHT");
    drawText(740, 608, "SEAT");
    drawText(825, 608, "CLASS");
    drawText(950, 608, "DESTINATION");
    drawText(1100, 608, "TICKET");

    for (int r = 0; r < ROWS_PER_PAGE; r++)
    {
        int k = passengerPage * ROWS_PER_PAGE + r;

        if (k >= count)
            break;

        int idx = start + k;
        Passenger& p = passengerRecords[idx];

        float top = 590.0f - r * 46.0f;

        if (idx == selectedPassengerIndex)
            glColor3f(0.10f, 0.30f, 0.55f);
        else
            setPanel();

        drawRect(320, top - 42, 1270, top);

        setWhite();

        drawText(335, top - 28, to_string(p.passengerID));
        drawText(415, top - 28, p.name);
        drawText(640, top - 28, p.flightID);
        drawText(740, top - 28, p.seat);
        drawText(825, top - 28, p.travelClass);
        drawText(950, top - 28, p.destination);
        drawText(1100, top - 28, p.ticketStatus);
    }

    drawButton(passPrevBtn, false);
    drawButton(passNextBtn, false);
    drawButton(passOpenBtn, false);

    setGray();
    drawText(660, 66,
             "Page " + to_string(passengerPage + 1) +
             " / " + to_string(pages));
}

void drawPassengerSearchPage()
{
    setWhite();
    drawText(320, 685, "PASSENGER SEARCH", GLUT_BITMAP_TIMES_ROMAN_24);

    setGray();
    drawText(320, 655, "Type a Passenger ID (example: 105) and press ENTER or click SEARCH.");

    setPanel();
    drawRect(320, 470, 960, 625);

    setWhite();
    drawText(345, 590, "PASSENGER ID");

    setDark();
    drawRect(345, 500, 730, 550);

    setWhite();
    drawText(360, 518, searchInput + "_");

    drawButton(searchBtn, false);

    if (!searchMessage.empty())
    {
        if (searchResultIndex >= 0)
            setGreen();
        else
            setRed();

        drawText(320, 435, searchMessage);
    }

    if (searchResultIndex >= 0)
    {
        Passenger& p = passengerRecords[searchResultIndex];

        setPanel();
        drawRect(320, 180, 800, 410);

        setWhite();
        drawText(345, 375, "SEARCH RESULT");

        drawField(345, 545, 335, "Passenger ID", to_string(p.passengerID));
        drawField(345, 545, 300, "Name", p.name);
        drawField(345, 545, 265, "Flight", p.flightID);
        drawField(345, 545, 230, "Seat", p.seat);
        drawField(345, 545, 195, "Destination", p.destination);

        drawButton(searchViewBtn, false);
    }

    // ===== HASHING START =====
    // Right of the search box: the 10 hash slots. The slot that holds
    // the found passenger glows, the blue bar marks the home slot.
    {
        setWhite();
        drawText(985, 610, "HASH TABLE (CO4)");

        int glow = -1;
        int home = -1;

        if (searchResultIndex >= 0)
        {
            int id = passengerRecords[searchResultIndex].passengerID;
            int probes = 0;

            home = hashFunction(id);
            glow = hashLocate(id, probes, false);

            drawHashMiniStrip(985, 545, glow, home);

            setGray();
            drawText(985, 510,
                     "ID % 10 = " + to_string(home) + "  (home slot)");

            if (glow >= 0)
            {
                setGreen();
                drawText(985, 485,
                         "Hash index: " + to_string(glow) + ", probes: " +
                         to_string(probes));
            }
            else
            {
                setRed();
                drawText(985, 485, "Not stored in hash table");
                drawButton(hashAddSearchBtn, false);
            }
        }
        else
        {
            drawHashMiniStrip(985, 545, -1, -1);

            setGray();
            drawText(985, 510, "Search an ID to see its slot.");
        }
    }
    // ===== HASHING END =====
}

void drawFlightDetailsPage()
{
    setWhite();
    drawText(320, 685, "FLIGHT DETAILS", GLUT_BITMAP_TIMES_ROMAN_24);

    setGray();
    drawText(320, 655, "Complete information for the selected flight.");

    if (currentNode == NULL)
    {
        setGray();
        drawText(320, 590, "No flight selected.");
        return;
    }

    int fi = currentFlightIndex();

    // Panel extended downward (was 170, then 130) to additionally
    // fit the new polymorphic "Fuel Required" row below.
    setPanel();
    drawRect(320, 70, 900, 630);

    setWhite();
    drawText(345, 590, "FLIGHT INFORMATION");

    drawField(345, 640, 545, "Flight ID", currentNode->data.flightID);
    drawField(345, 640, 500, "Destination", destinationNames[fi]);
    drawField(345, 640, 455, "Speed", to_string(currentNode->data.speed) + " km/h");
    drawField(345, 640, 410, "Altitude", to_string(currentNode->data.altitude) + " m");
    drawField(345, 640, 365, "Fuel", to_string(currentNode->data.fuel) + "%");
    drawField(345, 640, 320, "Passenger Count", to_string(currentNode->data.passengers));
    drawField(345, 640, 275, "Current Status", getStatusString());
    drawField(345, 640, 230, "List Position",
              to_string(fi + 1) + " of " + to_string(MAX_FLIGHTS));

    // Inheritance: type and the derived-class-specific field
    FlightBase* obj = flightObjects[fi];

    drawField(345, 640, 185, "Flight Type", obj->getType());
    drawField(345, 640, 140, obj->getExtraLabel(), obj->getExtraValue());

    // ===== POLYMORPHISM START =====
    // obj is declared as FlightBase* (the base class), but this ONE
    // call - obj->calculateFuel() - runs PassengerFlight's,
    // CargoFlight's or PrivateJet's own formula depending on which
    // derived object this flight actually is. Nothing here branches
    // on the type to decide which formula to use; the vtable does
    // that automatically at runtime - that is the polymorphism demo.
    drawField(345, 640, 95, "Fuel Required",
              to_string((int)(obj->calculateFuel() + 0.5f)) + " units (calculated)");
    // ===== POLYMORPHISM END =====

    drawButton(fdPassBtn, false);
    drawButton(fdPrevBtn, false);
    drawButton(fdNextBtn, false);
}

void drawPassengerDetailsPage()
{
    setWhite();
    drawText(320, 685, "PASSENGER DETAILS", GLUT_BITMAP_TIMES_ROMAN_24);

    setGray();
    drawText(320, 655, "Complete profile of the selected passenger.");

    setPanel();
    drawRect(320, 180, 1270, 630);

    if (selectedPassengerIndex < 0)
    {
        setGray();
        drawText(345, 570,
                 "No passenger selected. Please select a passenger");
        drawText(345, 540,
                 "from Passenger List or use Passenger Search.");
        return;
    }

    Passenger& p = passengerRecords[selectedPassengerIndex];

    setWhite();
    drawText(345, 590, "PASSENGER PROFILE");

    drawField(350, 560, 540, "Passenger ID", to_string(p.passengerID));
    drawField(350, 560, 495, "Name", p.name);
    drawField(350, 560, 450, "Age", to_string(p.age));
    drawField(350, 560, 405, "Gender", p.gender);
    drawField(350, 560, 360, "Seat", p.seat);

    drawField(800, 960, 540, "Flight ID", p.flightID);
    drawField(800, 960, 495, "Travel Class", p.travelClass);
    drawField(800, 960, 450, "Destination", p.destination);
    drawField(800, 960, 405, "Ticket Status", p.ticketStatus);

    // ===== HASHING START =====
    // CO4 hashing box: where this passenger lives in the hash table.
    {
        int probes = 0;
        int home = hashFunction(p.passengerID);
        int slot = hashLocate(p.passengerID, probes, false);

        setBlue();
        drawRect(345, 195, 1245, 335);

        setWhite();
        drawText(360, 310, "HASH TABLE (CO4 - HASHING)");

        drawText(360, 280,
                 "ID % 10 = " + to_string(p.passengerID) + " % 10 = " +
                 to_string(home));

        drawText(360, 252,
                 "Slot " + to_string(home) + " status = " +
                 hashStatusName(hashStatus[home]));

        if (slot >= 0)
            drawText(360, 224,
                     "This passenger: stored at slot " + to_string(slot) +
                     " (" + to_string(probes) + " probe" +
                     (probes == 1 ? ")" : "s)"));
        else
        {
            drawText(360, 224, "This passenger is NOT in the hash table.");
            drawButton(hashAddDetailsBtn, false);
        }
    }
    // ===== HASHING END =====
}

// ===== CONTROL WORD START =====
// =====================================================
// FLIGHT OPERATION CONTROL WORDS (COA CO4 - control unit)
// Every operation AeroDesk already pushes on its operation stack
// (TAKEOFF, LAND, WAIT, PAUSE, RESET, DEPART ...) is treated as an
// "instruction". Its OPCODE is cut out of the stack string (for
// example "TAKEOFF - AI101" -> TAKEOFF) and looked up in a small
// control store that holds one 8-bit CONTROL WORD per opcode - a
// 1 means the control unit asserts that signal for the operation.
// Shown in the main window with the C key (like the O OOP panel)
// and as a strip on the dashboard's Waiting Queue page.
// =====================================================

const int CW_BITS = 8;

// bit 7 (leftmost) ... bit 0 (rightmost)
const char* cwNames[CW_BITS] =
{
    "Runway Clear", "Engines Full", "Gear Retract", "Gear Extend",
    "Brake / Flaps", "Hold Position", "Queue Op", "Clear State"
};

const char* cwShort[CW_BITS] =
{
    "RWY", "ENG", "GUP", "GDN", "BRK", "HLD", "QUE", "CLR"
};

struct ControlWordEntry
{
    const char* opcode;
    unsigned char word;
};

const int CW_OPCODES = 8;

ControlWordEntry controlStore[CW_OPCODES] =
{
    { "TAKEOFF", 0xE0 },   // 1110 0000  runway, engines, gear up
    { "CRUISE",  0x40 },   // 0100 0000  engines
    { "LAND",    0x98 },   // 1001 1000  runway, gear down, brake
    { "WAIT",    0x06 },   // 0000 0110  hold, queue
    { "PAUSE",   0x04 },   // 0000 0100  hold
    { "RESUME",  0x40 },   // 0100 0000  engines
    { "RESET",   0x0B },   // 0000 1011  brake, queue, clear
    { "DEPART",  0xC2 }    // 1100 0010  runway, engines, queue
};

bool showControlPanel = false;

// "TAKEOFF - AI101" -> "TAKEOFF"; LANDING / LANDED -> "LAND".
string getOperationOpcode(const string& op)
{
    if (op == "No Operation")
        return "NONE";

    size_t pos = op.find(" - ");
    string name = (pos == string::npos) ? op : op.substr(0, pos);

    if (name == "LANDING" || name == "LANDED")
        name = "LAND";

    return name;
}

// Index into controlStore[] for an opcode, or -1 if it has none.
int findControlStoreIndex(const string& opcode)
{
    for (int i = 0; i < CW_OPCODES; i++)
    {
        if (opcode == controlStore[i].opcode)
            return i;
    }

    return -1;
}

// 8-bit word as text, e.g. "1110 0000".
string controlWordText(unsigned char word)
{
    string t;

    for (int b = 7; b >= 0; b--)
    {
        t += ((word >> b) & 1) ? '1' : '0';

        if (b == 4)
            t += ' ';
    }

    return t;
}

// Main-window panel (toggle with C): the whole control store, with
// the row of the latest operation highlighted.
void drawControlWordPanel()
{
    string op = getLatestOperation();
    string opcode = getOperationOpcode(op);
    int active = findControlStoreIndex(opcode);

    glColor3f(0.03f, 0.05f, 0.10f);
    drawRect(880, 230, 1265, 565);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(895, 540, "CONTROL WORD  (COA CO4)");

    drawText(895, 512, "IR: " + op, GLUT_BITMAP_HELVETICA_12);
    drawText(895, 494, "Opcode: " + opcode, GLUT_BITMAP_HELVETICA_12);

    if (active >= 0)
    {
        glColor3f(0.95f, 0.80f, 0.15f);
        drawText(895, 476, "Word: " + controlWordText(controlStore[active].word),
                 GLUT_BITMAP_HELVETICA_12);
    }
    else
    {
        glColor3f(0.65f, 0.68f, 0.74f);
        drawText(895, 476, "Word: (no control word for this op)",
                 GLUT_BITMAP_HELVETICA_12);
    }

    // column headers
    glColor3f(0.65f, 0.68f, 0.74f);

    for (int b = 0; b < CW_BITS; b++)
        drawCenteredText(1000.0f + b * 31.0f + 12.0f, 452,
                         cwShort[b], GLUT_BITMAP_HELVETICA_12);

    for (int r = 0; r < CW_OPCODES; r++)
    {
        float y = 424.0f - r * 22.0f;

        if (r == active)
            glColor3f(1.0f, 1.0f, 1.0f);
        else
            glColor3f(0.65f, 0.68f, 0.74f);

        drawText(895, y + 4, controlStore[r].opcode, GLUT_BITMAP_HELVETICA_12);

        for (int b = 0; b < CW_BITS; b++)
        {
            bool set = (controlStore[r].word >> (7 - b)) & 1;
            float x = 1000.0f + b * 31.0f;

            if (set && r == active)  glColor3f(0.95f, 0.80f, 0.15f);
            else if (set)            glColor3f(0.55f, 0.45f, 0.10f);
            else                     glColor3f(0.12f, 0.16f, 0.22f);

            drawRect(x, y, x + 24, y + 17);
        }
    }

    glColor3f(0.65f, 0.68f, 0.74f);
    drawText(895, 246, "RWY runway  ENG engines  GUP/GDN gear up/down",
             GLUT_BITMAP_HELVETICA_12);
    drawText(895, 232, "BRK brake  HLD hold  QUE queue  CLR clear state",
             GLUT_BITMAP_HELVETICA_12);
}

// Dashboard strip: the control word of the latest operation, one
// labelled lamp per signal.
void drawControlWordStrip(float x1, float y1, float x2, float y2)
{
    string op = getLatestOperation();
    string opcode = getOperationOpcode(op);
    int active = findControlStoreIndex(opcode);

    setPanel();
    drawRect(x1, y1, x2, y2);

    setBlue();
    drawText(x1 + 15, y2 - 28, "CONTROL WORD OF THE LATEST OPERATION");

    setWhite();
    drawText(x1 + 15, y2 - 52,
             "IR: " + op + "     Opcode: " + opcode +
             (active >= 0 ? "     Word: " + controlWordText(controlStore[active].word)
                          : string("     Word: none")),
             GLUT_BITMAP_HELVETICA_12);

    float cell = (x2 - x1 - 30.0f) / (float)CW_BITS;

    for (int b = 0; b < CW_BITS; b++)
    {
        float x = x1 + 15.0f + b * cell;
        bool set = (active >= 0) && (((controlStore[active].word >> (7 - b)) & 1) != 0);

        if (set) glColor3f(0.95f, 0.80f, 0.15f);
        else     glColor3f(0.12f, 0.16f, 0.22f);

        drawRect(x, y1 + 12, x + cell - 8.0f, y1 + 40);

        if (set) glColor3f(0.0f, 0.0f, 0.0f);
        else     setGray();

        drawCenteredText(x + (cell - 8.0f) / 2.0f, y1 + 21,
                         cwNames[b], GLUT_BITMAP_HELVETICA_12);
    }
}
// ===== CONTROL WORD END =====


void drawQueuePage()
{
    setWhite();
    drawText(320, 685, "WAITING QUEUE", GLUT_BITMAP_TIMES_ROMAN_24);

    setGray();
    drawText(320, 655, "FIFO: the first flight added is the first to depart.");

    for (int i = 0; i < QUEUE_SIZE; i++)
    {
        float x = 320.0f + i * 190.0f;

        if (i < queueCount)
            glColor3f(0.10f, 0.30f, 0.55f);
        else
            setPanel();

        drawRect(x, 480, x + 170, 600);

        setGray();
        drawText(x + 10, 575, "SLOT " + to_string(i + 1));

        setWhite();

        if (i < queueCount)
        {
            int index = (queueFront + i) % QUEUE_SIZE;

            drawText(x + 10, 525, flightQueue[index]);

            if (i == 0)
                drawText(x + 10, 495, "FRONT");

            if (i == queueCount - 1)
                drawText(x + 100, 495, "REAR");
        }
        else
        {
            drawText(x + 10, 525, "EMPTY");
        }
    }

    setWhite();
    drawText(320, 430, "Queue Size: " + to_string(queueCount) +
                       " / " + to_string(QUEUE_SIZE));
    drawText(320, 395, "Next Flight: " + getNextQueuedFlight());

    if (currentNode != NULL)
        drawText(320, 360, string("Selected Flight: ") +
                           currentNode->data.flightID);

    setBlue();
    drawText(320, 320, queueMessage);

    drawButton(queueAddBtn, false);
    drawButton(queueDepartBtn, false);

    // ===== CONTROL WORD START =====
    drawControlWordStrip(320, 70, 1270, 205);
    // ===== CONTROL WORD END =====
}

// ===== COA CO4 PAGE START =====
// =====================================================
// DASH_COA - CO4: CONTROL UNIT + PIPELINING, DRIVEN BY AERODESK
// A sidebar page (button 8 / key '8') that explains the four CO4
// topics USING the AeroDesk project itself, drawn with plain
// OpenGL shapes (rectangles, triangles, text):
//   1. Control signals    - the latest flight command (top of the
//                           operation stack) is the "instruction"
//                           and the control unit decodes it into
//                           runway/engine/gear signals
//   2. Hardwired vs Microprogrammed - AeroDesk's own two input
//                           paths: keyboard() switch (hardwired)
//                           and the mainBtns[] table (table-driven)
//   3. Organization       - 80386's 6 units, each mapped to the
//                           AeroDesk part that does the same job
//   4. Pipelining         - the real 5 flights flow through
//                           QUEUE > TAKEOFF > CRUISE > LANDING like
//                           instructions through pipeline stages
// Animated from ONE counter, coaTick, advanced by the project's
// existing update() timer (no new timer).
// =====================================================

int coaTick = 0;   // +1 every 40 ms tick

void updateCOAAnimation()
{
    coaTick++;
}

// ---- small drawing helpers (local to this page) ----
void coaArrowRight(float x1, float x2, float y)
{
    drawRect(x1, y - 1, x2 - 6, y + 1);

    glBegin(GL_TRIANGLES);
    glVertex2f(x2, y);
    glVertex2f(x2 - 8, y + 4);
    glVertex2f(x2 - 8, y - 4);
    glEnd();
}

void coaArrowDown(float x, float y1, float y2)
{
    drawRect(x - 1, y2 + 6, x + 1, y1);

    glBegin(GL_TRIANGLES);
    glVertex2f(x, y2);
    glVertex2f(x - 4, y2 + 8);
    glVertex2f(x + 4, y2 + 8);
    glEnd();
}

void coaBox(float x1, float y1, float x2, float y2,
            const string& label, bool lit)
{
    if (lit)
        glColor3f(0.10f, 0.50f, 0.85f);
    else
        glColor3f(0.14f, 0.20f, 0.30f);

    drawRect(x1, y1, x2, y2);

    setGray();
    drawRect(x1, y1, x2, y1 + 1);
    drawRect(x1, y2 - 1, x2, y2);
    drawRect(x1, y1, x1 + 1, y2);
    drawRect(x2 - 1, y1, x2, y2);

    setWhite();
    drawCenteredText((x1 + x2) / 2.0f, (y1 + y2) / 2.0f - 4.0f,
                     label, GLUT_BITMAP_HELVETICA_12);
}

// Which "opcode" column the latest AeroDesk operation decodes to:
// 0 TAKEOFF, 1 LAND, 2 WAIT, 3 PAUSE, -1 = no signal change.
int coaDecodeOperation(const string& op)
{
    if (op.compare(0, 7, "TAKEOFF") == 0) return 0;
    if (op.compare(0, 4, "LAND") == 0)    return 1;   // LANDING / LANDED
    if (op.compare(0, 4, "WAIT") == 0)    return 2;
    if (op.compare(0, 5, "PAUSE") == 0)   return 3;

    return -1;
}

// Which pipeline stage the CURRENT flight is really in
// (0 QUEUE, 1 TAKEOFF, 2 CRUISE, 3 LANDING) or -1 if idle.
int coaLiveStage()
{
    if (waiting)               return 0;
    if (phase == TAKING_OFF)   return 1;
    if (phase == CRUISING)     return 2;
    if (phase == LANDING)      return 3;

    return -1;
}

// ---- 1. CONTROL SIGNALS ----
void drawCOAControlSignals()
{
    setPanel();
    drawRect(320, 430, 700, 645);

    setBlue();
    drawText(335, 620, "1. CONTROL SIGNALS");

    string op = getLatestOperation();
    int active = coaDecodeOperation(op);

    setWhite();
    drawText(335, 600, "IR (latest flight command): " + op,
             GLUT_BITMAP_HELVETICA_12);

    const char* names[6] =
    {
        "Runway Clear", "Engines Full", "Gear Retract",
        "Gear Extend", "Brake / Flaps", "Hold Position"
    };

    const char* ops[4] = { "TAKEOFF", "LAND", "WAIT", "PAUSE" };

    // signal x opcode: 1 = control unit asserts this signal
    bool on[6][4] =
    {
        { 1, 1, 0, 0 },   // Runway Clear
        { 1, 0, 0, 0 },   // Engines Full
        { 1, 0, 0, 0 },   // Gear Retract
        { 0, 1, 0, 0 },   // Gear Extend
        { 0, 1, 0, 0 },   // Brake / Flaps
        { 0, 0, 1, 1 }    // Hold Position
    };

    for (int c = 0; c < 4; c++)
    {
        float x = 460.0f + c * 60.0f;

        if (c == active) glColor3f(0.10f, 0.50f, 0.85f);
        else             glColor3f(0.14f, 0.20f, 0.30f);

        drawRect(x, 572, x + 56, 592);

        setWhite();
        drawCenteredText(x + 28, 577, ops[c], GLUT_BITMAP_HELVETICA_12);
    }

    for (int r = 0; r < 6; r++)
    {
        float y = 548.0f - r * 20.0f;

        setGray();
        drawText(335, y + 4, names[r], GLUT_BITMAP_HELVETICA_12);

        for (int c = 0; c < 4; c++)
        {
            float x = 460.0f + c * 60.0f;

            if (on[r][c] && c == active)    glColor3f(0.95f, 0.80f, 0.15f);
            else if (on[r][c])              glColor3f(0.55f, 0.45f, 0.10f);
            else                            glColor3f(0.10f, 0.14f, 0.20f);

            drawRect(x, y, x + 56, y + 17);
        }
    }

    setGray();
    drawText(335, 438, "Yellow = signals the control unit asserts for that command.",
             GLUT_BITMAP_HELVETICA_12);
}

// ---- 2. HARDWIRED vs MICROPROGRAMMED (AeroDesk's own two paths) ----
void drawCOAControlUnits(int step)
{
    setPanel();
    drawRect(710, 430, 1270, 645);

    setBlue();
    drawText(725, 620, "2. HARDWIRED vs MICROPROGRAMMED");

    setWhite();
    drawText(725, 596, "Hardwired = keyboard() switch(key)  (fixed logic):",
             GLUT_BITMAP_HELVETICA_12);

    const char* hw[4] = { "Key Press", "switch(key)", "Action fn", "State" };

    for (int i = 0; i < 4; i++)
    {
        float x = 725.0f + i * 130.0f;
        coaBox(x, 555, x + 100, 583, hw[i], (step % 4) == i);

        if (i < 3)
        {
            setGray();
            coaArrowRight(x + 100, x + 130, 569);
        }
    }

    setWhite();
    drawText(725, 522, "Table-driven = mainBtns[] + inBtn()  (changeable table):",
             GLUT_BITMAP_HELVETICA_12);

    const char* mp[5] = { "Click", "mainBtns[]", "inBtn()", "Action fn", "State" };

    for (int i = 0; i < 5; i++)
    {
        float x = 725.0f + i * 104.0f;
        coaBox(x, 475, x + 82, 503, mp[i], (step % 5) == i);

        if (i < 4)
        {
            setGray();
            coaArrowRight(x + 82, x + 104, 489);
        }
    }

    setGray();
    drawText(725, 440, "Both end in the same flight actions (takeoff, land, wait...).",
             GLUT_BITMAP_HELVETICA_12);
}

// ---- 3. ORGANIZATION OF THE 80386 -> AeroDesk equivalents ----
void drawCOAOrganization(int unit)
{
    setPanel();
    drawRect(320, 70, 700, 420);

    setBlue();
    drawText(335, 395, "3. 80386 UNITS = AERODESK PARTS");

    const char* names[6] =
    {
        "Bus Interface", "Code Prefetch", "Instr Decode",
        "Execution", "Segmentation", "Paging"
    };

    const char* role[6] =
    {
        "Bus Interface: moves data between CPU, memory and I/O.",
        "Code Prefetch: reads ahead into a 16-byte queue.",
        "Instr Decode: turns prefetched bytes into operations.",
        "Execution: ALU + registers carry out the operation.",
        "Segmentation: logical -> linear address translation.",
        "Paging: linear -> physical address, 4 KB pages."
    };

    string mine[6] =
    {
        "AeroDesk: GLUT loop - keyboard/mouse in, screen out.",
        "AeroDesk: waiting queue - " + to_string(queueCount) + " flight(s) lined up.",
        "AeroDesk: keyboard()/mouse() decode input into actions.",
        "AeroDesk: update() timer runs the flight simulation.",
        "AeroDesk: Passenger ID -> record (map lookup).",
        "AeroDesk: passenger list pages, " + to_string(ROWS_PER_PAGE) +
            " rows per page (page " + to_string(passengerPage + 1) + ")."
    };

    // 2 columns x 3 rows, flowing left->right then down
    for (int i = 0; i < 6; i++)
    {
        int col = i % 2;
        int row = i / 2;

        float x = 345.0f + col * 180.0f;
        float y = 330.0f - row * 85.0f;

        coaBox(x, y, x + 140, y + 45, names[i], i == unit);

        if (col == 0)
        {
            setGray();
            coaArrowRight(x + 140, x + 180, y + 22);
        }
        else if (row < 2)
        {
            setGray();
            coaArrowDown(x + 70, y, y - 40);
        }
    }

    setWhite();
    drawText(335, 118, role[unit], GLUT_BITMAP_HELVETICA_12);

    glColor3f(0.95f, 0.80f, 0.15f);
    drawText(335, 96, mine[unit], GLUT_BITMAP_HELVETICA_12);
}

// ---- 4. PIPELINING: the real flights flow through the stages ----
void drawCOAPipeline(int clockNow)
{
    setPanel();
    drawRect(710, 70, 1270, 420);

    setBlue();
    drawText(725, 395, "4. PIPELINING: FLIGHTS AS INSTRUCTIONS");

    const char* stage[4] = { "QUEUE", "TAKEOFF", "CRUISE", "LANDING" };

    float sc[4][3] =
    {
        { 0.20f, 0.45f, 0.80f }, { 0.20f, 0.65f, 0.45f },
        { 0.85f, 0.55f, 0.15f }, { 0.75f, 0.25f, 0.30f }
    };

    // clock header (current clock in yellow)
    setGray();
    drawText(725, 362, "Clock", GLUT_BITMAP_HELVETICA_12);

    for (int c = 1; c <= 8; c++)
    {
        if (c == clockNow) glColor3f(0.95f, 0.80f, 0.15f);
        else               setGray();

        drawCenteredText(790.0f + (c - 1) * 55.0f + 25.0f, 362,
                         to_string(c), GLUT_BITMAP_HELVETICA_12);
    }

    int selected = currentFlightIndex();

    for (int i = 0; i < MAX_FLIGHTS; i++)
    {
        float y = 318.0f - i * 44.0f;

        if (i == selected) glColor3f(0.95f, 0.80f, 0.15f);
        else               setWhite();

        drawText(730, y + 12, flights[i].flightID, GLUT_BITMAP_HELVETICA_12);

        for (int c = 1; c <= 8; c++)
        {
            int st = c - 1 - i;           // stage this flight is in at this clock
            float x = 790.0f + (c - 1) * 55.0f;

            glColor3f(0.10f, 0.14f, 0.20f);
            drawRect(x, y, x + 50, y + 34);

            if (st >= 0 && st < 4 && c <= clockNow)
            {
                glColor3f(sc[st][0], sc[st][1], sc[st][2]);
                drawRect(x, y, x + 50, y + 34);

                setWhite();
                drawCenteredText(x + 25, y + 12, stage[st],
                                 GLUT_BITMAP_HELVETICA_12);
            }
        }
    }

    // legend chips - the one matching the CURRENT flight's real
    // phase is lit, so the live simulation is visible here too
    int live = coaLiveStage();

    for (int s = 0; s < 4; s++)
    {
        float x = 725.0f + s * 132.0f;

        if (s == live) glColor3f(0.95f, 0.80f, 0.15f);
        else           glColor3f(sc[s][0] * 0.6f, sc[s][1] * 0.6f, sc[s][2] * 0.6f);

        drawRect(x, 118, x + 124, 136);

        if (s == live) glColor3f(0.0f, 0.0f, 0.0f);
        else           setWhite();

        drawCenteredText(x + 62, 123, stage[s], GLUT_BITMAP_HELVETICA_12);
    }

    setWhite();
    string curID = (currentNode != NULL) ? string(currentNode->data.flightID) : string("-");
    drawText(725, 98,
             "Live: " + curID + " is " + getStatusString() +
             "  (lit chip = its real stage)",
             GLUT_BITMAP_HELVETICA_12);

    drawText(725, 78,
             "5 flights x 4 stages: 20 cycles one-by-one, 8 cycles pipelined = 2.5x faster.",
             GLUT_BITMAP_HELVETICA_12);
}

void drawCOAPage()
{
    setWhite();
    drawText(320, 685, "CO4 - CONTROL UNIT & PIPELINING",
             GLUT_BITMAP_TIMES_ROMAN_24);

    setGray();
    drawText(320, 655, "Explained with AeroDesk's own flights, commands, queue and code paths.");

    int ctrl  = coaTick / 14;                  // control-unit path animation
    int unit  = (coaTick / 22) % 6;            // active 80386 unit
    int clk   = (coaTick / 14) % 11;           // pipeline clock (0..10, then restart)

    drawCOAControlSignals();
    drawCOAControlUnits(ctrl);
    drawCOAOrganization(unit);
    drawCOAPipeline(clk);
}
// ===== COA CO4 PAGE END =====


// ===== HASHING START =====
// =====================================================
// DASH_HASH - PASSENGER HASH TABLE PAGE (PL CO4 - Hashing)
// 10 slots, colour = status, probe animation for every operation.
// =====================================================

// Position of "slot" inside the already-revealed part of the probe
// path (-1 = not visited yet).
int hashPathPos(int slot, int shown)
{
    for (int k = 0; k < shown && k < hashPathLen; k++)
        if (hashPath[k] == slot)
            return k;

    return -1;
}

void drawHashSlotCell(int i, float x, float top, int shown, bool running)
{
    const float W = 170.0f;
    const float H = 105.0f;
    float bottom = top - H;

    int s = hashStatus[i];
    int k = hashPathPos(i, shown);

    // base colour, brighter while this is the probe being tested now
    hashSlotColor(s);

    if (running && k >= 0 && k == shown - 1)
        glColor3f(0.55f, 0.42f, 0.08f);

    drawRect(x, bottom, x + W, top);

    // index + status
    setWhite();
    drawText(x + 10, top - 22, "[" + to_string(i) + "]");

    setGray();
    drawText(x + 50, top - 22, hashStatusName(s), GLUT_BITMAP_HELVETICA_12);

    if (s == 1 || s == 2)
    {
        string name = hashSlots[i].passengerName;

        if (name.length() > 20)
            name = name.substr(0, 20);

        if (s == 1)
            setWhite();
        else
            setGray();

        drawText(x + 10, top - 52,
                 "ID " + to_string(hashSlots[i].passengerID));
        drawText(x + 10, top - 72, name, GLUT_BITMAP_HELVETICA_12);

        int home = hashFunction(hashSlots[i].passengerID);

        if (home == i)
        {
            setGray();
            drawText(x + 10, top - 92,
                     "hash = " + to_string(home), GLUT_BITMAP_HELVETICA_12);
        }
        else
        {
            glColor3f(1.0f, 0.75f, 0.10f);
            drawText(x + 10, top - 92,
                     "hash = " + to_string(home) + "  (collision)",
                     GLUT_BITMAP_HELVETICA_12);
        }
    }

    // probe animation overlay
    if (k >= 0)
    {
        glColor3f(1.0f, 0.75f, 0.10f);
        hashOutline(x, bottom, x + W, top, 3);

        drawText(x + 128, top - 22, "#" + to_string(k + 1));
    }

    // final result slot pulses once the animation has finished
    if (!running && i == hashResultSlot && hashOpName != "")
    {
        float p = hashPulse();
        glColor3f(0.2f * p, 0.9f * p, 1.0f * p);
        hashOutline(x - 3, bottom - 3, x + W + 3, top + 3, 4);
    }
}

void drawHashPage()
{
    setWhite();
    drawText(320, 685, "PASSENGER HASH TABLE  (PL CO4 - HASHING)",
             GLUT_BITMAP_TIMES_ROMAN_24);

    setGray();
    drawText(320, 655,
             "hash(ID) = ID % 10  |  Linear Probing  |  Delete = tombstone "
             "(Deleted, not Empty)");

    // how many probes of the current operation are visible yet
    int shown = hashAnimTick / 7 + 1;

    if (shown > hashPathLen)
        shown = hashPathLen;

    bool running = (hashPathLen > 0 && shown < hashPathLen);

    // ---- 10 slots, two rows of five ----
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        float x = 320.0f + (i % 5) * 190.0f;
        float top = (i < 5) ? 630.0f : 510.0f;

        drawHashSlotCell(i, x, top, shown, running);
    }

    // ---- input ----
    setWhite();
    drawText(320, 372, "PASSENGER ID");

    setPanel();
    drawRect(320, 312, 560, 358);

    setWhite();
    drawText(335, 328, hashInput + "_");

    // ---- buttons ----
    drawButton(hashInsertBtn, false);
    drawButton(hashSearchBtn, false);
    drawButton(hashDeleteBtn, false);
    drawButton(hashDemoBtn, false);
    drawButton(hashClearBtn, false);

    // ---- last operation panel ----
    setPanel();
    drawRect(700, 300, 1270, 395);

    if (hashOpName.empty())
    {
        setGray();
        drawText(715, 360, "No operation yet.");
        drawText(715, 330, "Try INSERT, SEARCH or DELETE.");
    }
    else
    {
        setWhite();
        drawText(715, 372,
                 "LAST OPERATION: " + hashOpName + "  ID " +
                 to_string(hashOpID));

        setGray();
        drawText(715, 347,
                 "hash: " + to_string(hashOpID) + " % 10 = " +
                 to_string(hashFunction(hashOpID)));

        string path = "probe path: ";

        for (int k = 0; k < shown; k++)
        {
            if (k > 0)
                path += " -> ";

            path += to_string(hashPath[k]);
        }

        if (!running)
            path += "   (" + to_string(hashOpProbes) + " probe" +
                    (hashOpProbes == 1 ? ")" : "s)");

        glColor3f(1.0f, 0.75f, 0.10f);
        drawText(715, 322, path);
    }

    // ---- message ----
    if (!hashMessage.empty())
    {
        if (hashMessageKind == 1)
            setGreen();
        else if (hashMessageKind == 2)
            setRed();
        else
            setGray();

        drawText(320, 207, hashMessage);
    }

    // ---- legend + load factor ----
    setPanel();
    drawRect(320, 70, 1270, 185);

    hashSlotColor(0);
    drawRect(340, 148, 358, 166);
    setWhite();
    drawText(366, 150, "Empty (0)", GLUT_BITMAP_HELVETICA_12);

    hashSlotColor(1);
    drawRect(500, 148, 518, 166);
    setWhite();
    drawText(526, 150, "Occupied (1)", GLUT_BITMAP_HELVETICA_12);

    hashSlotColor(2);
    drawRect(690, 148, 708, 166);
    setWhite();
    drawText(716, 150, "Deleted (2) - tombstone", GLUT_BITMAP_HELVETICA_12);

    glColor3f(1.0f, 0.75f, 0.10f);
    drawText(930, 150, "#n = probe order (amber)", GLUT_BITMAP_HELVETICA_12);

    int occ = hashCountStatus(1);
    int del = hashCountStatus(2);
    char lf[64];

    snprintf(lf, sizeof(lf), "%.2f", occ / (float)HASH_TABLE_SIZE);

    setGray();
    drawText(340, 120,
             "Load factor = " + to_string(occ) + " / " +
             to_string(HASH_TABLE_SIZE) + " occupied = " + string(lf) +
             "   |   Deleted slots: " + to_string(del));

    drawText(340, 92,
             "Main window: press H for the original console hashing menu. "
             "Passenger Search / Details can ADD to this table.");
}
// ===== HASHING END =====


// ===== STL EXTRAS START =====
// =====================================================
// STL EXTRAS (shown on the OOP CONCEPTS page -> "STL CONTAINERS")
//   priority_queue : emergency departures - LOWEST FUEL leaves first
//                    (mirror of the Waiting Queue, which stays FIFO)
//   set<string>    : unique destinations, also blocks duplicate
//                    flights inside the priority queue
//   unordered_map  : same Passenger ID lookup as map<int,Passenger*>,
//                    timed side by side
// The hand-written queue/stack stay the source of truth.
// =====================================================

unordered_map<int, Passenger*> passengerHashMap;
set<string> destinationSet;

struct EmergencyEntry
{
    string flightID;
    int fuel;
    int seq;          // arrival number, breaks ties (earlier first)
    bool emergency;   // true = pushed with the forced 10% fuel
};

// min-heap on fuel: the entry with the LEAST fuel is top()
struct EmergencyCompare
{
    bool operator()(const EmergencyEntry& a, const EmergencyEntry& b) const
    {
        if (a.fuel != b.fuel)
            return a.fuel > b.fuel;

        return a.seq > b.seq;
    }
};

priority_queue<EmergencyEntry, vector<EmergencyEntry>, EmergencyCompare> emergencyPQ;
vector<EmergencyEntry> emergencyFIFO;     // same entries in arrival order
set<string> emergencyIn;                  // flights currently queued
vector<string> emergencyDeparted;         // departure order
int emergencySeq = 0;

string stlMessage = "";
int stlMessageKind = 0;                   // 0 neutral, 1 ok, 2 error

string stlInput = "";
bool stlCompared = false;
int stlCompareID = 0;
int stlMapIdx = -1, stlHashIdx = -1;
double stlMapNs = 0.0, stlHashNs = 0.0;
long stlSink = 0;

// built once after buildPassengerSTL()
void buildSTLExtras()
{
    passengerHashMap.clear();
    passengerHashMap.reserve(passengerRecordCount * 2);
    destinationSet.clear();

    for (int i = 0; i < passengerRecordCount; i++)
    {
        passengerHashMap[passengerRecords[i].passengerID] = &passengerRecords[i];
        destinationSet.insert(passengerRecords[i].destination);
    }

    cout << "[STL] unordered_map size = " << passengerHashMap.size()
         << " | set<string> destinations = " << destinationSet.size()
         << endl;
}

void stlSetMessage(const string& text, int kind)
{
    stlMessage = text;
    stlMessageKind = kind;
}

bool stlPQAdd(const string& id, int fuel, bool emergency)
{
    if (emergencyIn.count(id) > 0)
        return false;                       // set blocks the duplicate

    EmergencyEntry e;
    e.flightID = id;
    e.fuel = fuel;
    e.seq = ++emergencySeq;
    e.emergency = emergency;

    emergencyPQ.push(e);
    emergencyFIFO.push_back(e);
    emergencyIn.insert(id);

    return true;
}

void stlPQAddAll()
{
    int added = 0;

    for (int i = 0; i < MAX_FLIGHTS; i++)
    {
        if (flightObjects[i] == NULL)
            continue;

        if (stlPQAdd(flightObjects[i]->getFlightID(),
                     flightObjects[i]->getFuel(), false))
            added++;
    }

    stlSetMessage("push() x" + to_string(added) +
                  " - set<string> skipped flights already queued.", 1);
}

void stlPQAddCurrent(bool emergency)
{
    FlightBase* cur = getCurrentFlightObject();

    if (cur == NULL)
        return;

    int fuel = emergency ? 10 : cur->getFuel();

    if (stlPQAdd(cur->getFlightID(), fuel, emergency))
        stlSetMessage("push(" + cur->getFlightID() + ", fuel " +
                      to_string(fuel) + "%)" +
                      (emergency ? " as EMERGENCY - it jumps to top()." : "."), 1);
    else
        stlSetMessage(cur->getFlightID() +
                      " is already in the queue (set<string> rejected it).", 2);
}

void stlPQDepart()
{
    try
    {
        if (emergencyPQ.empty())
            throw EmergencyQueueEmptyException();   // AeroException.h

        EmergencyEntry top = emergencyPQ.top();
        emergencyPQ.pop();

        emergencyIn.erase(top.flightID);

        for (size_t i = 0; i < emergencyFIFO.size(); i++)
        {
            if (emergencyFIFO[i].flightID == top.flightID)
            {
                emergencyFIFO.erase(emergencyFIFO.begin() + i);
                break;
            }
        }

        emergencyDeparted.push_back(top.flightID);

        stlSetMessage("pop(): " + top.flightID + " departs first - lowest fuel (" +
                      to_string(top.fuel) + "%).", 1);

        cout << "\n[STL] priority_queue departure: " << top.flightID
             << " (fuel " << top.fuel << "%)\n";
    }
    catch (const runtime_error& e)
    {
        stlSetMessage(e.what(), 2);
    }
}

void stlPQClear()
{
    while (!emergencyPQ.empty())
        emergencyPQ.pop();

    emergencyFIFO.clear();
    emergencyIn.clear();
    emergencyDeparted.clear();
    emergencySeq = 0;

    stlSetMessage("Emergency queue cleared.", 0);
}

// Same ID through map<int,...> (red-black tree) and unordered_map
// (hash table), each repeated many times and timed.
void stlCompareSearch()
{
    if (stlInput.empty())
    {
        stlSetMessage("Type a Passenger ID first (example: 105).", 2);
        return;
    }

    int id = atoi(stlInput.c_str());

    stlCompared = true;
    stlCompareID = id;

    stlMapIdx = findPassengerIndexByMap(id);

    unordered_map<int, Passenger*>::iterator hit = passengerHashMap.find(id);
    stlHashIdx = (hit == passengerHashMap.end())
                     ? -1 : (int)(hit->second - passengerRecords);

    const int LOOPS = 300000;
    long found = 0;

    chrono::steady_clock::time_point t0 = chrono::steady_clock::now();

    for (int i = 0; i < LOOPS; i++)
    {
        map<int, Passenger*>::iterator m = passengerMap.find(id + (i & 7));
        found += (m != passengerMap.end()) ? 1 : 0;
    }

    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();

    for (int i = 0; i < LOOPS; i++)
    {
        unordered_map<int, Passenger*>::iterator h =
            passengerHashMap.find(id + (i & 7));
        found += (h != passengerHashMap.end()) ? 1 : 0;
    }

    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();

    stlSink = found;      // keeps the loops from being optimised away

    stlMapNs  = chrono::duration<double, nano>(t1 - t0).count() / LOOPS;
    stlHashNs = chrono::duration<double, nano>(t2 - t1).count() / LOOPS;

    stlSetMessage("Compared " + to_string(LOOPS) + " lookups per container.", 1);
}

// ---- buttons ----
Btn stlAddAllBtn = { 320, 275, 475, 320, "ADD ALL FLIGHTS",         0.08f, 0.42f, 0.78f };
Btn stlAddCurBtn = { 485, 275, 640, 320, "ADD CURRENT",             0.08f, 0.42f, 0.78f };
Btn stlClearBtn  = { 650, 275, 790, 320, "CLEAR",                   0.16f, 0.25f, 0.35f };
Btn stlEmergBtn  = { 320, 225, 590, 270, "CURRENT = EMERGENCY (10%)", 0.75f, 0.18f, 0.20f };
Btn stlDepartBtn = { 600, 225, 790, 270, "DEPART NEXT",             0.10f, 0.55f, 0.25f };
Btn stlCompareBtn = { 1025, 400, 1255, 442, "COMPARE",              0.60f, 0.35f, 0.15f };
Btn stlOpenBtn   = { 1060, 655, 1270, 705, "STL CONTAINERS >",      0.60f, 0.35f, 0.15f };
// ===== STL EXTRAS END =====

// ===== OOP DASHBOARD START =====
// =====================================================
// DASH_OOP - OOP CONCEPTS PAGE (dashboard version of the O-key panel)
// Overview = 4 clickable cards. Each card opens its own detail view
// (oopView): 1 Inheritance, 2 Polymorphism, 3 Abstraction,
// 4 Encapsulation. Every view has working buttons that call the REAL
// FlightBase / PassengerFlight / CargoFlight / PrivateJet code.
// =====================================================

const int OOP_OVERVIEW = 0;
const int OOP_INHERIT  = 1;
const int OOP_POLY     = 2;
const int OOP_ABSTRACT = 3;
const int OOP_ENCAP    = 4;
const int OOP_STL      = 5;   // STL containers view

// oopView itself is declared next to dashboardPage (goToPage resets it)

int oopClassSel = 1;                       // 0 base, 1 Passenger, 2 Cargo, 3 Jet
int oopFieldSel = -1;                      // clicked field box in the Inheritance view (-1 = none)
vector<string> oopDemoLines;               // result box of the demo buttons
int oopDemoKind = 0;                       // 0 neutral, 1 ok, 2 error
PassengerFlight* oopDemoObj = NULL;        // object used by the encapsulation demo

const char* oopClassNames[4] =
{
    "FlightBase", "PassengerFlight", "CargoFlight", "PrivateJet"
};

Btn oopPrevBtn = { 820,  80, 1030, 122, "PREV FLIGHT", 0.16f, 0.25f, 0.35f };
Btn oopNextBtn = { 1045, 80, 1255, 122, "NEXT FLIGHT", 0.16f, 0.25f, 0.35f };
Btn oopBackBtn = { 1060, 655, 1270, 705, "< BACK TO OOP", 0.16f, 0.25f, 0.35f };

Btn oopClsBtn[4] =
{
    { 320, 565, 560, 615, "FlightBase (abstract)", 0.45f, 0.20f, 0.55f },
    { 320, 500, 560, 550, "PassengerFlight",       0.08f, 0.30f, 0.60f },
    { 320, 435, 560, 485, "CargoFlight",           0.60f, 0.35f, 0.15f },
    { 320, 370, 560, 420, "PrivateJet",            0.10f, 0.50f, 0.50f }
};

Btn oopPrintOneBtn = { 815,  85, 1030, 125, "PRINT display(): THIS", 0.08f, 0.42f, 0.78f };
Btn oopPrintAllBtn = { 1040, 85, 1255, 125, "PRINT display(): ALL",  0.60f, 0.35f, 0.15f };
Btn oopPrintHideBtn = { 815, 130, 1255, 158, "HIDE OUTPUT - SHOW HOW THE CALL IS RESOLVED", 0.16f, 0.25f, 0.35f };

Btn oopTryAbsBtn = { 335, 245, 625, 290, "TRY new FlightBase()",        0.75f, 0.18f, 0.20f };
Btn oopTryConBtn = { 640, 245, 960, 290, "CargoFlight via FlightBase*", 0.10f, 0.55f, 0.25f };

Btn oopEncBtn[8] =
{
    { 815, 500, 1035, 540, "setFuel(150)",        0.08f, 0.42f, 0.78f },
    { 1050, 500, 1260, 540, "setFuel(-20)",       0.08f, 0.42f, 0.78f },
    { 815, 450, 1035, 490, "setFuel(40)",         0.08f, 0.42f, 0.78f },
    { 1050, 450, 1260, 490, "setSpeed(-50)",      0.08f, 0.42f, 0.78f },
    { 815, 400, 1035, 440, "setSpeed(700)",       0.08f, 0.42f, 0.78f },
    { 1050, 400, 1260, 440, "NEW: speed -10",     0.75f, 0.18f, 0.20f },
    { 815, 350, 1035, 390, "NEW: pass>capacity",  0.75f, 0.18f, 0.20f },
    { 1050, 350, 1260, 390, "RESET DEMO",         0.16f, 0.25f, 0.35f }
};

bool oopIn(float x1, float y1, float x2, float y2, float wx, float wy)
{
    return wx >= x1 && wx <= x2 && wy >= y1 && wy <= y2;
}

void oopClassRGB(const string& c, float& r, float& g, float& b)
{
    if (c == "PassengerFlight")      { r = 0.08f; g = 0.30f; b = 0.60f; }
    else if (c == "CargoFlight")     { r = 0.60f; g = 0.35f; b = 0.15f; }
    else if (c == "PrivateJet")      { r = 0.10f; g = 0.50f; b = 0.50f; }
    else                             { r = 0.45f; g = 0.20f; b = 0.55f; }
}

// flight indices belonging to a class selector (0 = every flight)
int oopFlightsOfClass(int sel, int out[])
{
    int n = 0;

    for (int i = 0; i < MAX_FLIGHTS; i++)
    {
        if (flightObjects[i] == NULL)
            continue;

        if (sel == 0 || flightObjects[i]->getClassName() == oopClassNames[sel])
            out[n++] = i;
    }

    return n;
}

string oopFuelText(FlightBase* obj)
{
    return to_string((int)(obj->calculateFuel() + 0.5f)) + " units";
}

// the actual formula, with this flight's numbers filled in
string oopFormulaWithNumbers(FlightBase* obj)
{
    string cls = obj->getClassName();
    string f = to_string(obj->getFuel());

    if (cls == "PassengerFlight")
    {
        PassengerFlight* pf = dynamic_cast<PassengerFlight*>(obj);
        int cap = (pf != NULL) ? pf->getCapacity() : 0;

        return f + " * (0.7 + 0.3 * " + to_string(obj->getPassengers()) +
               "/" + to_string(cap) + ") = " + oopFuelText(obj);
    }

    if (cls == "CargoFlight")
    {
        CargoFlight* cf = dynamic_cast<CargoFlight*>(obj);
        int w = (cf != NULL) ? cf->getCargoWeight() : 0;

        return f + " + " + to_string(w) + " * 0.002 = " + oopFuelText(obj);
    }

    if (cls == "PrivateJet")
        return f + " * 0.6 = " + oopFuelText(obj);

    return f + " = " + oopFuelText(obj);
}

// ---------------- button actions ----------------

void oopEnsureDemo()
{
    if (oopDemoObj == NULL)
        oopDemoObj = new PassengerFlight("DEMO01", 600, 5000, 50, 100, 150);
}

void oopEncAction(int k)
{
    oopEnsureDemo();

    oopDemoLines.clear();
    oopDemoKind = 1;

    switch (k)
    {
        case 0:
            oopDemoObj->setFuel(150);
            oopDemoLines.push_back("demo->setFuel(150);");
            oopDemoLines.push_back("Setter clamps the value: fuel is now " +
                                   to_string(oopDemoObj->getFuel()) + "% (never 150).");
            break;

        case 1:
            oopDemoObj->setFuel(-20);
            oopDemoLines.push_back("demo->setFuel(-20);");
            oopDemoLines.push_back("Setter clamps the value: fuel is now " +
                                   to_string(oopDemoObj->getFuel()) + "% (never negative).");
            break;

        case 2:
            oopDemoObj->setFuel(40);
            oopDemoLines.push_back("demo->setFuel(40);");
            oopDemoLines.push_back("Valid value accepted: fuel = " +
                                   to_string(oopDemoObj->getFuel()) + "%.");
            break;

        case 3:
        {
            int before = oopDemoObj->getSpeed();
            oopDemoObj->setSpeed(-50);
            oopDemoLines.push_back("demo->setSpeed(-50);");
            oopDemoLines.push_back("REJECTED: speed stays " +
                                   to_string(oopDemoObj->getSpeed()) +
                                   " km/h (was " + to_string(before) + ").");
            oopDemoKind = 2;
            break;
        }

        case 4:
            oopDemoObj->setSpeed(700);
            oopDemoLines.push_back("demo->setSpeed(700);");
            oopDemoLines.push_back("Valid value accepted: speed = " +
                                   to_string(oopDemoObj->getSpeed()) + " km/h.");
            break;

        case 5:
            oopDemoLines.push_back("new PassengerFlight(\"BAD1\", speed -10, ...);");

            try
            {
                PassengerFlight bad("BAD1", -10, 0, 50, 10, 100);
                oopDemoLines.push_back("(created - should not happen)");
            }
            catch (const invalid_argument& e)
            {
                oopDemoLines.push_back("Constructor THROWS invalid_argument:");
                oopDemoLines.push_back(e.what());
                oopDemoKind = 2;
            }
            break;

        case 6:
            oopDemoLines.push_back("new PassengerFlight(\"BAD2\", ..., passengers 200, capacity 100);");

            try
            {
                PassengerFlight bad("BAD2", 600, 5000, 50, 200, 100);
                oopDemoLines.push_back("(created - should not happen)");
            }
            catch (const invalid_argument& e)
            {
                oopDemoLines.push_back("Derived constructor THROWS invalid_argument:");
                oopDemoLines.push_back(e.what());
                oopDemoKind = 2;
            }
            break;

        case 7:
            delete oopDemoObj;
            oopDemoObj = NULL;
            oopEnsureDemo();
            oopDemoLines.push_back("Demo object re-created: speed 600, fuel 50%.");
            break;
    }
}

void oopTryAbstract()
{
    oopDemoLines.clear();
    oopDemoKind = 2;

    oopDemoLines.push_back("FlightBase f(\"X\", 0, 0, 0, 0);      // what the compiler says:");
    oopDemoLines.push_back("error: cannot declare variable 'f' to be of abstract type 'FlightBase'");
    oopDemoLines.push_back("note:  because the following virtual functions are pure within 'FlightBase':");
    oopDemoLines.push_back("note:    'virtual string FlightBase::getType() const'  (= 0)");
    oopDemoLines.push_back("note:    getClassName(), getExtraLabel(), getExtraValue()  (= 0)");
}

void oopTryConcrete()
{
    oopDemoLines.clear();
    oopDemoKind = 1;

    FlightBase* p = new CargoFlight("DEMO02", 500, 3000, 50, 0, 1000);

    oopDemoLines.push_back("FlightBase* p = new CargoFlight(\"DEMO02\", ...);   // OK");
    oopDemoLines.push_back("p->getType()      = " + p->getType());
    oopDemoLines.push_back("p->getClassName() = " + p->getClassName());
    oopDemoLines.push_back("p->calculateFuel() = " + oopFuelText(p) +
                           "   (CargoFlight formula)");

    delete p;     // virtual destructor: safe through the base pointer
}

// ===== OOP PRINT DISPLAY START =====
// The two PRINT display() buttons call the REAL virtual display().
// cout is captured for a moment, so the same text goes to the console
// AND into oopPrintLines, which the dashboard draws in a box.
vector<string> oopPrintLines;
string oopPrintTitle = "";
bool oopPrintShown = false;

void oopPrintFlight(int i)
{
    if (i < 0 || i >= MAX_FLIGHTS || flightObjects[i] == NULL)
        return;

    stringstream ss;
    streambuf* old = cout.rdbuf(ss.rdbuf());

    flightObjects[i]->display();          // virtual: derived version runs

    cout.rdbuf(old);

    string text = ss.str();
    cout << text;                         // console copy

    // split into lines, then wrap long ones at the double-space gaps
    size_t pos = 0;

    while (pos < text.size())
    {
        size_t nl = text.find('\n', pos);

        if (nl == string::npos)
            nl = text.size();

        string line = text.substr(pos, nl - pos);
        pos = nl + 1;

        string cur = "";
        size_t p = 0;

        while (p < line.size())
        {
            size_t g = line.find("  ", p + 2);

            if (g == string::npos)
                g = line.size();

            string tok = line.substr(p, g - p);
            p = g;

            if (!cur.empty() && cur.size() + tok.size() > 64)
            {
                oopPrintLines.push_back(cur);
                cur = "   " + tok.substr(tok.find_first_not_of(' ') == string::npos ? 0 : tok.find_first_not_of(' '));
            }
            else
                cur += tok;
        }

        if (!cur.empty())
            oopPrintLines.push_back(cur);
    }
}

void oopPrintBegin(const string& title)
{
    oopPrintLines.clear();
    oopPrintTitle = title;
    oopPrintShown = true;
}
// ===== OOP PRINT DISPLAY END =====

void oopHandleClick(float wx, float wy)
{
    if (oopView == OOP_OVERVIEW)
    {
        if (inBtn(stlOpenBtn, wx, wy))
            oopView = OOP_STL;
        else if (inBtn(oopPrevBtn, wx, wy))
            previousFlight();
        else if (inBtn(oopNextBtn, wx, wy))
            nextFlight();
        else if (oopIn(320, 370, 780, 630, wx, wy))
            oopView = OOP_INHERIT;
        else if (oopIn(800, 370, 1270, 630, wx, wy))
            oopView = OOP_POLY;
        else if (oopIn(320, 240, 780, 350, wx, wy))
        {
            oopView = OOP_ABSTRACT;
            oopDemoLines.clear();
        }
        else if (oopIn(320, 70, 780, 240, wx, wy))
        {
            oopView = OOP_ENCAP;
            oopDemoLines.clear();
            oopEnsureDemo();
        }

        return;
    }

    if (inBtn(oopBackBtn, wx, wy))
    {
        oopView = OOP_OVERVIEW;
        return;
    }

    if (oopView == OOP_INHERIT)
    {
        for (int c = 0; c < 4; c++)
        {
            if (inBtn(oopClsBtn[c], wx, wy))
            {
                oopClassSel = c;
                oopFieldSel = -1;
            }
        }

        // object-layout boxes: click a field to see its details
        {
            int fields = 5 + ((oopClassSel == 0) ? 0 : (oopClassSel == 3) ? 2 : 1);

            for (int f = 0; f < fields; f++)
            {
                float fx = 595.0f + f * 92.0f;

                if (oopIn(fx, 160, fx + 88, 205, wx, wy))
                    oopFieldSel = (oopFieldSel == f) ? -1 : f;
            }
        }

        int list[MAX_FLIGHTS];
        int n = oopFlightsOfClass(oopClassSel, list);

        for (int r = 0; r < n; r++)
        {
            float y = 290.0f - r * 38.0f;

            if (oopIn(325, y - 12, 555, y + 22, wx, wy))
                selectFlightByIndex(list[r]);
        }
    }
    else if (oopView == OOP_POLY)
    {
        for (int i = 0; i < MAX_FLIGHTS; i++)
        {
            float y = 565.0f - i * 40.0f;

            if (oopIn(320, y - 12, 1270, y + 24, wx, wy))
                selectFlightByIndex(i);
        }

        if (inBtn(oopPrintOneBtn, wx, wy))
        {
            cout << "\n[OOP] display() through FlightBase* for the selected flight:\n";
            oopPrintBegin("display() output - selected flight");
            oopPrintFlight(currentFlightIndex());
        }
        else if (oopPrintShown && inBtn(oopPrintHideBtn, wx, wy))
            oopPrintShown = false;
        else if (inBtn(oopPrintAllBtn, wx, wy))
        {
            cout << "\n[OOP] display() through FlightBase* for ALL flights:\n";
            oopPrintBegin("display() output - ALL flights");
            for (int i = 0; i < MAX_FLIGHTS; i++)
                oopPrintFlight(i);
        }
    }
    else if (oopView == OOP_ABSTRACT)
    {
        if (inBtn(oopTryAbsBtn, wx, wy))
            oopTryAbstract();
        else if (inBtn(oopTryConBtn, wx, wy))
            oopTryConcrete();
    }
    else if (oopView == OOP_ENCAP)
    {
        for (int k = 0; k < 8; k++)
            if (inBtn(oopEncBtn[k], wx, wy))
                oopEncAction(k);
    }
    else if (oopView == OOP_STL)
    {
        if (inBtn(stlAddAllBtn, wx, wy))
            stlPQAddAll();
        else if (inBtn(stlAddCurBtn, wx, wy))
            stlPQAddCurrent(false);
        else if (inBtn(stlEmergBtn, wx, wy))
            stlPQAddCurrent(true);
        else if (inBtn(stlDepartBtn, wx, wy))
            stlPQDepart();
        else if (inBtn(stlClearBtn, wx, wy))
            stlPQClear();
        else if (inBtn(stlCompareBtn, wx, wy))
            stlCompareSearch();
    }
}

// ---------------- drawing helpers ----------------

void drawOOPClassBox(float x1, float y1, float x2, float y2,
                     const string& name, bool active, float r, float g, float b)
{
    glColor3f(r, g, b);
    drawRect(x1, y1, x2, y2);

    if (active)
    {
        float p = hashPulse();
        glColor3f(1.0f * p, 0.85f * p, 0.15f * p);
        hashOutline(x1 - 3, y1 - 3, x2 + 3, y2 + 3, 3);
    }

    setWhite();
    drawCenteredText((x1 + x2) / 2.0f, (y1 + y2) / 2.0f - 5.0f,
                     name, GLUT_BITMAP_HELVETICA_12);
}

void drawOOPLine(float x1, float y1, float x2, float y2)
{
    // axis-aligned connector (thin rectangle)
    drawRect(fminf(x1, x2) - 1, fminf(y1, y2) - 1,
             fmaxf(x1, x2) + 1, fmaxf(y1, y2) + 1);
}

void oopHint(float x, float y)
{
    glColor3f(1.0f, 0.75f, 0.10f);
    drawText(x, y, "CLICK TO OPEN >", GLUT_BITMAP_HELVETICA_12);
}

void oopText(float x, float y, const string& t)
{
    drawText(x, y, t, GLUT_BITMAP_HELVETICA_12);
}

void oopHeader(const string& title, const string& sub)
{
    setWhite();
    drawText(320, 685, title, GLUT_BITMAP_TIMES_ROMAN_24);

    setGray();
    drawText(320, 655, sub);

    drawButton(oopBackBtn, false);
}

// result box shared by the demo views
void drawOOPDemoBox(float x1, float y1, float x2, float y2, float topLine)
{
    setPanel();
    drawRect(x1, y1, x2, y2);

    if (oopDemoLines.empty())
    {
        setGray();
        oopText(x1 + 15, topLine, "Press a button - the result of the real call appears here.");
        return;
    }

    for (size_t i = 0; i < oopDemoLines.size(); i++)
    {
        if (i == 0)
            setWhite();
        else if (oopDemoKind == 1)
            setGreen();
        else if (oopDemoKind == 2)
            setRed();
        else
            setGray();

        drawText(x1 + 15, topLine - i * 24.0f, oopDemoLines[i],
                 GLUT_BITMAP_HELVETICA_12);
    }
}

// ---------------- OVERVIEW ----------------

void drawOOPOverview()
{
    setWhite();
    drawText(320, 685, "OOP CONCEPTS  (AeroDesk FlightBase classes)",
             GLUT_BITMAP_TIMES_ROMAN_24);

    setGray();
    drawText(320, 655,
             "Click any card to open it. All of it runs on the real flight objects.");

    FlightBase* cur = getCurrentFlightObject();
    string curClass = (cur != NULL) ? cur->getClassName() : "";

    drawButton(stlOpenBtn, false);

    // ---- 1. INHERITANCE ----
    setPanel();
    drawRect(320, 370, 780, 630);

    setWhite();
    drawText(335, 603, "1. INHERITANCE  (is-a)");
    oopHint(650, 603);

    drawOOPClassBox(430, 525, 670, 570, "FlightBase  (abstract)",
                    false, 0.45f, 0.20f, 0.55f);

    glColor3f(0.65f, 0.68f, 0.74f);
    drawOOPLine(550, 505, 550, 525);
    drawOOPLine(395, 505, 705, 505);
    drawOOPLine(395, 480, 395, 505);
    drawOOPLine(550, 480, 550, 505);
    drawOOPLine(705, 480, 705, 505);

    drawOOPClassBox(330, 435, 460, 480, "PassengerFlight",
                    curClass == "PassengerFlight", 0.08f, 0.30f, 0.60f);
    drawOOPClassBox(485, 435, 615, 480, "CargoFlight",
                    curClass == "CargoFlight", 0.60f, 0.35f, 0.15f);
    drawOOPClassBox(640, 435, 770, 480, "PrivateJet",
                    curClass == "PrivateJet", 0.10f, 0.50f, 0.50f);

    setGray();
    drawCenteredText(395, 412, "+ Seat Capacity", GLUT_BITMAP_HELVETICA_12);
    drawCenteredText(550, 412, "+ Cargo Weight", GLUT_BITMAP_HELVETICA_12);
    drawCenteredText(705, 412, "+ Owner / VIP Seats", GLUT_BITMAP_HELVETICA_12);

    oopText(335, 385, "Derived classes reuse base data: ID, speed, altitude, fuel.");

    // ---- 2. POLYMORPHISM ----
    setPanel();
    drawRect(800, 370, 1270, 630);

    setWhite();
    drawText(815, 603, "2. POLYMORPHISM  (one call, many forms)");

    setGray();
    oopText(815, 575, "FLIGHT");
    oopText(900, 575, "CLASS");
    oopText(1050, 575, "calculateFuel()");
    oopHint(1150, 575);

    int ci = currentFlightIndex();

    for (int i = 0; i < MAX_FLIGHTS; i++)
    {
        float y = 545.0f - i * 30.0f;
        FlightBase* obj = flightObjects[i];

        if (obj == NULL)
            continue;

        if (i == ci)
        {
            glColor3f(0.12f, 0.20f, 0.34f);
            drawRect(808, y - 8, 1262, y + 20);
        }

        setWhite();
        oopText(815, y, obj->getFlightID());
        oopText(900, y, obj->getClassName());

        glColor3f(0.30f, 1.0f, 0.40f);
        oopText(1050, y, oopFuelText(obj));
    }

    setGray();
    oopText(815, 392, "obj->calculateFuel() -> DERIVED class formula (virtual)");

    // ---- 3. ABSTRACTION ----
    setPanel();
    drawRect(320, 240, 780, 350);

    setWhite();
    drawText(335, 323, "3. ABSTRACTION");
    oopHint(650, 323);

    setGray();
    oopText(335, 298, "virtual string getType() const = 0;   (pure virtual)");
    oopText(335, 280, "-> FlightBase is ABSTRACT: no object of it can exist,");
    oopText(335, 262, "   only PassengerFlight / CargoFlight / PrivateJet.");

    // ---- 4. ENCAPSULATION ----
    setPanel();
    drawRect(320, 70, 780, 232);

    setWhite();
    drawText(335, 205, "4. ENCAPSULATION");
    oopHint(650, 205);

    setGray();
    oopText(335, 180, "private: flightID, speed, altitude, fuel, passengers");
    oopText(335, 162, "public : setSpeed() / setFuel() ... validate input");

    if (cur != NULL)
    {
        glColor3f(0.30f, 1.0f, 0.40f);
        oopText(335, 138,
                "obj->getSpeed() = " + to_string(cur->getSpeed()) +
                " km/h,  getFuel() = " + to_string(cur->getFuel()) + "%");
    }

    setGray();
    oopText(335, 114, "setFuel() clamps to 0..100; the constructor throws");
    oopText(335, 96, "invalid_argument for speed < 0 or fuel > 100.");

    // ---- LIVE OBJECT ----
    setPanel();
    drawRect(800, 70, 1270, 350);

    setWhite();
    drawText(815, 323, "LIVE OBJECT  (flightObjects[])");

    if (cur != NULL)
    {
        const float VX = 1015.0f;

        drawField(815, VX, 295, "Object", cur->getFlightID());
        drawField(815, VX, 269, "Derived class", cur->getClassName());
        drawField(815, VX, 243, "Type", cur->getType());
        drawField(815, VX, 217, cur->getExtraLabel(), cur->getExtraValue());
        drawField(815, VX, 191, "Speed / Alt",
                  to_string(cur->getSpeed()) + " km/h / " +
                  to_string(cur->getAltitude()) + " m");
        drawField(815, VX, 165, "Fuel / Pass",
                  to_string(cur->getFuel()) + "% / " +
                  to_string(cur->getPassengers()));

        setGreen();
        drawText(815, 139, "Fuel Required");
        drawText(VX, 139, oopFuelText(cur) + " (calculated)");
    }

    drawButton(oopPrevBtn, false);
    drawButton(oopNextBtn, false);
}

// ---------------- 1. INHERITANCE VIEW ----------------
// ---- details of one field of the object layout (Inheritance view) ----
// field 0..4 = FlightBase members, 5/6 = the derived class's own members
void oopFieldInfo(int field, int sel, FlightBase* ex,
                  string& l1, string& l2, string& l3, bool& derivedField)
{
    derivedField = (field >= 5);
    string who = (ex != NULL) ? ex->getFlightID() + " -> " : string("");

    if (field == 0)
    {
        l1 = "flightID   -   string   -   private in FlightBase";
        l2 = "Read: getFlightID()   Write: setFlightData(...) / constructor (no separate setter)";
        l3 = (ex != NULL) ? who + "flightID = " + ex->getFlightID() : "";
    }
    else if (field == 1)
    {
        l1 = "speed   -   int   -   private in FlightBase";
        l2 = "Read: getSpeed()   Write: setSpeed(v) rejects v < 0   |   constructor throws if v < 0";
        l3 = (ex != NULL) ? who + "speed = " + to_string(ex->getSpeed()) + " km/h" : "";
    }
    else if (field == 2)
    {
        l1 = "altitude   -   int   -   private in FlightBase";
        l2 = "Read: getAltitude()   Write: setAltitude(v) rejects v < 0";
        l3 = (ex != NULL) ? who + "altitude = " + to_string(ex->getAltitude()) + " m" : "";
    }
    else if (field == 3)
    {
        l1 = "fuel   -   int   -   private in FlightBase";
        l2 = "Read: getFuel()   Write: setFuel(v) clamps to 0..100   |   constructor throws if > 100";
        l3 = (ex != NULL) ? who + "fuel = " + to_string(ex->getFuel()) + " %" : "";
    }
    else if (field == 4)
    {
        l1 = "passengers   -   int   -   private in FlightBase";
        l2 = "Read: getPassengers()   Write: setPassengers(v) rejects v < 0";
        l3 = (ex != NULL) ? who + "passengers = " + to_string(ex->getPassengers()) : "";
    }
    else if (sel == 1)
    {
        PassengerFlight* pf = dynamic_cast<PassengerFlight*>(ex);

        l1 = "capacity   -   int   -   private in PassengerFlight (added by the derived class)";
        l2 = "Read: getCapacity()   Write: setCapacity(c) rejects c < 0   |   passengers <= capacity";
        l3 = (pf != NULL) ? who + "capacity = " + to_string(pf->getCapacity()) + " seats" : "";
    }
    else if (sel == 2)
    {
        CargoFlight* cf = dynamic_cast<CargoFlight*>(ex);

        l1 = "cargoWeight   -   int   -   private in CargoFlight (added by the derived class)";
        l2 = "Read: getCargoWeight()   Write: setCargoWeight(w) rejects w < 0   |   used in fuel formula";
        l3 = (cf != NULL) ? who + "cargoWeight = " + to_string(cf->getCargoWeight()) + " kg" : "";
    }
    else if (field == 5)
    {
        PrivateJet* pj = dynamic_cast<PrivateJet*>(ex);

        l1 = "vipSeats   -   int   -   private in PrivateJet (added by the derived class)";
        l2 = "Read: getVipSeats()   Write: setVipSeats(s) rejects s < 0";
        l3 = (pj != NULL) ? who + "vipSeats = " + to_string(pj->getVipSeats()) : "";
    }
    else
    {
        PrivateJet* pj = dynamic_cast<PrivateJet*>(ex);

        l1 = "ownerName   -   string   -   private in PrivateJet (added by the derived class)";
        l2 = "Read: getOwnerName()   Write: constructor only (no setter)";
        l3 = (pj != NULL) ? who + "ownerName = " + pj->getOwnerName() : "";
    }
}


void drawOOPInheritView()
{
    oopHeader("OOP: INHERITANCE",
              "Pick a class. Same base data, different extra field + formula.");

    int sel = oopClassSel;
    string cname = oopClassNames[sel];

    for (int c = 0; c < 4; c++)
        drawButton(oopClsBtn[c], c == sel);

    // flights of the selected class (clickable)
    setPanel();
    drawRect(320, 70, 560, 350);

    setWhite();
    oopText(335, 328, (sel == 0) ? "ALL FLIGHTS (click to select)"
                                 : "FLIGHTS OF THIS CLASS (click)");

    int list[MAX_FLIGHTS];
    int n = oopFlightsOfClass(sel, list);
    int ci = currentFlightIndex();

    if (n == 0)
    {
        setGray();
        oopText(335, 290, "No flight uses this class.");
    }

    for (int r = 0; r < n; r++)
    {
        float y = 290.0f - r * 38.0f;
        FlightBase* obj = flightObjects[list[r]];

        if (list[r] == ci)
        {
            glColor3f(0.12f, 0.20f, 0.34f);
            drawRect(325, y - 12, 555, y + 22);
        }

        setWhite();
        drawText(335, y, obj->getFlightID() + "  " + obj->getClassName(),
                 GLUT_BITMAP_HELVETICA_12);
    }

    // detail panel
    setPanel();
    drawRect(580, 70, 1270, 630);

    float cr, cg, cb;
    oopClassRGB(cname, cr, cg, cb);
    glColor3f(cr, cg, cb);
    drawRect(580, 598, 1270, 630);

    setWhite();
    drawText(595, 606,
             (sel == 0) ? string("abstract class FlightBase")
                        : "class " + cname + " : public FlightBase");

    setGray();
    oopText(595, 570, (sel == 0) ? "OWN DATA (private, shared by every flight):"
                                 : "INHERITED from FlightBase (reused, not rewritten):");
    setWhite();
    oopText(610, 548, "flightID, speed, altitude, fuel, passengers + setters / getters");

    setGray();
    oopText(595, 515, "EXTRA MEMBER ADDED BY THIS CLASS:");
    setWhite();

    string own, ctor, formula, overrides;

    if (sel == 0)
    {
        own = "none - it only declares the interface";
        ctor = "FlightBase(id, spd, alt, fl, pass) - throws invalid_argument if speed < 0 or fuel > 100";
        formula = "calculateFuel() default body = fuel   (generic fallback)";
        overrides = "declares getType(), getClassName(), getExtraLabel(), getExtraValue() as pure virtual";
    }
    else if (sel == 1)
    {
        own = "private int capacity;     (seats)";
        ctor = "FlightBase(...) runs FIRST, then PassengerFlight checks passengers <= capacity";
        formula = "fuel * (0.7 + 0.3 * passengers / capacity)";
        overrides = "calculateFuel(), display(), getType(), getClassName(), getExtraLabel(), getExtraValue()";
    }
    else if (sel == 2)
    {
        own = "private int cargoWeight;     (kg)";
        ctor = "FlightBase(...) runs FIRST, then CargoFlight stores cargoWeight";
        formula = "fuel + cargoWeight * 0.002";
        overrides = "calculateFuel(), display(), getType(), getClassName(), getExtraLabel(), getExtraValue()";
    }
    else
    {
        own = "private int vipSeats;  private string ownerName;";
        ctor = "FlightBase(...) runs FIRST, then PrivateJet stores vipSeats + ownerName";
        formula = "fuel * 0.6";
        overrides = "calculateFuel(), display(), getType(), getClassName(), getExtraLabel(), getExtraValue()";
    }

    oopText(610, 493, own);

    setGray();
    oopText(595, 460, "CONSTRUCTOR CHAIN:");
    setWhite();
    oopText(610, 438, ctor);

    setGray();
    oopText(595, 405, (sel == 0) ? "DECLARES:" : "OVERRIDES (virtual -> chosen at runtime):");
    setWhite();
    oopText(610, 383, overrides);

    setGray();
    oopText(595, 350, "FUEL FORMULA:");
    glColor3f(0.30f, 1.0f, 0.40f);
    oopText(610, 328, formula);

    // live example = selected flight if it belongs to this class, else first match
    FlightBase* ex = NULL;

    if (n > 0)
    {
        ex = flightObjects[list[0]];

        for (int r = 0; r < n; r++)
            if (list[r] == ci)
                ex = flightObjects[list[r]];
    }

    setGray();
    oopText(595, 295, "LIVE EXAMPLE:");

    if (sel == 0)
    {
        glColor3f(1.0f, 0.45f, 0.45f);
        oopText(610, 273, "FlightBase f;   -> compile error: abstract class has no objects");
    }
    else if (ex != NULL)
    {
        glColor3f(0.30f, 1.0f, 0.40f);
        oopText(610, 273, ex->getFlightID() + ":  " + oopFormulaWithNumbers(ex));
    }

    // object layout in memory: base part first, then derived part.
    // Every box is a button: click it to see that field's details.
    setGray();
    oopText(595, 235, "OBJECT LAYOUT IN MEMORY (click a field):");

    const char* baseFields[5] = { "flightID", "speed", "altitude", "fuel", "passengers" };
    const char* extraFields[2] = { "", "" };
    int extras = 0;

    if (sel == 1)      { extraFields[0] = "capacity"; extras = 1; }
    else if (sel == 2) { extraFields[0] = "cargoWeight"; extras = 1; }
    else if (sel == 3) { extraFields[0] = "vipSeats"; extraFields[1] = "ownerName"; extras = 2; }

    int fields = 5 + extras;

    if (oopFieldSel >= fields)
        oopFieldSel = -1;

    for (int f = 0; f < fields; f++)
    {
        float fx = 595.0f + f * 92.0f;
        bool derived = (f >= 5);

        if (derived)
            glColor3f(cr, cg, cb);
        else
            glColor3f(0.08f, 0.30f, 0.60f);

        drawRect(fx, 160, fx + 88, 205);

        if (f == oopFieldSel)
        {
            float p = hashPulse();
            glColor3f(1.0f * p, 0.85f * p, 0.15f * p);
            hashOutline(fx - 3, 157, fx + 91, 208, 3);
        }

        setWhite();
        drawCenteredText(fx + 44, 177,
                         derived ? extraFields[f - 5] : baseFields[f],
                         GLUT_BITMAP_HELVETICA_12);
    }

    setGray();
    oopText(595, 216, "blue = FlightBase part (inherited)");

    if (sel != 0)
    {
        glColor3f(cr, cg, cb);
        oopText(850, 216, "colour = part added by " + cname);
    }

    // field details
    setDark();
    drawRect(595, 78, 1255, 150);

    if (oopFieldSel < 0)
    {
        setGray();
        oopText(610, 120, "Click a field box to see its type, getter / setter,");
        oopText(610, 100, "validation rule and the live value of the selected flight.");
    }
    else
    {
        string l1, l2, l3;
        bool derivedField = false;

        oopFieldInfo(oopFieldSel, sel, ex, l1, l2, l3, derivedField);

        setWhite();
        oopText(610, 132, l1);

        setGray();
        oopText(610, 112, l2);

        glColor3f(0.30f, 1.0f, 0.40f);
        oopText(610, 90, l3);
    }
}

// ---------------- 2. POLYMORPHISM VIEW ----------------

void drawOOPPolyView()
{
    oopHeader("OOP: POLYMORPHISM",
              "One FlightBase* call, different formula per class. Click a row.");

    setPanel();
    drawRect(320, 385, 1270, 630);

    setGray();
    oopText(335, 600, "FLIGHT");
    oopText(420, 600, "CLASS");
    oopText(570, 600, "getType()");
    oopText(690, 600, "FUEL %");
    oopText(780, 600, "FORMULA (fuel = base reading)");
    oopText(1120, 600, "calculateFuel()");

    int ci = currentFlightIndex();

    for (int i = 0; i < MAX_FLIGHTS; i++)
    {
        float y = 565.0f - i * 40.0f;
        FlightBase* obj = flightObjects[i];

        if (obj == NULL)
            continue;

        if (i == ci)
        {
            glColor3f(0.12f, 0.20f, 0.34f);
            drawRect(325, y - 12, 1265, y + 24);
        }

        string cls = obj->getClassName();
        string formula = (cls == "PassengerFlight") ? "fuel*(0.7+0.3*pass/capacity)"
                       : (cls == "CargoFlight")     ? "fuel + cargoWeight*0.002"
                                                    : "fuel * 0.6";

        setWhite();
        drawText(335, y, obj->getFlightID(), GLUT_BITMAP_HELVETICA_18);
        oopText(420, y, cls);
        oopText(570, y, obj->getType());
        oopText(690, y, to_string(obj->getFuel()) + "%");
        oopText(780, y, formula);

        glColor3f(0.30f, 1.0f, 0.40f);
        drawText(1120, y, oopFuelText(obj));
    }

    // how a virtual call is resolved
    setPanel();
    drawRect(320, 70, 780, 365);

    // ===== OOP PRINT DISPLAY START =====
    if (oopPrintShown)
    {
        setWhite();
        oopText(335, 340, oopPrintTitle);

        float ly = 316.0f;

        for (size_t k = 0; k < oopPrintLines.size() && ly > 76.0f; k++, ly -= 17.0f)
        {
            const string& ln = oopPrintLines[k];

            if (ln.compare(0, 6, "Flight") == 0)
                glColor3f(0.30f, 1.0f, 0.40f);
            else if (ln.compare(0, 6, "   ->") == 0 || ln.find("->") != string::npos)
                glColor3f(1.0f, 0.80f, 0.20f);
            else
                setGray();

            oopText(335, ly, ln);
        }
    }
    else
    // ===== OOP PRINT DISPLAY END =====
    {
    setWhite();
    oopText(335, 340, "HOW THE CALL IS RESOLVED");

    setGray();
    oopText(335, 312, "FlightBase* obj = flightObjects[i];");
    oopText(335, 290, "float f = obj->calculateFuel();      // virtual call");
    oopText(335, 258, "1. The compiler only knows: obj is a FlightBase*");
    oopText(335, 238, "2. At runtime obj's hidden vptr points to its class vtable");
    oopText(335, 218, "3. The vtable slot jumps to the DERIVED override");
    oopText(335, 190, "No if/else on the class anywhere - the vtable chooses.");
    }

    FlightBase* cur = getCurrentFlightObject();

    if (cur != NULL && !oopPrintShown)
    {
        setWhite();
        oopText(335, 150, "Selected: " + cur->getFlightID() + " -> " +
                          cur->getClassName() + "::calculateFuel()");

        glColor3f(0.30f, 1.0f, 0.40f);
        oopText(335, 126, oopFormulaWithNumbers(cur));
    }

    // comparison bars
    setPanel();
    drawRect(800, 70, 1270, 365);

    setWhite();
    oopText(815, 340, "calculateFuel() COMPARISON  (units)");

    for (int i = 0; i < MAX_FLIGHTS; i++)
    {
        FlightBase* obj = flightObjects[i];

        if (obj == NULL)
            continue;

        float y = 305.0f - i * 34.0f;
        float v = obj->calculateFuel();
        float r, g, b;

        oopClassRGB(obj->getClassName(), r, g, b);

        setWhite();
        oopText(815, y, obj->getFlightID());

        glColor3f(r, g, b);
        drawRect(870, y - 6, 870 + v * 1.9f, y + 14);

        setWhite();
        oopText(878 + v * 1.9f, y, to_string((int)(v + 0.5f)));

        if (i == ci)
        {
            float p = hashPulse();
            glColor3f(1.0f * p, 0.85f * p, 0.15f * p);
            hashOutline(866, y - 9, 874 + v * 1.9f, y + 17, 2);
        }
    }

    drawButton(oopPrintOneBtn, false);
    drawButton(oopPrintAllBtn, false);

    if (oopPrintShown)
        drawButton(oopPrintHideBtn, false);
    else
    {
        setGray();
        oopText(815, 138, "Output appears in the box on the left (and in the console).");
    }
}

// ---------------- 3. ABSTRACTION VIEW ----------------

void drawOOPAbstractView()
{
    oopHeader("OOP: ABSTRACTION",
              "FlightBase says WHAT a flight offers; the derived class decides HOW.");

    FlightBase* cur = getCurrentFlightObject();

    setPanel();
    drawRect(320, 330, 780, 630);

    setWhite();
    oopText(335, 605, "PURE VIRTUAL INTERFACE OF FlightBase");

    if (cur != NULL)
    {
        setGray();
        oopText(335, 575, "Live answers from " + cur->getFlightID() +
                          " (" + cur->getClassName() + "):");

        setWhite();
        oopText(335, 540, "virtual string getType() const = 0;");
        glColor3f(0.30f, 1.0f, 0.40f);
        oopText(360, 518, "-> " + cur->getType());

        setWhite();
        oopText(335, 488, "virtual string getClassName() const = 0;");
        glColor3f(0.30f, 1.0f, 0.40f);
        oopText(360, 466, "-> " + cur->getClassName());

        setWhite();
        oopText(335, 436, "virtual string getExtraLabel() const = 0;");
        glColor3f(0.30f, 1.0f, 0.40f);
        oopText(360, 414, "-> " + cur->getExtraLabel());

        setWhite();
        oopText(335, 384, "virtual string getExtraValue() const = 0;");
        glColor3f(0.30f, 1.0f, 0.40f);
        oopText(360, 362, "-> " + cur->getExtraValue());
    }

    setPanel();
    drawRect(800, 330, 1270, 630);

    setWhite();
    oopText(815, 605, "WHAT ABSTRACT MEANS");

    setGray();
    oopText(815, 572, "A class with at least one pure virtual function (= 0)");
    oopText(815, 546, "is abstract.");
    oopText(815, 510, "Cannot:   FlightBase f;          // compile error");
    oopText(815, 484, "Can:      FlightBase* p = new CargoFlight(...);");
    oopText(815, 448, "A derived class must implement ALL four, otherwise it");
    oopText(815, 428, "stays abstract too.");
    oopText(815, 392, "Code using FlightBase* never needs to know which class.");

    drawButton(oopTryAbsBtn, false);
    drawButton(oopTryConBtn, false);

    setGray();
    oopText(335, 312, "Press a button to run the real call:");

    drawOOPDemoBox(320, 70, 1270, 232, 205);
}

// ---------------- 4. ENCAPSULATION VIEW ----------------

void drawOOPEncapView()
{
    oopHeader("OOP: ENCAPSULATION",
              "Data is private; only validated setters change it. Try to break it.");

    oopEnsureDemo();

    setPanel();
    drawRect(320, 330, 780, 630);

    setWhite();
    oopText(335, 605, "class FlightBase {");

    setGray();
    oopText(350, 578, "private:");
    glColor3f(1.0f, 0.75f, 0.10f);
    oopText(370, 556, "string flightID;  int speed;  int altitude;");
    oopText(370, 536, "int fuel;  int passengers;");

    setGray();
    oopText(350, 504, "public:");
    setWhite();
    oopText(370, 482, "void setSpeed(int v)  { if (v >= 0) speed = v; }");
    oopText(370, 462, "void setFuel(int v)   { clamp v to 0..100; fuel = v; }");
    oopText(370, 442, "int  getSpeed() const;   int getFuel() const;");

    setGray();
    oopText(335, 418, "};");

    glColor3f(1.0f, 0.45f, 0.45f);
    oopText(335, 388, "demo.speed = -5;      // compile error: 'speed' is private");

    setGray();
    oopText(335, 350, "Only setters / getters can touch the data.");

    // demo object state
    setPanel();
    drawRect(800, 330, 1270, 630);

    setWhite();
    oopText(815, 605, "DEMO OBJECT  (PassengerFlight \"DEMO01\")");

    if (oopDemoObj != NULL)
    {
        setGray();
        oopText(1050, 575, "getSpeed() = " + to_string(oopDemoObj->getSpeed()));
        oopText(815, 575, "getFuel() = " + to_string(oopDemoObj->getFuel()) + "%");
    }

    for (int k = 0; k < 8; k++)
        drawButton(oopEncBtn[k], false);

    drawOOPDemoBox(320, 70, 1270, 310, 280);

    setGray();
    oopText(335, 100, "Red buttons make the CONSTRUCTOR throw invalid_argument "
                      "(same exceptions as the E-key demo).");
}

// ===== STL EXTRAS START =====
// ---------------- 5. STL CONTAINERS VIEW ----------------

void drawOOPStlView()
{
    oopHeader("OOP: STL CONTAINERS",
              "priority_queue, set, unordered_map vs map - on AeroDesk data.");

    // ================= priority_queue =================
    setPanel();
    drawRect(320, 330, 790, 630);

    setWhite();
    oopText(335, 605, "priority_queue<EmergencyEntry>  -  lowest fuel departs first");

    setGray();
    oopText(335, 575, "FIFO (order added)");
    oopText(565, 575, "PRIORITY (pop order)");

    // copy the heap and pop the copy, so the real queue is untouched
    vector<EmergencyEntry> pri;
    {
        priority_queue<EmergencyEntry, vector<EmergencyEntry>, EmergencyCompare> copy = emergencyPQ;

        while (!copy.empty())
        {
            pri.push_back(copy.top());
            copy.pop();
        }
    }

    if (emergencyFIFO.empty())
    {
        setGray();
        oopText(335, 545, "Queue is empty - press ADD ALL FLIGHTS.");
    }

    for (size_t r = 0; r < emergencyFIFO.size() && r < 6; r++)
    {
        float y = 548.0f - r * 30.0f;

        setWhite();
        oopText(335, y, emergencyFIFO[r].flightID + "  fuel " +
                        to_string(emergencyFIFO[r].fuel) + "%" +
                        (emergencyFIFO[r].emergency ? "  EMERGENCY" : ""));

        if (r == 0)
        {
            glColor3f(0.07f, 0.30f, 0.18f);
            drawRect(560, y - 8, 785, y + 18);
        }

        if (r < pri.size())
        {
            if (r == 0)
                glColor3f(0.30f, 1.0f, 0.40f);
            else if (pri[r].emergency)
                glColor3f(1.0f, 0.45f, 0.45f);
            else
                setWhite();

            oopText(565, y, to_string(r + 1) + ". " + pri[r].flightID +
                            "  fuel " + to_string(pri[r].fuel) + "%" +
                            (pri[r].emergency ? "  EMERGENCY" : ""));
        }
    }

    if (!pri.empty())
    {
        glColor3f(0.30f, 1.0f, 0.40f);
        oopText(335, 370, "top() = " + pri[0].flightID + " (fuel " +
                          to_string(pri[0].fuel) + "%)  departs next");
    }

    setGray();
    oopText(335, 348, "size() = " + to_string(emergencyPQ.size()) +
                      "   |   Waiting Queue itself stays FIFO");

    drawButton(stlAddAllBtn, false);
    drawButton(stlAddCurBtn, false);
    drawButton(stlClearBtn, false);
    drawButton(stlEmergBtn, false);
    drawButton(stlDepartBtn, false);

    // departed + message
    setPanel();
    drawRect(320, 70, 790, 205);

    setWhite();
    oopText(335, 180, "DEPARTED (in order):");

    string dep;

    for (size_t i = 0; i < emergencyDeparted.size(); i++)
    {
        if (i > 0)
            dep += "  >  ";

        dep += emergencyDeparted[i];
    }

    if (dep.empty())
        dep = "none yet";

    setGray();
    oopText(335, 156, dep);

    if (!stlMessage.empty())
    {
        if (stlMessageKind == 1)
            setGreen();
        else if (stlMessageKind == 2)
            setRed();
        else
            setGray();

        oopText(335, 120, stlMessage);
    }

    // ================= set<string> =================
    setPanel();
    drawRect(810, 500, 1270, 630);

    setWhite();
    oopText(825, 605, "set<string>  -  unique destinations");

    string names;

    for (set<string>::iterator it = destinationSet.begin();
         it != destinationSet.end(); ++it)
    {
        if (!names.empty())
            names += ", ";

        names += *it;
    }

    setGray();
    oopText(825, 578, to_string(passengerRecordCount) + " passenger records -> " +
                      to_string(destinationSet.size()) + " unique destinations");

    glColor3f(0.30f, 1.0f, 0.40f);
    oopText(825, 554, names);

    if (!destinationSet.empty())
    {
        set<string> tmp = destinationSet;
        bool inserted = tmp.insert(*destinationSet.begin()).second;

        setGray();
        oopText(825, 526, "insert(\"" + *destinationSet.begin() + "\") again -> " +
                          (inserted ? "added" : "ignored") +
                          ", size stays " + to_string(tmp.size()));
    }

    // ================= map vs unordered_map =================
    setPanel();
    drawRect(810, 70, 1270, 485);

    setWhite();
    oopText(825, 460, "map  vs  unordered_map  -  Passenger ID search");

    setGray();
    oopText(825, 420, "ID:");

    setDark();
    drawRect(860, 400, 1010, 442);

    setWhite();
    drawText(870, 415, stlInput + "_");

    drawButton(stlCompareBtn, false);

    setGray();
    oopText(825, 372, "");
    oopText(1000, 372, "map");
    oopText(1130, 372, "unordered_map");

    oopText(825, 346, "Found at index");
    oopText(825, 320, "ns per lookup");
    oopText(825, 294, "Complexity");
    oopText(825, 268, "Structure");

    setWhite();
    oopText(1000, 294, "O(log n)");
    oopText(1130, 294, "O(1) average");
    oopText(1000, 268, "red-black tree");
    oopText(1130, 268, "hash table");

    if (stlCompared)
    {
        setWhite();
        oopText(1000, 346, stlMapIdx >= 0 ? to_string(stlMapIdx) : string("not found"));
        oopText(1130, 346, stlHashIdx >= 0 ? to_string(stlHashIdx) : string("not found"));

        char b1[32], b2[32];
        snprintf(b1, sizeof(b1), "%.1f ns", stlMapNs);
        snprintf(b2, sizeof(b2), "%.1f ns", stlHashNs);

        glColor3f(0.30f, 1.0f, 0.40f);
        oopText(1000, 320, b1);
        oopText(1130, 320, b2);

        setGray();
        oopText(825, 242, "ID " + to_string(stlCompareID) +
                          ": same answer from both; time changes every run.");
    }
    else
    {
        setGray();
        oopText(825, 346, "type an ID, press COMPARE (or ENTER)");
    }

    // iteration order: map is sorted, unordered_map is not
    string ordMap, ordHash;
    int shown = 0;

    for (map<int, Passenger*>::iterator it = passengerMap.begin();
         it != passengerMap.end() && shown < 6; ++it, ++shown)
        ordMap += to_string(it->first) + " ";

    shown = 0;

    for (unordered_map<int, Passenger*>::iterator it = passengerHashMap.begin();
         it != passengerHashMap.end() && shown < 6; ++it, ++shown)
        ordHash += to_string(it->first) + " ";

    setWhite();
    oopText(825, 205, "map iteration order (sorted):");
    glColor3f(0.30f, 1.0f, 0.40f);
    oopText(825, 183, ordMap);

    setWhite();
    oopText(825, 150, "unordered_map iteration order (hash order):");
    glColor3f(1.0f, 0.75f, 0.10f);
    oopText(825, 128, ordHash);

    setGray();
    oopText(825, 92, "560 records is small: both are fast, the gap grows with n.");
}
// ===== STL EXTRAS END =====


void drawOOPPage()
{
    updateOOPFlightObject();     // keep the live object in sync

    if (oopView == OOP_INHERIT)
        drawOOPInheritView();
    else if (oopView == OOP_POLY)
        drawOOPPolyView();
    else if (oopView == OOP_ABSTRACT)
        drawOOPAbstractView();
    else if (oopView == OOP_ENCAP)
        drawOOPEncapView();
    else if (oopView == OOP_STL)
        drawOOPStlView();
    else
        drawOOPOverview();
}
// ===== OOP DASHBOARD END =====


// ===== SEARCH SORT START =====
// =====================================================
// DASH_SORT - PL: SEARCHING AND SORTING (step by step)
//   Binary search : on the selected flight's passengers
//                   (passengerRecords[] of one flight is sorted by ID)
//   Bubble sort   : on a COPY of the flights (speed / fuel /
//                   passengers) or of the first 15 passengers of the
//                   selected flight (age) - real data is not changed
// Every STEP runs exactly one comparison of the real algorithm; AUTO
// plays the steps one after another.
// =====================================================

int sortTab = 0;              // 0 = binary search, 1 = bubble sort
bool sortAuto = false;
int sortTick = 0;
string sortInput = "";        // target Passenger ID (binary search)
string sortMessage = "";

// ---------------- binary search state ----------------
int srchFlight = -1;          // flight the state was built for
int srchBase = 0;             // index of the flight's first passenger
int srchN = 0;
int srchTarget = -1;
int srchLo = 0, srchHi = -1, srchMid = -1;
int srchCompares = 0;
bool srchStarted = false, srchDone = false, srchFound = false;
int srchFoundPos = -1;
int srchRandomCount = 0;
vector<string> srchLog;

void srchReset()
{
    int fi = currentFlightIndex();

    srchFlight = fi;
    srchBase = getFlightStartIndex(fi);
    srchN = flights[fi].passengers;

    srchTarget = -1;
    srchLo = 0;
    srchHi = srchN - 1;
    srchMid = -1;
    srchCompares = 0;
    srchStarted = false;
    srchDone = false;
    srchFound = false;
    srchFoundPos = -1;
    srchLog.clear();
    sortMessage = "";
}

void srchStep()
{
    if (srchFlight != currentFlightIndex())
        srchReset();

    if (srchDone)
        return;

    if (!srchStarted)
    {
        if (sortInput.empty())
        {
            sortMessage = "Type a Passenger ID first (or press RANDOM ID).";
            sortAuto = false;
            return;
        }

        srchTarget = atoi(sortInput.c_str());
        srchStarted = true;
        sortMessage = "";

        srchLog.push_back("search ID " + to_string(srchTarget) + " in " +
                          to_string(srchN) + " sorted IDs (" +
                          to_string(passengerRecords[srchBase].passengerID) + "-" +
                          to_string(passengerRecords[srchBase + srchN - 1].passengerID) + ")");
    }

    if (srchLo > srchHi)
    {
        srchDone = true;
        return;
    }

    srchMid = srchLo + (srchHi - srchLo) / 2;

    int midID = passengerRecords[srchBase + srchMid].passengerID;

    srchCompares++;

    string head = "#" + to_string(srchCompares) + "  mid=" + to_string(srchMid) +
                  " (ID " + to_string(midID) + ") ";

    if (midID == srchTarget)
    {
        srchFound = true;
        srchFoundPos = srchMid;
        srchDone = true;
        srchLog.push_back(head + "== target  FOUND");
    }
    else if (midID < srchTarget)
    {
        srchLog.push_back(head + "< target  -> go RIGHT");
        srchLo = srchMid + 1;
    }
    else
    {
        srchLog.push_back(head + "> target  -> go LEFT");
        srchHi = srchMid - 1;
    }

    if (!srchFound && srchLo > srchHi)
    {
        srchDone = true;
        srchLog.push_back("range is empty (lo > hi)  -> NOT FOUND");
    }
}

// RANDOM ID: every 4th press picks an ID that is not in the flight
void srchRandomID()
{
    int fi = currentFlightIndex();

    if (srchFlight != fi)
        srchReset();

    srchRandomCount++;

    int id;

    if (srchRandomCount % 4 == 0)
    {
        id = passengerRecords[srchBase + srchN - 1].passengerID + 7;
    }
    else
    {
        int idx = (srchRandomCount * 37 + 11) % srchN;
        id = passengerRecords[srchBase + idx].passengerID;
    }

    sortInput = to_string(id);
    srchReset();
}

// ---------------- bubble sort state ----------------
int bubData = 0;              // 0 = flights, 1 = passengers (age)
int bubKey = 0;               // flights: 0 speed, 1 fuel, 2 passengers
int bubFlight = -1;
vector<int> bubVal;
vector<string> bubLabel;
int bubI = 0, bubJ = 0;
bool bubSwappedPass = false, bubDone = false;
int bubCompares = 0, bubSwaps = 0;
int bubA = -1, bubB = -1;
bool bubLastSwap = false;
string bubLast = "";

const char* bubKeyName()
{
    if (bubData == 1)
        return "KEY: AGE";

    if (bubKey == 0) return "KEY: SPEED";
    if (bubKey == 1) return "KEY: FUEL";
    return "KEY: PASSENGERS";
}

void bubReset()
{
    bubVal.clear();
    bubLabel.clear();

    bubFlight = currentFlightIndex();

    if (bubData == 0)
    {
        for (int k = 0; k < MAX_FLIGHTS; k++)
        {
            int v = (bubKey == 0) ? flights[k].speed
                  : (bubKey == 1) ? flights[k].fuel
                                  : flights[k].passengers;

            bubVal.push_back(v);
            bubLabel.push_back(flights[k].flightID);
        }
    }
    else
    {
        int start = getFlightStartIndex(bubFlight);
        int m = flights[bubFlight].passengers;

        if (m > 15)
            m = 15;

        for (int k = 0; k < m; k++)
        {
            bubVal.push_back(passengerRecords[start + k].age);
            bubLabel.push_back(to_string(passengerRecords[start + k].passengerID));
        }
    }

    bubI = 0;
    bubJ = 0;
    bubSwappedPass = false;
    bubDone = false;
    bubCompares = 0;
    bubSwaps = 0;
    bubA = -1;
    bubB = -1;
    bubLastSwap = false;
    bubLast = "press STEP: compare neighbours, swap if left > right";
}

void bubStep()
{
    if (bubDone)
        return;

    int n = (int)bubVal.size();

    if (n < 2)
    {
        bubDone = true;
        return;
    }

    bubA = bubJ;
    bubB = bubJ + 1;
    bubCompares++;

    string l = to_string(bubVal[bubA]);
    string r = to_string(bubVal[bubB]);

    if (bubVal[bubA] > bubVal[bubB])
    {
        swap(bubVal[bubA], bubVal[bubB]);
        swap(bubLabel[bubA], bubLabel[bubB]);

        bubSwaps++;
        bubSwappedPass = true;
        bubLastSwap = true;
        bubLast = "compare " + l + " > " + r + "  -> SWAP";
    }
    else
    {
        bubLastSwap = false;
        bubLast = "compare " + l + " <= " + r + "  -> no swap";
    }

    bubJ++;

    if (bubJ >= n - 1 - bubI)
    {
        bubI++;

        if (!bubSwappedPass || bubI >= n - 1)
        {
            bubDone = true;
            bubA = -1;
            bubB = -1;
            bubLast += "   |   no more swaps: SORTED";
        }
        else
        {
            bubSwappedPass = false;
            bubJ = 0;
        }
    }
}

// ---------------- shared ----------------

void sortStepCurrent()
{
    if (sortTab == 0)
        srchStep();
    else
        bubStep();
}

// a flight change on another page must not leave stale data here
void sortSyncFlight()
{
    if (srchFlight != currentFlightIndex())
        srchReset();

    if (bubData == 1 && bubFlight != currentFlightIndex())
        bubReset();
}

void sortPageEnter()
{
    sortAuto = false;
    sortInput = "";
    srchReset();
    bubReset();
}

void updateSortAnimation()
{
    sortTick++;

    if (sortAuto && dashboardPage == DASH_SORT && sortTick % 8 == 0)
    {
        sortSyncFlight();
        sortStepCurrent();

        if ((sortTab == 0 && srchDone) || (sortTab == 1 && bubDone))
            sortAuto = false;
    }
}

// ---------------- buttons ----------------
Btn sortTabBtn[2] =
{
    { 320, 590, 570, 630, "BINARY SEARCH", 0.08f, 0.30f, 0.60f },
    { 580, 590, 830, 630, "BUBBLE SORT",   0.08f, 0.30f, 0.60f }
};

Btn sortStepBtn   = { 590,  462,  700, 508, "STEP",       0.08f, 0.42f, 0.78f };
Btn sortAutoBtn   = { 710,  462,  820, 508, "AUTO: OFF",  0.10f, 0.55f, 0.25f };
Btn sortResetBtn  = { 830,  462,  940, 508, "RESET",      0.16f, 0.25f, 0.35f };
Btn sortRandBtn   = { 950,  462, 1060, 508, "RANDOM ID",  0.60f, 0.35f, 0.15f };
Btn sortFlightBtn = { 1070, 462, 1270, 508, "NEXT FLIGHT", 0.16f, 0.25f, 0.35f };

Btn bubDataBtn    = { 320,  462,  520, 508, "DATA: FLIGHTS", 0.60f, 0.35f, 0.15f };
Btn bubKeyBtn     = { 530,  462,  700, 508, "KEY: SPEED",    0.60f, 0.35f, 0.15f };
Btn bubStepBtn    = { 710,  462,  810, 508, "STEP",           0.08f, 0.42f, 0.78f };
Btn bubAutoBtn    = { 820,  462,  920, 508, "AUTO: OFF",      0.10f, 0.55f, 0.25f };
Btn bubResetBtn   = { 930,  462, 1060, 508, "RESET",          0.16f, 0.25f, 0.35f };

void sortHandleClick(float wx, float wy)
{
    sortSyncFlight();

    for (int t = 0; t < 2; t++)
    {
        if (inBtn(sortTabBtn[t], wx, wy) && sortTab != t)
        {
            sortTab = t;
            sortAuto = false;
            return;
        }
    }

    if (sortTab == 0)
    {
        if (inBtn(sortStepBtn, wx, wy))
            srchStep();
        else if (inBtn(sortAutoBtn, wx, wy))
            sortAuto = !sortAuto && !srchDone;
        else if (inBtn(sortResetBtn, wx, wy))
        {
            sortAuto = false;
            srchReset();
        }
        else if (inBtn(sortRandBtn, wx, wy))
        {
            sortAuto = false;
            srchRandomID();
        }
        else if (inBtn(sortFlightBtn, wx, wy))
        {
            sortAuto = false;
            nextFlight();
            srchReset();
        }
    }
    else
    {
        if (inBtn(bubDataBtn, wx, wy))
        {
            sortAuto = false;
            bubData = 1 - bubData;
            bubReset();
        }
        else if (inBtn(bubKeyBtn, wx, wy))
        {
            if (bubData == 0)
            {
                sortAuto = false;
                bubKey = (bubKey + 1) % 3;
                bubReset();
            }
        }
        else if (inBtn(bubStepBtn, wx, wy))
            bubStep();
        else if (inBtn(bubAutoBtn, wx, wy))
            sortAuto = !sortAuto && !bubDone;
        else if (inBtn(bubResetBtn, wx, wy))
        {
            sortAuto = false;
            bubReset();
        }
        else if (bubData == 1 && inBtn(sortFlightBtn, wx, wy))
        {
            sortAuto = false;
            nextFlight();
            bubReset();
        }
    }
}

// ---------------- drawing ----------------

void sortTriangleDown(float x, float y)
{
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y);
    glVertex2f(x - 6, y + 11);
    glVertex2f(x + 6, y + 11);
    glEnd();
}

void sortTriangleUp(float x, float y)
{
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y);
    glVertex2f(x - 6, y - 11);
    glVertex2f(x + 6, y - 11);
    glEnd();
}

void drawBinarySearchView()
{
    int fi = currentFlightIndex();

    // ---- info line ----
    setWhite();
    drawText(320, 545,
             string("Flight ") + flights[fi].flightID + " - " +
             to_string(srchN) + " passengers sorted by ID (" +
             to_string(passengerRecords[srchBase].passengerID) + " - " +
             to_string(passengerRecords[srchBase + srchN - 1].passengerID) + ")");

    // ---- target input + buttons ----
    setGray();
    drawText(320, 478, "TARGET ID");

    setPanel();
    drawRect(425, 462, 570, 508);

    setWhite();
    drawText(437, 478, sortInput + "_");

    sortAutoBtn.label = sortAuto ? "AUTO: ON" : "AUTO: OFF";

    drawButton(sortStepBtn, false);
    drawButton(sortAutoBtn, sortAuto);
    drawButton(sortResetBtn, false);
    drawButton(sortRandBtn, false);
    drawButton(sortFlightBtn, false);

    // ---- passenger strip ----
    setPanel();
    drawRect(320, 300, 1270, 450);

    float x0 = 335.0f;
    float totalW = 920.0f;
    float cellW = totalW / (float)srchN;

    for (int i = 0; i < srchN; i++)
    {
        float x = x0 + i * cellW;
        float x2 = x + cellW - (cellW > 3.0f ? 1.0f : 0.0f);

        bool inRange = (!srchStarted) || (i >= srchLo && i <= srchHi);

        if (srchDone && srchFound && i == srchFoundPos)
            glColor3f(0.20f, 0.85f, 0.35f);
        else if (srchStarted && !srchDone && i == srchMid)
            glColor3f(1.0f, 0.75f, 0.10f);
        else if (inRange)
            glColor3f(0.10f, 0.35f, 0.65f);
        else
            glColor3f(0.16f, 0.17f, 0.20f);

        drawRect(x, 350, x2, 410);
    }

    // mid marker (amber, points down at the compared cell)
    if (srchMid >= 0 && srchMid < srchN)
    {
        float mx = x0 + (srchMid + 0.5f) * cellW;

        glColor3f(1.0f, 0.75f, 0.10f);
        sortTriangleDown(mx, 412);
        drawText(mx - 20, 428, "mid=" + to_string(srchMid), GLUT_BITMAP_HELVETICA_12);
    }

    // lo / hi markers (blue, point up at the range edges)
    if (srchStarted && srchLo >= 0 && srchLo < srchN)
    {
        float lx = x0 + (srchLo + 0.5f) * cellW;

        glColor3f(0.35f, 0.65f, 1.0f);
        sortTriangleUp(lx, 348);
        drawText(lx - 10, 320, "lo", GLUT_BITMAP_HELVETICA_12);
    }

    if (srchStarted && srchHi >= 0 && srchHi < srchN)
    {
        float hx = x0 + (srchHi + 0.5f) * cellW;

        glColor3f(0.35f, 0.65f, 1.0f);
        sortTriangleUp(hx, 348);
        drawText(hx - 10, 306, "hi", GLUT_BITMAP_HELVETICA_12);
    }

    setGray();
    drawText(335, 438,
             "idx 0 = ID " + to_string(passengerRecords[srchBase].passengerID),
             GLUT_BITMAP_HELVETICA_12);
    drawText(1100, 438,
             "idx " + to_string(srchN - 1) + " = ID " +
             to_string(passengerRecords[srchBase + srchN - 1].passengerID),
             GLUT_BITMAP_HELVETICA_12);

    // ---- state ----
    setPanel();
    drawRect(320, 70, 780, 290);

    setWhite();
    drawText(335, 262, "STATE");

    setGray();
    drawText(335, 236,
             "lo = " + to_string(srchLo) + "    hi = " + to_string(srchHi) +
             "    mid = " + (srchMid >= 0 ? to_string(srchMid) : string("-")));
    drawText(335, 210, "comparisons = " + to_string(srchCompares) +
                       "    target = " +
                       (srchStarted ? to_string(srchTarget) : string("-")));

    if (!sortMessage.empty())
    {
        setRed();
        drawText(335, 176, sortMessage, GLUT_BITMAP_HELVETICA_12);
    }
    else if (srchDone && srchFound)
    {
        setGreen();
        drawText(335, 176,
                 "FOUND: " + passengerRecords[srchBase + srchFoundPos].name +
                 " (idx " + to_string(srchFoundPos) + ")");
    }
    else if (srchDone)
    {
        setRed();
        drawText(335, 176, "NOT FOUND after " + to_string(srchCompares) + " comparisons");
    }
    else
    {
        setGray();
        drawText(335, 176, srchStarted ? "searching..." : "press STEP or AUTO to start");
    }

    int ceilLog = 0;

    while ((1 << ceilLog) < srchN + 1)
        ceilLog++;

    int linear = (srchDone && srchFound) ? srchFoundPos + 1 : srchN;

    setGray();
    drawText(335, 134, "Linear search needs: " + to_string(linear) + " comparisons",
             GLUT_BITMAP_HELVETICA_12);
    drawText(335, 114, "Binary search worst case: ceil(log2(n+1)) = " +
                       to_string(ceilLog) + " comparisons",
             GLUT_BITMAP_HELVETICA_12);
    drawText(335, 94, "Needs sorted data: O(log n) vs O(n)",
             GLUT_BITMAP_HELVETICA_12);

    // ---- step log ----
    setPanel();
    drawRect(800, 70, 1270, 290);

    setWhite();
    drawText(815, 262, "STEP LOG");

    int first = (int)srchLog.size() > 8 ? (int)srchLog.size() - 8 : 0;

    for (int i = first; i < (int)srchLog.size(); i++)
    {
        float y = 236.0f - (i - first) * 21.0f;

        if (i == (int)srchLog.size() - 1)
            glColor3f(0.30f, 1.0f, 0.40f);
        else
            setGray();

        drawText(815, y, srchLog[i], GLUT_BITMAP_HELVETICA_12);
    }
}

void drawBubbleSortView()
{
    int n = (int)bubVal.size();

    setWhite();
    drawText(320, 545,
             bubData == 0
                 ? string("Sorting the 5 flights by ") +
                   (bubKey == 0 ? "speed" : bubKey == 1 ? "fuel" : "passengers") +
                   " (ascending) - a copy, the real list is untouched"
                 : string("Sorting the first ") + to_string(n) +
                   " passengers of flight " + flights[bubFlight].flightID +
                   " by age (ascending)");

    bubDataBtn.label = (bubData == 0) ? "DATA: FLIGHTS" : "DATA: PASSENGERS";
    bubKeyBtn.label = bubKeyName();
    bubAutoBtn.label = sortAuto ? "AUTO: ON" : "AUTO: OFF";

    drawButton(bubDataBtn, false);
    drawButton(bubKeyBtn, false);
    drawButton(bubStepBtn, false);
    drawButton(bubAutoBtn, sortAuto);
    drawButton(bubResetBtn, false);

    if (bubData == 1)
        drawButton(sortFlightBtn, false);

    // ---- bars ----
    setPanel();
    drawRect(320, 190, 1270, 450);

    int maxV = 1;

    for (int i = 0; i < n; i++)
        if (bubVal[i] > maxV)
            maxV = bubVal[i];

    float slot = 930.0f / (float)(n > 0 ? n : 1);
    float barW = slot * 0.70f;

    if (barW > 100.0f)
        barW = 100.0f;

    int sortedFrom = bubDone ? 0 : n - bubI;

    for (int i = 0; i < n; i++)
    {
        float cx = 335.0f + slot * (i + 0.5f);
        float h = 24.0f + ((float)bubVal[i] / (float)maxV) * 150.0f;

        if (i == bubA || i == bubB)
        {
            if (bubLastSwap)
                glColor3f(0.85f, 0.25f, 0.22f);
            else
                glColor3f(1.0f, 0.75f, 0.10f);
        }
        else if (i >= sortedFrom)
            glColor3f(0.12f, 0.60f, 0.30f);
        else
            glColor3f(0.10f, 0.35f, 0.65f);

        drawRect(cx - barW / 2, 235, cx + barW / 2, 235 + h);

        setWhite();
        drawCenteredText(cx, 235 + h + 8, to_string(bubVal[i]),
                         GLUT_BITMAP_HELVETICA_12);

        setGray();
        drawCenteredText(cx, 212, bubLabel[i], GLUT_BITMAP_HELVETICA_12);
    }

    // ---- info ----
    setPanel();
    drawRect(320, 70, 1270, 180);

    if (bubLastSwap && !bubDone)
        setRed();
    else if (bubDone)
        setGreen();
    else
        setWhite();

    drawText(335, 150, bubLast, GLUT_BITMAP_HELVETICA_12);

    setGray();
    drawText(335, 122, "comparisons = " + to_string(bubCompares) +
                       "    swaps = " + to_string(bubSwaps) +
                       "    passes = " + to_string(bubI));

    if (bubDone)
    {
        setGreen();
        drawText(335, 94, "SORTED (ascending)");
    }
    else
    {
        setGray();
        drawText(335, 94, "sorting...  green = already in final place");
    }

    setGray();
    drawText(800, 150, "Bubble sort: O(n^2) worst case, O(n) best",
             GLUT_BITMAP_HELVETICA_12);
    drawText(800, 128, "(early exit when a pass makes no swap)",
             GLUT_BITMAP_HELVETICA_12);
    drawText(800, 104, "n = " + to_string(n) + " -> at most " +
                       to_string(n * (n - 1) / 2) + " comparisons",
             GLUT_BITMAP_HELVETICA_12);
    drawText(800, 80, "yellow = compared, red = just swapped",
             GLUT_BITMAP_HELVETICA_12);
}

void drawSortPage()
{
    sortSyncFlight();

    setWhite();
    drawText(320, 685, "PL: SEARCHING AND SORTING", GLUT_BITMAP_TIMES_ROMAN_24);

    setGray();
    drawText(320, 655,
             "Binary search and bubble sort, step by step, on AeroDesk flights and passengers.");

    drawButton(sortTabBtn[0], sortTab == 0);
    drawButton(sortTabBtn[1], sortTab == 1);

    if (sortTab == 0)
        drawBinarySearchView();
    else
        drawBubbleSortView();
}
// ===== SEARCH SORT END =====


// ===== CLIPPING START =====
// =====================================================
// DASH_CLIP - CG: CLIPPING AND VIEWING (AeroDesk airspace radar)
//   World  : a map of AeroDesk's airspace (hub + the 5 flight routes
//            AI101..AI505 and two weather / restricted zones)
//   Window : the clip window = the radar's field of view (move it,
//            resize it, or click the map)
//   Lines  : Cohen-Sutherland (outcodes, trivial accept / reject,
//            edge intersections) on the selected flight's route
//   Polygon: Sutherland-Hodgman against LEFT, RIGHT, BOTTOM, TOP
//   Viewing: the clipped result is mapped window -> viewport onto
//            the RADAR panel (xv = xvmin + (xw - xwmin) * sx)
// Every STEP is one real step of the algorithm. All geometry is
// drawn with quads / the project's Bresenham circle - no GL line
// primitives.
// =====================================================

struct ClipPt
{
    double x, y;
};

struct ClipWin
{
    double xmin, ymin, xmax, ymax;
};

// world = a lon/lat style box around India
const double CLIP_WX0 = 66.0, CLIP_WX1 = 98.0;
const double CLIP_WY0 = 7.0,  CLIP_WY1 = 37.0;

// hub (Nagpur area) and the five destinations of AI101..AI505
const ClipPt clipHub = { 79.1, 21.1 };

const ClipPt clipDest[MAX_FLIGHTS] =
{
    { 77.1, 28.6 },   // AI101  Delhi
    { 72.9, 19.1 },   // AI202  Mumbai
    { 77.6, 13.0 },   // AI303  Bangalore
    { 80.3, 13.1 },   // AI404  Chennai
    { 88.4, 22.6 }    // AI505  Kolkata
};

const float clipRouteCol[MAX_FLIGHTS][3] =
{
    { 0.25f, 0.85f, 0.95f },
    { 1.00f, 0.60f, 0.20f },
    { 0.95f, 0.35f, 0.80f },
    { 0.55f, 0.95f, 0.30f },
    { 0.95f, 0.40f, 0.40f }
};

// two convex zones (clockwise)
const ClipPt clipPolyStorm[6] =
{
    { 71, 24 }, { 76, 29.5 }, { 84, 30 }, { 89, 25 }, { 86, 17.5 }, { 76, 15 }
};

const ClipPt clipPolyRestricted[4] =
{
    { 68, 18 }, { 82, 34 }, { 94, 20 }, { 80, 8 }
};

const char* clipZoneName[2] = { "STORM ZONE", "RESTRICTED ZONE" };

// ---------------- state ----------------
int clipTab = 0;               // 0 = Cohen-Sutherland, 1 = Sutherland-Hodgman
bool clipAuto = false;
int clipTick = 0;
int clipZone = 0;
ClipWin clipWin = { 73.0, 18.0, 85.0, 24.0 };   // 2:1 like the radar viewport
int clipRoute = -1;            // flight whose route is being stepped

// ---------------- Cohen-Sutherland ----------------
struct CSStep
{
    double x0, y0, x1, y1;     // segment AFTER this step
    int c0, c1;
    string text;
    int kind;                  // 0 start, 1 clip, 2 accept, 3 reject
    double ix, iy;
    bool hasI;
};

vector<CSStep> csSteps;
int csPos = 0;

bool csAllVis[MAX_FLIGHTS];
ClipPt csAllA[MAX_FLIGHTS];
ClipPt csAllB[MAX_FLIGHTS];

string clipNum(double v)
{
    char b[32];
    snprintf(b, sizeof(b), "%.1f", v);
    return string(b);
}

// outcode bits: 8 = TOP, 4 = BOTTOM, 2 = RIGHT, 1 = LEFT
int csCode(double x, double y, const ClipWin& w)
{
    int c = 0;

    if (x < w.xmin)      c |= 1;
    else if (x > w.xmax) c |= 2;

    if (y < w.ymin)      c |= 4;
    else if (y > w.ymax) c |= 8;

    return c;
}

string csBits(int c)
{
    string s;
    s += (c & 8) ? '1' : '0';
    s += (c & 4) ? '1' : '0';
    s += (c & 2) ? '1' : '0';
    s += (c & 1) ? '1' : '0';
    return s;
}

// Runs the whole algorithm and records every step.
// Returns true if any part of the segment is visible.
bool csRun(ClipPt a, ClipPt b, const ClipWin& w, vector<CSStep>& steps,
           const string& n0, const string& n1)
{
    steps.clear();

    double x0 = a.x, y0 = a.y, x1 = b.x, y1 = b.y;
    int c0 = csCode(x0, y0, w);
    int c1 = csCode(x1, y1, w);

    CSStep s;
    s.x0 = x0; s.y0 = y0; s.x1 = x1; s.y1 = y1;
    s.c0 = c0; s.c1 = c1;
    s.kind = 0;
    s.hasI = false;
    s.ix = s.iy = 0;
    s.text = "outcodes (T B R L):  " + n0 + " = " + csBits(c0) +
             ",  " + n1 + " = " + csBits(c1);
    steps.push_back(s);

    for (int guard = 0; guard < 8; guard++)
    {
        if ((c0 | c1) == 0)
        {
            s.x0 = x0; s.y0 = y0; s.x1 = x1; s.y1 = y1;
            s.c0 = c0; s.c1 = c1;
            s.kind = 2;
            s.hasI = false;
            s.text = "both codes 0000 -> TRIVIAL ACCEPT: draw what is left";
            steps.push_back(s);
            return true;
        }

        if ((c0 & c1) != 0)
        {
            s.x0 = x0; s.y0 = y0; s.x1 = x1; s.y1 = y1;
            s.c0 = c0; s.c1 = c1;
            s.kind = 3;
            s.hasI = false;
            s.text = csBits(c0) + " AND " + csBits(c1) + " = " +
                     csBits(c0 & c1) + " (not 0000) -> TRIVIAL REJECT";
            steps.push_back(s);
            return false;
        }

        int out = (c0 != 0) ? c0 : c1;
        double x = 0, y = 0;
        string edge;

        // same order every time: TOP, BOTTOM, RIGHT, LEFT
        if (out & 8)
        {
            x = x0 + (x1 - x0) * (w.ymax - y0) / (y1 - y0);
            y = w.ymax;
            edge = "TOP";
        }
        else if (out & 4)
        {
            x = x0 + (x1 - x0) * (w.ymin - y0) / (y1 - y0);
            y = w.ymin;
            edge = "BOTTOM";
        }
        else if (out & 2)
        {
            y = y0 + (y1 - y0) * (w.xmax - x0) / (x1 - x0);
            x = w.xmax;
            edge = "RIGHT";
        }
        else
        {
            y = y0 + (y1 - y0) * (w.xmin - x0) / (x1 - x0);
            x = w.xmin;
            edge = "LEFT";
        }

        string who;

        if (out == c0)
        {
            x0 = x; y0 = y;
            c0 = csCode(x0, y0, w);
            who = n0 + " -> new code " + csBits(c0);
        }
        else
        {
            x1 = x; y1 = y;
            c1 = csCode(x1, y1, w);
            who = n1 + " -> new code " + csBits(c1);
        }

        s.x0 = x0; s.y0 = y0; s.x1 = x1; s.y1 = y1;
        s.c0 = c0; s.c1 = c1;
        s.kind = 1;
        s.hasI = true;
        s.ix = x;
        s.iy = y;
        s.text = "clip at " + edge + " edge: (" + clipNum(x) + ", " +
                 clipNum(y) + ")  " + who;
        steps.push_back(s);
    }

    return false;
}

// ---------------- Sutherland-Hodgman ----------------
vector<ClipPt> shStage[5];       // 0 = original, 1..4 after LEFT, RIGHT, BOTTOM, TOP
vector<string> shLog[5];
int shPos = 0;

const char* shEdgeName[4] = { "LEFT", "RIGHT", "BOTTOM", "TOP" };

bool shInside(const ClipPt& p, int e, const ClipWin& w)
{
    if (e == 0) return p.x >= w.xmin;
    if (e == 1) return p.x <= w.xmax;
    if (e == 2) return p.y >= w.ymin;
    return p.y <= w.ymax;
}

ClipPt shIntersect(const ClipPt& a, const ClipPt& b, int e, const ClipWin& w)
{
    ClipPt r;

    if (e == 0 || e == 1)
    {
        r.x = (e == 0) ? w.xmin : w.xmax;
        r.y = a.y + (b.y - a.y) * (r.x - a.x) / (b.x - a.x);
    }
    else
    {
        r.y = (e == 2) ? w.ymin : w.ymax;
        r.x = a.x + (b.x - a.x) * (r.y - a.y) / (b.y - a.y);
    }

    return r;
}

void shRun(const ClipPt* poly, int n, const ClipWin& w)
{
    for (int s = 0; s < 5; s++)
    {
        shStage[s].clear();
        shLog[s].clear();
    }

    for (int i = 0; i < n; i++)
        shStage[0].push_back(poly[i]);

    shLog[0].push_back("original polygon: " + to_string(n) +
                       " vertices.  Press STEP to clip it edge by edge.");

    for (int e = 0; e < 4; e++)
    {
        const vector<ClipPt>& in = shStage[e];
        vector<ClipPt>& out = shStage[e + 1];
        int m = (int)in.size();

        for (int i = 0; i < m; i++)
        {
            const ClipPt& cur = in[i];
            int pi = (i + m - 1) % m;
            const ClipPt& prev = in[pi];

            string pair = "V" + to_string(pi + 1) + "->V" + to_string(i + 1) + ": ";

            bool curIn = shInside(cur, e, w);
            bool prevIn = shInside(prev, e, w);

            if (curIn)
            {
                if (!prevIn)
                {
                    out.push_back(shIntersect(prev, cur, e, w));
                    shLog[e + 1].push_back(pair + "out -> in   add I and V" + to_string(i + 1));
                }
                else
                {
                    shLog[e + 1].push_back(pair + "in -> in    add V" + to_string(i + 1));
                }

                out.push_back(cur);
            }
            else
            {
                if (prevIn)
                {
                    out.push_back(shIntersect(prev, cur, e, w));
                    shLog[e + 1].push_back(pair + "in -> out   add I");
                }
                else
                {
                    shLog[e + 1].push_back(pair + "out -> out  nothing");
                }
            }
        }

        shLog[e + 1].insert(shLog[e + 1].begin(),
                            string(shEdgeName[e]) + " edge: " + to_string(m) +
                            " vertices in -> " + to_string((int)out.size()) + " out");
    }
}

// ---------------- recompute / progress ----------------
void clipRecompute()
{
    int r = currentFlightIndex();
    clipRoute = r;

    for (int i = 0; i < MAX_FLIGHTS; i++)
    {
        vector<CSStep> tmp;

        csAllVis[i] = csRun(clipHub, clipDest[i], clipWin, tmp, "P0", "P1");

        csAllA[i].x = tmp.back().x0;
        csAllA[i].y = tmp.back().y0;
        csAllB[i].x = tmp.back().x1;
        csAllB[i].y = tmp.back().y1;
    }

    csRun(clipHub, clipDest[r], clipWin, csSteps,
          "P0 (hub)", "P1 (" + destinationNames[r] + ")");
    csPos = 0;

    if (clipZone == 0)
        shRun(clipPolyStorm, 6, clipWin);
    else
        shRun(clipPolyRestricted, 4, clipWin);

    shPos = 0;
}

void clipSync()
{
    if (clipRoute != currentFlightIndex())
        clipRecompute();
}

void clipPageEnter()
{
    clipAuto = false;
    clipRecompute();
}

bool clipFinished()
{
    if (clipTab == 0)
        return csPos >= (int)csSteps.size() - 1;

    return shPos >= 4;
}

void clipStep()
{
    if (clipTab == 0)
    {
        if (csPos < (int)csSteps.size() - 1)
            csPos++;
    }
    else
    {
        if (shPos < 4)
            shPos++;
    }
}

void clipResetProgress()
{
    csPos = 0;
    shPos = 0;
}

void clipSetWindowCenter(double cx, double cy)
{
    double w = clipWin.xmax - clipWin.xmin;
    double h = clipWin.ymax - clipWin.ymin;

    double x0 = cx - w / 2.0;
    double y0 = cy - h / 2.0;

    if (x0 < CLIP_WX0) x0 = CLIP_WX0;
    if (x0 + w > CLIP_WX1) x0 = CLIP_WX1 - w;
    if (y0 < CLIP_WY0) y0 = CLIP_WY0;
    if (y0 + h > CLIP_WY1) y0 = CLIP_WY1 - h;

    clipWin.xmin = x0;
    clipWin.xmax = x0 + w;
    clipWin.ymin = y0;
    clipWin.ymax = y0 + h;

    clipRecompute();
}

void clipMoveWindow(double dx, double dy)
{
    clipSetWindowCenter((clipWin.xmin + clipWin.xmax) / 2.0 + dx,
                        (clipWin.ymin + clipWin.ymax) / 2.0 + dy);
}

void clipScaleWindow(double f)
{
    double cx = (clipWin.xmin + clipWin.xmax) / 2.0;
    double cy = (clipWin.ymin + clipWin.ymax) / 2.0;
    double w = (clipWin.xmax - clipWin.xmin) * f;

    if (w < 4.0) w = 4.0;
    if (w > 28.0) w = 28.0;

    clipWin.xmin = cx - w / 2.0;
    clipWin.xmax = cx + w / 2.0;
    clipWin.ymin = cy - w / 4.0;
    clipWin.ymax = cy + w / 4.0;

    clipSetWindowCenter(cx, cy);
}

// aircraft move hub <-> destination (triangle wave), so they enter
// and leave the clip window while the page is open
void clipAircraft(int i, double& x, double& y)
{
    double ph = fmod(clipTick * 0.003 + i * 0.2, 2.0);
    double t = (ph < 1.0) ? ph : 2.0 - ph;

    x = clipHub.x + (clipDest[i].x - clipHub.x) * t;
    y = clipHub.y + (clipDest[i].y - clipHub.y) * t;
}

void updateClipAnimation()
{
    clipTick++;

    if (clipAuto && dashboardPage == DASH_CLIP && clipTick % 14 == 0)
    {
        clipSync();
        clipStep();

        if (clipFinished())
            clipAuto = false;
    }
}

// ---------------- buttons ----------------
Btn clipTabBtn[2] =
{
    { 320, 590, 640, 630, "LINES: COHEN-SUTHERLAND",       0.08f, 0.30f, 0.60f },
    { 650, 590, 1060, 630, "POLYGON: SUTHERLAND-HODGMAN",  0.08f, 0.30f, 0.60f }
};

Btn clipStepBtn  = { 320, 150, 410, 195, "STEP",        0.08f, 0.42f, 0.78f };
Btn clipAutoBtn  = { 420, 150, 520, 195, "AUTO: OFF",   0.10f, 0.55f, 0.25f };
Btn clipResetBtn = { 530, 150, 620, 195, "RESET",       0.16f, 0.25f, 0.35f };
Btn clipExtraBtn = { 630, 150, 790, 195, "NEXT FLIGHT", 0.60f, 0.35f, 0.15f };

Btn clipLeftBtn   = { 320,  90, 380, 135, "LEFT",    0.16f, 0.25f, 0.35f };
Btn clipRightBtn  = { 390,  90, 450, 135, "RIGHT",   0.16f, 0.25f, 0.35f };
Btn clipUpBtn     = { 460,  90, 520, 135, "UP",      0.16f, 0.25f, 0.35f };
Btn clipDownBtn   = { 530,  90, 590, 135, "DOWN",    0.16f, 0.25f, 0.35f };
Btn clipBigBtn    = { 600,  90, 690, 135, "BIGGER",  0.16f, 0.25f, 0.35f };
Btn clipSmallBtn  = { 700,  90, 790, 135, "SMALLER", 0.16f, 0.25f, 0.35f };

// ---------------- screen mapping ----------------
float clipMapScale()
{
    return fminf(450.0f / 32.0f, 325.0f / 30.0f);
}

float clipMapOX() { return 330.0f + (450.0f - 32.0f * clipMapScale()) / 2.0f; }
float clipMapOY() { return 235.0f + (325.0f - 30.0f * clipMapScale()) / 2.0f; }

float clipMX(double lon) { return clipMapOX() + (float)(lon - CLIP_WX0) * clipMapScale(); }
float clipMY(double lat) { return clipMapOY() + (float)(lat - CLIP_WY0) * clipMapScale(); }

// radar viewport rectangle (pixels)
const float VP_X0 = 885.0f, VP_Y0 = 372.0f, VP_W = 320.0f, VP_H = 160.0f;

float clipVX(double x)
{
    return VP_X0 + (float)((x - clipWin.xmin) * VP_W / (clipWin.xmax - clipWin.xmin));
}

float clipVY(double y)
{
    return VP_Y0 + (float)((y - clipWin.ymin) * VP_H / (clipWin.ymax - clipWin.ymin));
}

void clipHandleClick(float wx, float wy)
{
    clipSync();

    if (inBtn(clipTabBtn[0], wx, wy) && clipTab != 0)
    {
        clipTab = 0;
        clipAuto = false;
        return;
    }

    if (inBtn(clipTabBtn[1], wx, wy) && clipTab != 1)
    {
        clipTab = 1;
        clipAuto = false;
        return;
    }

    if (inBtn(clipStepBtn, wx, wy))
        clipStep();
    else if (inBtn(clipAutoBtn, wx, wy))
        clipAuto = !clipAuto && !clipFinished();
    else if (inBtn(clipResetBtn, wx, wy))
    {
        clipAuto = false;
        clipResetProgress();
    }
    else if (inBtn(clipExtraBtn, wx, wy))
    {
        clipAuto = false;

        if (clipTab == 0)
        {
            nextFlight();
            clipRecompute();
        }
        else
        {
            clipZone = 1 - clipZone;
            clipRecompute();
        }
    }
    else if (inBtn(clipLeftBtn, wx, wy))   clipMoveWindow(-1.0, 0.0);
    else if (inBtn(clipRightBtn, wx, wy))  clipMoveWindow(1.0, 0.0);
    else if (inBtn(clipUpBtn, wx, wy))     clipMoveWindow(0.0, 1.0);
    else if (inBtn(clipDownBtn, wx, wy))   clipMoveWindow(0.0, -1.0);
    else if (inBtn(clipBigBtn, wx, wy))    clipScaleWindow(1.15);
    else if (inBtn(clipSmallBtn, wx, wy))  clipScaleWindow(1.0 / 1.15);
    else if (wx >= 330 && wx <= 780 && wy >= 235 && wy <= 560)
    {
        // click on the map = move the window centre there
        double lon = CLIP_WX0 + (wx - clipMapOX()) / clipMapScale();
        double lat = CLIP_WY0 + (wy - clipMapOY()) / clipMapScale();

        clipSetWindowCenter(lon, lat);
    }
}

// ---------------- drawing helpers ----------------
void clipThickLine(float x1, float y1, float x2, float y2, float w)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = sqrtf(dx * dx + dy * dy);

    if (len < 0.001f)
        return;

    float nx = -dy / len * w * 0.5f;
    float ny = dx / len * w * 0.5f;

    glBegin(GL_QUADS);
    glVertex2f(x1 + nx, y1 + ny);
    glVertex2f(x2 + nx, y2 + ny);
    glVertex2f(x2 - nx, y2 - ny);
    glVertex2f(x1 - nx, y1 - ny);
    glEnd();
}

void clipDashLine(float x1, float y1, float x2, float y2, float w,
                  float dash, float gap)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = sqrtf(dx * dx + dy * dy);

    if (len < 0.001f)
        return;

    float ux = dx / len;
    float uy = dy / len;

    for (float pos = 0.0f; pos < len; pos += dash + gap)
    {
        float e = fminf(pos + dash, len);
        clipThickLine(x1 + ux * pos, y1 + uy * pos,
                      x1 + ux * e,   y1 + uy * e, w);
    }
}

void clipRectOutline(float x1, float y1, float x2, float y2, float t)
{
    drawRect(x1, y1, x2, y1 + t);
    drawRect(x1, y2 - t, x2, y2);
    drawRect(x1, y1, x1 + t, y2);
    drawRect(x2 - t, y1, x2, y2);
}

// convex polygon: triangle fan from vertex 0
void clipFillPoly(const vector<float>& xs, const vector<float>& ys)
{
    if (xs.size() < 3)
        return;

    glBegin(GL_TRIANGLES);

    for (size_t i = 1; i + 1 < xs.size(); i++)
    {
        glVertex2f(xs[0], ys[0]);
        glVertex2f(xs[i], ys[i]);
        glVertex2f(xs[i + 1], ys[i + 1]);
    }

    glEnd();
}

void clipPolyOutline(const vector<float>& xs, const vector<float>& ys, float w)
{
    size_t n = xs.size();

    for (size_t i = 0; i < n; i++)
    {
        size_t j = (i + 1) % n;
        clipThickLine(xs[i], ys[i], xs[j], ys[j], w);
    }
}

void clipStagePixels(const vector<ClipPt>& poly, bool toViewport,
                     vector<float>& xs, vector<float>& ys)
{
    xs.clear();
    ys.clear();

    for (size_t i = 0; i < poly.size(); i++)
    {
        xs.push_back(toViewport ? clipVX(poly[i].x) : clipMX(poly[i].x));
        ys.push_back(toViewport ? clipVY(poly[i].y) : clipMY(poly[i].y));
    }
}

// ---------------- map ----------------
void drawClipMap()
{
    setPanel();
    drawRect(320, 225, 790, 570);

    glColor3f(0.04f, 0.07f, 0.11f);
    drawRect(330, 235, 780, 560);

    // graticule
    glColor3f(0.10f, 0.14f, 0.20f);

    for (int lon = (int)CLIP_WX0; lon <= (int)CLIP_WX1; lon += 4)
        drawRect(clipMX(lon) - 0.5f, clipMY(CLIP_WY0), clipMX(lon) + 0.5f, clipMY(CLIP_WY1));

    for (int lat = (int)CLIP_WY0; lat <= (int)CLIP_WY1; lat += 4)
        drawRect(clipMX(CLIP_WX0), clipMY(lat) - 0.5f, clipMX(CLIP_WX1), clipMY(lat) + 0.5f);

    setGray();
    drawText(335, 545, "AIRSPACE MAP", GLUT_BITMAP_HELVETICA_12);

    const ClipWin& w = clipWin;

    // Cohen-Sutherland: window edges extended over the whole map
    // make the 9 outcode regions visible
    if (clipTab == 0)
    {
        glColor3f(0.30f, 0.34f, 0.42f);

        clipDashLine(clipMX(w.xmin), clipMY(CLIP_WY0), clipMX(w.xmin), clipMY(CLIP_WY1), 1.0f, 6, 5);
        clipDashLine(clipMX(w.xmax), clipMY(CLIP_WY0), clipMX(w.xmax), clipMY(CLIP_WY1), 1.0f, 6, 5);
        clipDashLine(clipMX(CLIP_WX0), clipMY(w.ymin), clipMX(CLIP_WX1), clipMY(w.ymin), 1.0f, 6, 5);
        clipDashLine(clipMX(CLIP_WX0), clipMY(w.ymax), clipMX(CLIP_WX1), clipMY(w.ymax), 1.0f, 6, 5);

        double xc[3] = { (CLIP_WX0 + w.xmin) / 2, (w.xmin + w.xmax) / 2, (w.xmax + CLIP_WX1) / 2 };
        double yc[3] = { (CLIP_WY0 + w.ymin) / 2, (w.ymin + w.ymax) / 2, (w.ymax + CLIP_WY1) / 2 };

        glColor3f(0.40f, 0.45f, 0.55f);

        for (int a = 0; a < 3; a++)
            for (int b = 0; b < 3; b++)
                drawText(clipMX(xc[a]) - 14, clipMY(yc[b]) - 4,
                         csBits(csCode(xc[a], yc[b], w)), GLUT_BITMAP_HELVETICA_12);
    }

    // zone polygon (polygon tab)
    if (clipTab == 1)
    {
        vector<float> xs, ys;

        clipStagePixels(shStage[0], false, xs, ys);
        glColor3f(0.55f, 0.58f, 0.65f);
        clipPolyOutline(xs, ys, 1.5f);

        clipStagePixels(shStage[shPos], false, xs, ys);
        glColor3f(0.40f, 0.28f, 0.06f);
        clipFillPoly(xs, ys);
        glColor3f(1.0f, 0.75f, 0.10f);
        clipPolyOutline(xs, ys, 2.5f);

        for (size_t i = 0; i < xs.size(); i++)
            drawRect(xs[i] - 3, ys[i] - 3, xs[i] + 3, ys[i] + 3);

        // the window edge that was just applied
        if (shPos >= 1)
        {
            int e = shPos - 1;

            glColor3f(0.20f, 0.90f, 1.0f);

            if (e == 0)
                clipThickLine(clipMX(w.xmin), clipMY(CLIP_WY0), clipMX(w.xmin), clipMY(CLIP_WY1), 3);
            else if (e == 1)
                clipThickLine(clipMX(w.xmax), clipMY(CLIP_WY0), clipMX(w.xmax), clipMY(CLIP_WY1), 3);
            else if (e == 2)
                clipThickLine(clipMX(CLIP_WX0), clipMY(w.ymin), clipMX(CLIP_WX1), clipMY(w.ymin), 3);
            else
                clipThickLine(clipMX(CLIP_WX0), clipMY(w.ymax), clipMX(CLIP_WX1), clipMY(w.ymax), 3);
        }
    }

    // routes
    for (int i = 0; i < MAX_FLIGHTS; i++)
    {
        float r = clipRouteCol[i][0], g = clipRouteCol[i][1], b = clipRouteCol[i][2];
        bool selected = (i == clipRoute);

        float hx = clipMX(clipHub.x), hy = clipMY(clipHub.y);
        float dx = clipMX(clipDest[i].x), dy = clipMY(clipDest[i].y);

        // the whole route, dim and dashed
        glColor3f(r * 0.35f, g * 0.35f, b * 0.35f);
        clipDashLine(hx, hy, dx, dy, 1.5f, 5, 5);

        if (clipTab == 0 && selected && !csSteps.empty())
        {
            const CSStep& s = csSteps[csPos];

            if (s.kind != 3)
            {
                if (s.kind == 2)
                    glColor3f(0.30f, 1.0f, 0.40f);
                else
                    glColor3f(1.0f, 0.75f, 0.10f);

                clipThickLine(clipMX(s.x0), clipMY(s.y0), clipMX(s.x1), clipMY(s.y1), 4);
            }

            if (s.hasI)
            {
                glColor3f(1.0f, 1.0f, 1.0f);
                drawFilledBresenhamCircle((int)clipMX(s.ix), (int)clipMY(s.iy), 5);
            }
        }
        else if (csAllVis[i])
        {
            glColor3f(r, g, b);
            clipThickLine(clipMX(csAllA[i].x), clipMY(csAllA[i].y),
                          clipMX(csAllB[i].x), clipMY(csAllB[i].y), 3);
        }
    }

    // clip window
    glColor3f(1.0f, 0.85f, 0.15f);
    clipRectOutline(clipMX(w.xmin), clipMY(w.ymin), clipMX(w.xmax), clipMY(w.ymax), 2);

    // hub and destinations
    glColor3f(1.0f, 1.0f, 1.0f);
    drawFilledBresenhamCircle((int)clipMX(clipHub.x), (int)clipMY(clipHub.y), 5);
    setWhite();
    drawText(clipMX(clipHub.x) + 8, clipMY(clipHub.y) - 14, "HUB", GLUT_BITMAP_HELVETICA_12);

    for (int i = 0; i < MAX_FLIGHTS; i++)
    {
        glColor3f(clipRouteCol[i][0], clipRouteCol[i][1], clipRouteCol[i][2]);
        drawFilledBresenhamCircle((int)clipMX(clipDest[i].x), (int)clipMY(clipDest[i].y), 4);

        drawText(clipMX(clipDest[i].x) + 7, clipMY(clipDest[i].y) + 3,
                 string(flights[i].flightID) + " " + destinationNames[i],
                 GLUT_BITMAP_HELVETICA_12);
    }

    // aircraft
    for (int i = 0; i < MAX_FLIGHTS; i++)
    {
        double ax, ay;
        clipAircraft(i, ax, ay);

        bool inside = (csCode(ax, ay, w) == 0);

        if (inside)
            glColor3f(0.30f, 1.0f, 0.40f);
        else
            glColor3f(0.55f, 0.55f, 0.60f);

        drawFilledBresenhamCircle((int)clipMX(ax), (int)clipMY(ay), 3);
    }
}

// ---------------- radar viewport ----------------
void drawClipRadar()
{
    setPanel();
    drawRect(810, 325, 1270, 575);

    // aircraft currently inside the window
    int visible = 0;

    for (int i = 0; i < MAX_FLIGHTS; i++)
    {
        double ax, ay;
        clipAircraft(i, ax, ay);

        if (csCode(ax, ay, clipWin) == 0)
            visible++;
    }

    setWhite();
    drawText(825, 552, "RADAR VIEWPORT   aircraft in window: " +
                       to_string(visible) + "/" + to_string(MAX_FLIGHTS),
             GLUT_BITMAP_HELVETICA_12);

    // viewport
    glColor3f(0.02f, 0.10f, 0.06f);
    drawRect(VP_X0, VP_Y0, VP_X0 + VP_W, VP_Y0 + VP_H);

    glColor3f(0.05f, 0.20f, 0.12f);

    for (int i = 1; i < 4; i++)
    {
        drawRect(VP_X0 + i * VP_W / 4 - 0.5f, VP_Y0, VP_X0 + i * VP_W / 4 + 0.5f, VP_Y0 + VP_H);
        drawRect(VP_X0, VP_Y0 + i * VP_H / 4 - 0.5f, VP_X0 + VP_W, VP_Y0 + i * VP_H / 4 + 0.5f);
    }

    // rotating sweep line, itself clipped to the viewport with the
    // same Cohen-Sutherland routine
    {
        ClipWin vp = { VP_X0, VP_Y0, VP_X0 + VP_W, VP_Y0 + VP_H };
        double cx = VP_X0 + VP_W / 2.0;
        double cy = VP_Y0 + VP_H / 2.0;

        for (int k = 5; k >= 0; k--)
        {
            double ang = clipTick * 0.07 - k * 0.08;
            ClipPt p0 = { cx, cy };
            ClipPt p1 = { cx + cos(ang) * 400.0, cy + sin(ang) * 400.0 };
            vector<CSStep> st;

            if (csRun(p0, p1, vp, st, "a", "b"))
            {
                float f = (k == 0) ? 0.55f : 0.20f - k * 0.025f;
                glColor3f(0.10f * f * 2, f, 0.15f * f * 2);
                clipThickLine((float)st.back().x0, (float)st.back().y0,
                              (float)st.back().x1, (float)st.back().y1,
                              (k == 0) ? 2.0f : 1.5f);
            }
        }
    }

    // clipped content, mapped window -> viewport
    if (clipTab == 0)
    {
        for (int i = 0; i < MAX_FLIGHTS; i++)
        {
            if (!csAllVis[i])
                continue;

            glColor3f(clipRouteCol[i][0], clipRouteCol[i][1], clipRouteCol[i][2]);
            clipThickLine(clipVX(csAllA[i].x), clipVY(csAllA[i].y),
                          clipVX(csAllB[i].x), clipVY(csAllB[i].y), 3);
        }
    }
    else
    {
        vector<float> xs, ys;

        clipStagePixels(shStage[4], true, xs, ys);

        if (xs.size() >= 3)
        {
            glColor3f(0.40f, 0.28f, 0.06f);
            clipFillPoly(xs, ys);
            glColor3f(1.0f, 0.75f, 0.10f);
            clipPolyOutline(xs, ys, 2.5f);
        }
    }

    // aircraft inside the window
    for (int i = 0; i < MAX_FLIGHTS; i++)
    {
        double ax, ay;
        clipAircraft(i, ax, ay);

        if (csCode(ax, ay, clipWin) != 0)
            continue;

        glColor3f(1.0f, 1.0f, 1.0f);
        drawFilledBresenhamCircle((int)clipVX(ax), (int)clipVY(ay), 3);

        drawText(clipVX(ax) + 6, clipVY(ay) + 4, flights[i].flightID,
                 GLUT_BITMAP_HELVETICA_12);
    }

    glColor3f(0.30f, 0.80f, 0.45f);
    clipRectOutline(VP_X0, VP_Y0, VP_X0 + VP_W, VP_Y0 + VP_H, 2);

    // window -> viewport formulas (live numbers)
    double sx = VP_W / (clipWin.xmax - clipWin.xmin);
    double sy = VP_H / (clipWin.ymax - clipWin.ymin);

    setGray();
    drawText(825, 352,
             "xv = " + to_string((int)VP_X0) + " + (xw - " + clipNum(clipWin.xmin) +
             ") * " + clipNum(sx) + "      yv = " + to_string((int)VP_Y0) +
             " + (yw - " + clipNum(clipWin.ymin) + ") * " + clipNum(sy),
             GLUT_BITMAP_HELVETICA_12);
    drawText(825, 336,
             "window " + clipNum(clipWin.xmax - clipWin.xmin) + " x " +
             clipNum(clipWin.ymax - clipWin.ymin) +
             " world units  ->  viewport 320 x 160 pixels",
             GLUT_BITMAP_HELVETICA_12);
}

// ---------------- log ----------------
void drawClipLog()
{
    setPanel();
    drawRect(810, 70, 1270, 315);

    setWhite();

    if (clipTab == 0)
    {
        drawText(825, 290,
                 string("COHEN-SUTHERLAND  ") + flights[clipRoute].flightID +
                 ": hub -> " + destinationNames[clipRoute],
                 GLUT_BITMAP_HELVETICA_12);

        int first = (csPos + 1 > 8) ? csPos + 1 - 8 : 0;

        for (int i = first; i <= csPos && i < (int)csSteps.size(); i++)
        {
            float y = 262.0f - (i - first) * 24.0f;

            if (i == csPos)
            {
                if (csSteps[i].kind == 2)
                    glColor3f(0.30f, 1.0f, 0.40f);
                else if (csSteps[i].kind == 3)
                    glColor3f(1.0f, 0.45f, 0.45f);
                else
                    glColor3f(1.0f, 0.75f, 0.10f);
            }
            else
            {
                setGray();
            }

            drawText(825, y, to_string(i + 1) + ". " + csSteps[i].text,
                     GLUT_BITMAP_HELVETICA_12);
        }

        setGray();
        drawText(825, 80, "outcode = T B R L   (1 = beyond that window edge)",
                 GLUT_BITMAP_HELVETICA_12);
    }
    else
    {
        drawText(825, 290,
                 string("SUTHERLAND-HODGMAN  ") + clipZoneName[clipZone] +
                 "  (stage " + to_string(shPos) + "/4)",
                 GLUT_BITMAP_HELVETICA_12);

        const vector<string>& lg = shLog[shPos];

        for (size_t i = 0; i < lg.size() && i < 10; i++)
        {
            float y = 262.0f - i * 21.0f;

            if (i == 0)
                glColor3f(1.0f, 0.75f, 0.10f);
            else
                setGray();

            drawText(825, y, lg[i], GLUT_BITMAP_HELVETICA_12);
        }

        setGray();
        drawText(825, 80, "in->in keep V | in->out add I | out->in add I,V | out->out none",
                 GLUT_BITMAP_HELVETICA_12);
    }
}

void drawClipPage()
{
    clipSync();

    setWhite();
    drawText(320, 685, "CG: CLIPPING AND VIEWING", GLUT_BITMAP_TIMES_ROMAN_24);

    setGray();
    drawText(320, 655,
             "AeroDesk flight routes and weather zones clipped by the radar window.");

    drawButton(clipTabBtn[0], clipTab == 0);
    drawButton(clipTabBtn[1], clipTab == 1);

    drawClipMap();
    drawClipRadar();
    drawClipLog();

    // status line + controls
    setGray();

    if (clipTab == 0)
        drawText(320, 205,
                 "Step " + to_string(csPos + 1) + " / " + to_string((int)csSteps.size()) +
                 (clipFinished() ? "  - finished" : ""),
                 GLUT_BITMAP_HELVETICA_12);
    else
        drawText(320, 205,
                 "Edge " + to_string(shPos) + " / 4" +
                 (clipFinished() ? "  - finished (final polygon on the radar)" : ""),
                 GLUT_BITMAP_HELVETICA_12);

    clipAutoBtn.label = clipAuto ? "AUTO: ON" : "AUTO: OFF";
    clipExtraBtn.label = (clipTab == 0)
                             ? "NEXT FLIGHT"
                             : (clipZone == 0 ? "ZONE: STORM" : "ZONE: RESTRICTED");

    drawButton(clipStepBtn, false);
    drawButton(clipAutoBtn, clipAuto);
    drawButton(clipResetBtn, false);
    drawButton(clipExtraBtn, false);

    drawButton(clipLeftBtn, false);
    drawButton(clipRightBtn, false);
    drawButton(clipUpBtn, false);
    drawButton(clipDownBtn, false);
    drawButton(clipBigBtn, false);
    drawButton(clipSmallBtn, false);

    setGray();
    drawText(320, 74, "CLIP WINDOW - or click the map to move it",
             GLUT_BITMAP_HELVETICA_12);
}
// ===== CLIPPING END =====


void displayDashboard()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    setDark();
    drawRect(0, 0, DASH_W, DASH_H);

    drawDashboardHeader();
    drawSidebar();

    switch (dashboardPage)
    {
        case DASH_HOME:
            drawHomePage();
            break;

        case DASH_FLIGHTS:
            drawFlightListPage();
            break;

        case DASH_PASSENGERS:
            drawPassengerListPage();
            break;

        case DASH_SEARCH:
            drawPassengerSearchPage();
            break;

        case DASH_FLIGHT_DETAILS:
            drawFlightDetailsPage();
            break;

        case DASH_PASSENGER_DETAILS:
            drawPassengerDetailsPage();
            break;

        case DASH_QUEUE:
            drawQueuePage();
            break;

        // ===== COA CO4 PAGE START =====
        case DASH_COA:
            drawCOAPage();
            break;
        // ===== COA CO4 PAGE END =====

        // ===== HASHING START =====
        case DASH_HASH:
            drawHashPage();
            break;
        // ===== HASHING END =====

        // ===== OOP DASHBOARD START =====
        case DASH_OOP:
            drawOOPPage();
            break;
        // ===== OOP DASHBOARD END =====

        // ===== SEARCH SORT START =====
        case DASH_SORT:
            drawSortPage();
            break;
        // ===== SEARCH SORT END =====

        // ===== CLIPPING START =====
        case DASH_CLIP:
            drawClipPage();
            break;
        // ===== CLIPPING END =====
    }

    glutSwapBuffers();
}


// =====================================================
// MAIN WINDOW DISPLAY
// =====================================================

void displayMain()
{
    updateOOPFlightObject();

    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // SKY
    // ===== DASHBOARD TEXTURE START =====
    drawSkyGradient();
    // ===== DASHBOARD TEXTURE END =====

    // HEADER
    glColor3f(0.03f, 0.05f, 0.10f);
    drawRect(20, 700, 1280, 785);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(45, 750, "AERODESK AIRPORT", GLUT_BITMAP_TIMES_ROMAN_24);

    drawText(1000, 752, "STATUS: " + getStatusString());

    if (currentNode != NULL)
    {
        string info =
            string("Flight: ") + currentNode->data.flightID +
            "   Speed: " + to_string((int)speedValue) + " km/h" +
            "   Altitude: " + to_string((int)altitudeValue) + " m" +
            "   Fuel: " + to_string((int)fuelValue) + "%" +
            "   Passengers: " + to_string(currentNode->data.passengers) +
            "   Node: " + to_string(getCurrentIndex()) + "/" + to_string(MAX_FLIGHTS);

        drawText(45, 715, info);
    }

    // ===== CAMERA TRANSFORM START =====
    // Everything from here down to the FLIGHT LABEL is drawn inside
    // the camera transform. The Sky and the header/button HUD stay
    // outside it and never move.
    glPushMatrix();
    applyCameraTransform();
    // ===== CAMERA TRANSFORM END =====

    // ===== STAR FIELD START =====
    drawAllStars();
    // ===== STAR FIELD END =====

    // CLOUDS
    drawCloud(250, 640);
    drawCloud(1150, 650);

    // ===== PARALLAX CLOUDS START =====
    drawAllParallaxClouds();
    // ===== PARALLAX CLOUDS END =====

    // ===== BRESENHAM SUN START =====
    // ===== SUN MOON SWITCH START =====
    // Day half of the cycle shows the Sun, night half the Moon.
    if (getDayNightFactor() < 0.5f)
        drawSun();
    else
        drawMoon();
    // ===== SUN MOON SWITCH END =====
    // ===== BRESENHAM SUN END =====

    // ===== FLYING BIRDS START =====
    drawAllBirds();
    // ===== FLYING BIRDS END =====

    // TERMINAL
    // ===== TEXTURE MAPPING USED: facade texture on terminal =====
    drawTexturedTerminal();

    // CONTROL TOWER (base rects)
    glColor3f(0.65f, 0.65f, 0.70f);
    drawRect(1010, 245, 1070, 470);

    glColor3f(0.05f, 0.12f, 0.20f);
    drawRect(975, 470, 1105, 520);

    // ===== DASHBOARD TEXTURE START =====
    drawTerminalTexture();
    // ===== DASHBOARD TEXTURE END =====

    // ROOF
    glColor3f(0.12f, 0.20f, 0.50f);

    glBegin(GL_TRIANGLES);
    glVertex2f(140, 430);
    glVertex2f(550, 510);
    glVertex2f(960, 430);
    glEnd();

    // TERMINAL WINDOWS
    // ===== DAY NIGHT CYCLE START =====
    float terminalWindowColor[3];
    getTerminalWindowColor(terminalWindowColor);
    glColor3f(terminalWindowColor[0], terminalWindowColor[1], terminalWindowColor[2]);
    // ===== DAY NIGHT CYCLE END =====
    drawRect(220, 320, 300, 385);
    drawRect(330, 320, 410, 385);
    drawRect(690, 320, 770, 385);
    drawRect(800, 320, 880, 385);

    // DOOR
    glColor3f(0.05f, 0.10f, 0.20f);
    drawRect(490, 245, 610, 360);

    glColor3f(0.0f, 0.0f, 0.0f);
    drawCenteredText(550, 395, "TERMINAL");

    // ===== AIRPORT BEACON START =====
    drawAirportBeacon();
    // ===== AIRPORT BEACON END =====

    // RUNWAY
    // ===== TEXTURE MAPPING USED: asphalt texture on runway =====
    drawTexturedRunway();

    glColor3f(1.0f, 1.0f, 1.0f);

    for (int x = 60; x < 1250; x += 100)
        drawRect((float)x, 203, (float)x + 50, 211);

    // ===== APRON MARKINGS START =====
    drawApronMarkings();
    // ===== APRON MARKINGS END =====

    // AIRCRAFT
    drawAircraft(aircraftX, aircraftY);

    // ===== ENGINE SMOKE START =====
    drawEngineSmoke();
    // ===== ENGINE SMOKE END =====

    // FLIGHT LABEL
    if (currentNode != NULL)
    {
        glColor3f(0.0f, 0.0f, 0.0f);
        drawText(aircraftX, aircraftY + 80, currentNode->data.flightID);
    }

    // ===== CAMERA TRANSFORM START =====
    // Back to identity - the rest is fixed HUD again.
    glPopMatrix();
    // ===== CAMERA TRANSFORM END =====

    // LATEST OPERATION + QUEUE
    glColor3f(0.0f, 0.0f, 0.0f);

    drawText(40, 140,
             "LATEST OPERATION: " + getLatestOperation() +
             "      QUEUE: " + to_string(queueCount) + "/" + to_string(QUEUE_SIZE) +
             "   NEXT: " + getNextQueuedFlight());

    drawText(40, 112,
             "SPACE Takeoff | L Land | W Wait | P Pause | R Reset | N/B Flight | D Dashboard | O OOP | C Control Word | E Exceptions | UP/DOWN Speed | ESC Exit",
             GLUT_BITMAP_HELVETICA_12);

    // OOP PANEL (toggle with O) - shows base + derived class info
    if (showOOPPanel)
    {
        FlightBase* obj = getCurrentFlightObject();

        glColor3f(0.03f, 0.05f, 0.10f);
        drawRect(40, 165, 420, 570);

        glColor3f(1.0f, 1.0f, 1.0f);

        drawText(55, 535, "OOP: INHERITANCE + POLYMORPHISM");
        drawText(55, 505, "BASE: FlightBase (abstract)");

        if (obj != NULL)
        {
            drawText(55, 475, "OBJECT: " + obj->getFlightID());
            drawText(55, 445, "DERIVED: " + obj->getClassName());
            drawText(55, 410, "Speed: " + to_string(obj->getSpeed()) + " km/h");
            drawText(55, 380, "Altitude: " + to_string(obj->getAltitude()) + " m");
            drawText(55, 350, "Fuel: " + to_string(obj->getFuel()) + "%");
            drawText(55, 320, "Passengers: " + to_string(obj->getPassengers()));
            drawText(55, 285, "TYPE: " + obj->getType());
            drawText(55, 255, obj->getExtraLabel() + ": " + obj->getExtraValue());

            // ===== POLYMORPHISM START =====
            // Same obj (declared FlightBase*) as every line above -
            // this call alone resolves to the correct derived
            // class's calculateFuel() formula at runtime.
            drawText(55, 225,
                     "Fuel Required: " +
                     to_string((int)(obj->calculateFuel() + 0.5f)) + " units");
            // ===== POLYMORPHISM END =====
        }

        glColor3f(0.3f, 1.0f, 0.4f);
        drawText(55, 190, "STATUS: OBJECT ACTIVE");
    }

    // ===== CONTROL WORD START =====
    if (showControlPanel)
        drawControlWordPanel();
    // ===== CONTROL WORD END =====

    // BUTTON PANEL
    glColor3f(0.03f, 0.05f, 0.10f);
    drawRect(20, 10, 1280, 105);

    for (int i = 0; i < MAIN_BTN_COUNT; i++)
        drawButton(mainBtns[i], false);

    glutSwapBuffers();
}


// =====================================================
// OPEN / CLOSE SECONDARY DASHBOARD WINDOW
// =====================================================

void initOrtho(
    double w,
    double h,
    float r,
    float g,
    float b)
{
    glClearColor(r, g, b, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // ===== DASHBOARD TEXTURE START =====
    // Antialiasing for points and lines (smooth Bresenham rays,
    // circle outlines and bird wings). Set once per window.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    // ===== DASHBOARD TEXTURE END =====
}

void dashboardMouse(int button, int state, int x, int y);
void dashboardKeyboard(unsigned char key, int x, int y);
void dashboardSpecial(int key, int x, int y);

// Called by FreeGLUT when the dashboard window is closed
// (for example with the window close button).
void dashboardClosed()
{
    dashboardOpen = false;
    dashboardWindow = 0;
}

void openManagementDashboard()
{
    if (dashboardOpen)
    {
        glutSetWindow(dashboardWindow);
        glutShowWindow();
        glutPopWindow();
        return;
    }

    glutInitWindowSize((int)DASH_W, (int)DASH_H);
    glutInitWindowPosition(90, 50);

    dashboardWindow =
        glutCreateWindow("AeroDesk - Management Dashboard");

    dashboardOpen = true;

    initOrtho(DASH_W, DASH_H, 0.035f, 0.045f, 0.075f);

    glutDisplayFunc(displayDashboard);
    glutKeyboardFunc(dashboardKeyboard);
    glutSpecialFunc(dashboardSpecial);
    glutMouseFunc(dashboardMouse);
    glutCloseFunc(dashboardClosed);

    dashboardPage = DASH_HOME;

    cout << "\n[DASHBOARD] Management Dashboard Opened.\n";

    glutPostRedisplay();
}

void closeManagementDashboard()
{
    if (!dashboardOpen)
        return;

    int id = dashboardWindow;

    dashboardOpen = false;
    dashboardWindow = 0;

    glutDestroyWindow(id);

    if (mainWindow != 0)
    {
        glutSetWindow(mainWindow);
        glutPostRedisplay();
    }

    cout << "\n[DASHBOARD] Management Dashboard Closed.\n";
}


// =====================================================
// SECONDARY DASHBOARD INPUT
// =====================================================

void dashboardMouse(
    int button,
    int state,
    int x,
    int y)
{
    if (button != GLUT_LEFT_BUTTON ||
        state != GLUT_DOWN)
    {
        return;
    }

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    if (w <= 0 || h <= 0)
        return;

    float wx = (float)x * DASH_W / (float)w;
    float wy = (float)(h - y) * DASH_H / (float)h;

    // SIDEBAR
    for (int i = 0; i < 12; i++)
    {
        if (inBtn(dashBtns[i], wx, wy))
        {
            goToPage(i);
            return;
        }
    }

    if (inBtn(dashBtns[12], wx, wy))
    {
        closeManagementDashboard();
        return;
    }

    // FLIGHT LIST
    if (dashboardPage == DASH_FLIGHTS)
    {
        if (wx >= 320 && wx <= 1270 && wy <= 590)
        {
            float offset = 590.0f - wy;
            int row = (int)(offset / 62.0f);

            if (row >= 0 &&
                row < MAX_FLIGHTS &&
                offset - row * 62.0f <= 54.0f)
            {
                // STL: translate the clicked screen row to the real
                // flight index through the sorted order.
                selectFlightByIndex(flightSortOrder[row]);
            }
        }

        if (inBtn(flightsOpenBtn, wx, wy))
            goToPage(DASH_FLIGHT_DETAILS);

        // ===== STL START =====
        if (inBtn(flightsSortBtn, wx, wy))
            cycleFlightSort();
        // ===== STL END =====
    }

    // PASSENGER LIST
    else if (dashboardPage == DASH_PASSENGERS)
    {
        int fi = currentFlightIndex();
        int start = getFlightStartIndex(fi);
        int count = flights[fi].passengers;
        int pages = (count + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;

        if (wx >= 320 && wx <= 1270 && wy <= 590)
        {
            float offset = 590.0f - wy;
            int row = (int)(offset / 46.0f);
            int k = passengerPage * ROWS_PER_PAGE + row;

            if (row >= 0 &&
                row < ROWS_PER_PAGE &&
                k < count &&
                offset - row * 46.0f <= 42.0f)
            {
                selectedPassengerIndex = start + k;
            }
        }

        if (inBtn(passPrevBtn, wx, wy))
        {
            if (passengerPage > 0)
                passengerPage--;
        }
        else if (inBtn(passNextBtn, wx, wy))
        {
            if (passengerPage < pages - 1)
                passengerPage++;
        }
        else if (inBtn(passOpenBtn, wx, wy))
        {
            goToPage(DASH_PASSENGER_DETAILS);
        }
    }

    // PASSENGER SEARCH
    else if (dashboardPage == DASH_SEARCH)
    {
        if (inBtn(searchBtn, wx, wy))
        {
            doPassengerSearch();
        }
        else if (searchResultIndex >= 0 &&
                 inBtn(searchViewBtn, wx, wy))
        {
            selectedPassengerIndex = searchResultIndex;
            goToPage(DASH_PASSENGER_DETAILS);
        }
        // ===== HASHING START =====
        else if (searchResultIndex >= 0 &&
                 hashSearchPassenger(
                     passengerRecords[searchResultIndex].passengerID) < 0 &&
                 inBtn(hashAddSearchBtn, wx, wy))
        {
            hashAddFromRecord(searchResultIndex);
        }
        // ===== HASHING END =====
    }

    // FLIGHT DETAILS
    else if (dashboardPage == DASH_FLIGHT_DETAILS)
    {
        if (inBtn(fdPassBtn, wx, wy))
        {
            passengerPage = 0;
            goToPage(DASH_PASSENGERS);
        }
        else if (inBtn(fdPrevBtn, wx, wy))
        {
            previousFlight();
        }
        else if (inBtn(fdNextBtn, wx, wy))
        {
            nextFlight();
        }
    }

    // WAITING QUEUE
    else if (dashboardPage == DASH_QUEUE)
    {
        if (inBtn(queueAddBtn, wx, wy))
            addCurrentFlightToQueue();
        else if (inBtn(queueDepartBtn, wx, wy))
            departNextFlight();
    }

    // ===== HASHING START =====
    // HASH TABLE PAGE
    else if (dashboardPage == DASH_HASH)
    {
        if (inBtn(hashInsertBtn, wx, wy))
            hashDashInsert();
        else if (inBtn(hashSearchBtn, wx, wy))
            hashDashSearch();
        else if (inBtn(hashDeleteBtn, wx, wy))
            hashDashDelete();
        else if (inBtn(hashDemoBtn, wx, wy))
            hashLoadDemo();
        else if (inBtn(hashClearBtn, wx, wy))
        {
            hashClearTable();
            hashSetMessage("Hash table cleared (all slots Empty).", 0);
        }
    }

    // ===== OOP DASHBOARD START =====
    // OOP CONCEPTS PAGE
    else if (dashboardPage == DASH_OOP)
    {
        oopHandleClick(wx, wy);
    }

    // ===== SEARCH SORT START =====
    // SEARCH & SORT PAGE
    else if (dashboardPage == DASH_SORT)
    {
        sortHandleClick(wx, wy);
    }
    // ===== SEARCH SORT END =====

    // ===== CLIPPING START =====
    // CLIPPING PAGE
    else if (dashboardPage == DASH_CLIP)
    {
        clipHandleClick(wx, wy);
    }
    // ===== CLIPPING END =====
    // ===== OOP DASHBOARD END =====

    // PASSENGER DETAILS (hash box button)
    else if (dashboardPage == DASH_PASSENGER_DETAILS)
    {
        if (selectedPassengerIndex >= 0 &&
            hashSearchPassenger(
                passengerRecords[selectedPassengerIndex].passengerID) < 0 &&
            inBtn(hashAddDetailsBtn, wx, wy))
        {
            hashAddFromRecord(selectedPassengerIndex);
        }
    }
    // ===== HASHING END =====

    redrawAll();
}

void dashboardKeyboard(
    unsigned char key,
    int x,
    int y)
{
    if (key == 27)
    {
        closeManagementDashboard();
        return;
    }

    // Passenger Search page: keyboard types the Passenger ID
    if (dashboardPage == DASH_SEARCH)
    {
        if (key == 13)
        {
            doPassengerSearch();
        }
        else if (key == 8)
        {
            if (!searchInput.empty())
                searchInput.erase(searchInput.length() - 1, 1);
        }
        else if (key >= '0' && key <= '9')
        {
            if (searchInput.length() < 8)
                searchInput += (char)key;
        }

        redrawAll();
        return;
    }

    // ===== CLIPPING START =====
    // Clipping page: ENTER / SPACE = STEP (arrow keys move the window)
    if (dashboardPage == DASH_CLIP && (key == 13 || key == ' '))
    {
        clipSync();
        clipStep();
        redrawAll();
        return;
    }
    // ===== CLIPPING END =====

    // ===== SEARCH SORT START =====
    // Search & Sort page: ENTER / SPACE = STEP; on the binary-search
    // tab digits type the target Passenger ID.
    if (dashboardPage == DASH_SORT &&
        (sortTab == 0 || key == 13 || key == ' '))
    {
        if (key == 13 || key == ' ')
        {
            sortSyncFlight();
            sortStepCurrent();
        }
        else if (key == 8)
        {
            if (!sortInput.empty())
            {
                sortInput.erase(sortInput.length() - 1, 1);
                srchReset();
            }
        }
        else if (key >= '0' && key <= '9')
        {
            if (sortInput.length() < 8)
            {
                sortInput += (char)key;
                srchReset();
            }
        }

        redrawAll();
        return;
    }
    // ===== SEARCH SORT END =====

    // ===== STL EXTRAS START =====
    // OOP page, STL view: digits type the Passenger ID, ENTER compares.
    if (dashboardPage == DASH_OOP && oopView == OOP_STL)
    {
        if (key == 13)
        {
            stlCompareSearch();
        }
        else if (key == 8)
        {
            if (!stlInput.empty())
                stlInput.erase(stlInput.length() - 1, 1);
        }
        else if (key >= '0' && key <= '9')
        {
            if (stlInput.length() < 8)
                stlInput += (char)key;
        }

        redrawAll();
        return;
    }
    // ===== STL EXTRAS END =====

    // ===== HASHING START =====
    // Hash Table page: digits type the Passenger ID,
    // ENTER = insert, S = search, X = delete.
    if (dashboardPage == DASH_HASH)
    {
        if (key == 13)
        {
            hashDashInsert();
        }
        else if (key == 's' || key == 'S')
        {
            hashDashSearch();
        }
        else if (key == 'x' || key == 'X')
        {
            hashDashDelete();
        }
        else if (key == 8)
        {
            if (!hashInput.empty())
                hashInput.erase(hashInput.length() - 1, 1);
        }
        else if (key >= '0' && key <= '9')
        {
            if (hashInput.length() < 8)
                hashInput += (char)key;
        }

        redrawAll();
        return;
    }
    // ===== HASHING END =====

    switch (key)
    {
        case '1': goToPage(DASH_HOME); break;
        case '2': goToPage(DASH_FLIGHTS); break;
        case '3': goToPage(DASH_PASSENGERS); break;
        case '4': goToPage(DASH_SEARCH); break;
        case '5': goToPage(DASH_FLIGHT_DETAILS); break;
        case '6': goToPage(DASH_PASSENGER_DETAILS); break;
        case '7': goToPage(DASH_QUEUE); break;
        case '8': goToPage(DASH_COA); break;
        case '9': goToPage(DASH_HASH); break;   // HASHING
        case '0': goToPage(DASH_OOP); break;    // OOP DASHBOARD
        case '-': goToPage(DASH_SORT); break;   // SEARCH SORT
        case '=': goToPage(DASH_CLIP); break;   // CLIPPING
    }

    redrawAll();
}

void dashboardSpecial(
    int key,
    int x,
    int y)
{
    // ===== CLIPPING START =====
    // Clipping page: arrow keys move the clip window
    if (dashboardPage == DASH_CLIP)
    {
        clipSync();

        if (key == GLUT_KEY_LEFT)       clipMoveWindow(-1.0, 0.0);
        else if (key == GLUT_KEY_RIGHT) clipMoveWindow(1.0, 0.0);
        else if (key == GLUT_KEY_UP)    clipMoveWindow(0.0, 1.0);
        else if (key == GLUT_KEY_DOWN)  clipMoveWindow(0.0, -1.0);

        redrawAll();
        return;
    }
    // ===== CLIPPING END =====

    if (dashboardPage == DASH_PASSENGERS)
    {
        int fi = currentFlightIndex();
        int pages =
            (flights[fi].passengers + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;

        if (key == GLUT_KEY_RIGHT)
        {
            if (passengerPage < pages - 1)
                passengerPage++;
        }
        else if (key == GLUT_KEY_LEFT)
        {
            if (passengerPage > 0)
                passengerPage--;
        }
    }

    redrawAll();
}


// =====================================================
// MAIN WINDOW INPUT
// =====================================================

// ===== EXCEPTION HANDLING START =====
void demonstrateExceptions();   // forward declaration (defined below, near main())
// ===== EXCEPTION HANDLING END =====

void keyboard(
    unsigned char key,
    int x,
    int y)
{
    switch (key)
    {
        case 'd':
        case 'D':
            openManagementDashboard();
            break;

        case 'i':
        case 'I':
            displayFlightList();
            displayStackHistory();
            displayQueue();
            break;

        case 'n':
        case 'N':
            nextFlight();
            break;

        case 'b':
        case 'B':
            previousFlight();
            break;

        case 'q':
        case 'Q':
            displayQueue();
            break;

        case 'o':
        case 'O':
            showOOPPanel = !showOOPPanel;

            cout << "\n[OOP] PANEL "
                 << (showOOPPanel ? "SHOWN" : "HIDDEN")
                 << "\n";

            break;

        case ' ':
            startTakeoff();
            break;

        case 'p':
        case 'P':
            togglePause();
            break;

        case 'l':
        case 'L':
            startLanding();
            break;

        case 'r':
        case 'R':
            resetFlightSimulation();
            break;

        case 'w':
        case 'W':
            toggleWaiting();
            break;

        // ===== EXCEPTION HANDLING START =====
        // ===== CONTROL WORD START =====
        case 'c':
        case 'C':
            showControlPanel = !showControlPanel;

            cout << "\n[CONTROL WORD] PANEL "
                 << (showControlPanel ? "SHOWN" : "HIDDEN")
                 << "\n";

            break;
        // ===== CONTROL WORD END =====

        case 'e':
        case 'E':
            demonstrateExceptions();
            break;

        // ===== HASHING START =====
        case 'h':
        case 'H':
            // Original console hashing menu (type 5 to come back).
            // It edits the same table the Hash Table page draws.
            runHashingModule();
            break;
        // ===== HASHING END =====
        // ===== EXCEPTION HANDLING END =====

        case 27:
            exit(0);
            break;
    }

    redrawAll();
}

void specialKeys(
    int key,
    int x,
    int y)
{
    if (key == GLUT_KEY_UP)
    {
        speedValue += 10;

        if (currentNode != NULL &&
            speedValue > currentNode->data.speed)
        {
            speedValue = (float)currentNode->data.speed;
        }
    }
    else if (key == GLUT_KEY_DOWN)
    {
        speedValue -= 10;

        if (speedValue < 0)
            speedValue = 0;
    }

    redrawAll();
}

void mouse(
    int button,
    int state,
    int x,
    int y)
{
    if (button != GLUT_LEFT_BUTTON ||
        state != GLUT_DOWN)
    {
        return;
    }

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    if (w <= 0 || h <= 0)
        return;

    float wx = (float)x * MAIN_W / (float)w;
    float wy = (float)(h - y) * MAIN_H / (float)h;

    if (inBtn(mainBtns[0], wx, wy))
    {
        cout << "\n[MOUSE] OPEN MANAGEMENT DASHBOARD CLICKED\n";
        openManagementDashboard();
    }
    else if (inBtn(mainBtns[1], wx, wy))
    {
        startTakeoff();
    }
    else if (inBtn(mainBtns[2], wx, wy))
    {
        startLanding();
    }
    else if (inBtn(mainBtns[3], wx, wy))
    {
        togglePause();
    }
    else if (inBtn(mainBtns[4], wx, wy))
    {
        toggleWaiting();
    }
    else if (inBtn(mainBtns[5], wx, wy))
    {
        resetFlightSimulation();
    }
    else if (inBtn(mainBtns[6], wx, wy))
    {
        previousFlight();
    }
    else if (inBtn(mainBtns[7], wx, wy))
    {
        nextFlight();
    }

    redrawAll();
}

// Closing the main window ends the whole program.
void mainClosed()
{
    exit(0);
}


// =====================================================
// UPDATE (SIMULATION TIMER)
// =====================================================

void update(int value)
{
    // ===== BRESENHAM SUN START =====
    // Sun keeps rotating/glowing regardless of pause or flight state.
    updateSunAnimation();
    // ===== BRESENHAM SUN END =====

    // ===== FLYING BIRDS START =====
    updateBirds();
    // ===== FLYING BIRDS END =====

    // ===== PARALLAX CLOUDS START =====
    updateParallaxClouds();
    // ===== PARALLAX CLOUDS END =====

    // ===== DAY NIGHT CYCLE START =====
    updateDayNightCycle();
    // ===== DAY NIGHT CYCLE END =====

    // ===== COA CO4 PAGE START =====
    updateCOAAnimation();
    // ===== COA CO4 PAGE END =====

    // ===== HASHING START =====
    updateHashAnimation();
    // ===== HASHING END =====

    // ===== SEARCH SORT START =====
    updateSortAnimation();
    // ===== SEARCH SORT END =====

    // ===== CLIPPING START =====
    updateClipAnimation();
    // ===== CLIPPING END =====

    // ===== STAR FIELD START =====
    updateStars();
    // ===== STAR FIELD END =====

    // ===== AIRPORT BEACON START =====
    updateBeacon();
    // ===== AIRPORT BEACON END =====

    if (!paused &&
        !waiting &&
        currentNode != NULL)
    {
        float targetAltitude = (float)currentNode->data.altitude;

        // ===== ENGINE SMOKE START =====
        // Existing puffs keep drifting/fading in every phase.
        updateEngineSmoke();
        // ===== ENGINE SMOKE END =====

        // ===== ENGINE TURBINE START =====
        // Blade spin is part of the flight sim (tied to speedValue),
        // so it freezes when paused.
        updateEngineRotation();
        // ===== ENGINE TURBINE END =====

        // ===== CAMERA TRANSFORM START =====
        updateCamera();
        // ===== CAMERA TRANSFORM END =====

        // TAKEOFF
        if (phase == TAKING_OFF)
        {
            if (speedValue < currentNode->data.speed)
                speedValue += 5.0f;

            if (speedValue > currentNode->data.speed)
                speedValue = (float)currentNode->data.speed;

            aircraftX += 1.0f + speedValue * 0.01f;

            altitudeValue += 20.0f;

            if (altitudeValue >= targetAltitude)
            {
                altitudeValue = targetAltitude;

                phase = CRUISING;

                cout << "\nFLIGHT REACHED CRUISING ALTITUDE\n";

                pushOperation(
                    string("CRUISE - ") +
                    currentNode->data.flightID
                );
            }

            aircraftY =
                RUNWAY_Y +
                (altitudeValue / targetAltitude) *
                (MAX_FLIGHT_Y - RUNWAY_Y);

            // ===== ENGINE SMOKE START =====
            // Fresh puffs at the engine nacelle (x + 35, y - 24).
            spawnSmokeBurst(aircraftX + 35.0f, aircraftY - 24.0f);
            // ===== ENGINE SMOKE END =====

            fuelValue -= 0.02f;

            if (fuelValue < 0)
                fuelValue = 0;
        }

        // CRUISING
        else if (phase == CRUISING)
        {
            aircraftX += 1.0f + speedValue * 0.015f;

            if (aircraftX > RIGHT_EDGE)
                aircraftX = LEFT_EDGE;

            fuelValue -= 0.01f;

            if (fuelValue < 0)
                fuelValue = 0;
        }

        // LANDING
        else if (phase == LANDING)
        {
            if (speedValue > 80)
                speedValue -= 2.0f;

            altitudeValue -= 30.0f;

            if (altitudeValue > 0)
            {
                aircraftY =
                    RUNWAY_Y +
                    (altitudeValue / targetAltitude) *
                    (MAX_FLIGHT_Y - RUNWAY_Y);
            }
            else
            {
                aircraftY = RUNWAY_Y;

                altitudeValue = 0;
                speedValue = 0;

                phase = ON_GROUND;

                cout << "\nLANDING COMPLETED\n";

                pushOperation(
                    string("LANDED - ") +
                    currentNode->data.flightID
                );
            }

            aircraftX += 1.0f;

            if (aircraftX > RIGHT_EDGE)
                aircraftX = LEFT_EDGE;
        }
    }

    redrawAll();

    glutTimerFunc(40, update, 0);
}


// =====================================================
// OOP TESTING FUNCTION (INHERITANCE + POLYMORPHISM)
// =====================================================

void testConstructorAndEncapsulation()
{
    cout << "\n============================================\n";
    cout << "     AERODESK INHERITANCE TESTING\n";
    cout << "============================================\n";

    PassengerFlight pf("TEST-P1", 300, 5000, 90, 150, 180);
    CargoFlight     cf("TEST-C1", 250, 4000, 80, 5, 60000);
    PrivateJet      pj("TEST-J1", 400, 8000, 95, 8, 10, "Test Owner");

    // Base class pointer array -> polymorphism
    FlightBase* list[3] = { &pf, &cf, &pj };

    for (int i = 0; i < 3; i++)
    {
        cout << "\n" << list[i]->getFlightID()
             << " | Type: " << list[i]->getType()
             << " | " << list[i]->getExtraLabel()
             << ": " << list[i]->getExtraValue() << endl;
    }

    // ===== POLYMORPHISM START =====
    // Same array, same FlightBase* type for every element, but
    // calculateFuel() and display() each run a different formula/
    // output depending on the ACTUAL object behind the pointer -
    // PassengerFlight's load-based formula, CargoFlight's
    // weight-based formula, PrivateJet's lighter formula.
    cout << "\n--- calculateFuel() / display() through FlightBase* ---\n";

    for (int i = 0; i < 3; i++)
        list[i]->display();
    // ===== POLYMORPHISM END =====

    // Encapsulation validation (base class setter on derived object)
    pf.setFuel(150);
    cout << "\nSet Fuel to 150 -> Actual: " << pf.getFuel() << "%\n";

    cf.setSpeed(-100);
    cout << "Set Speed to -100 -> Actual: " << cf.getSpeed() << " km/h\n";

    cout << "\n============================================\n";
    cout << "     INHERITANCE TESTING COMPLETED\n";
    cout << "============================================\n";
}


// ===== EXCEPTION HANDLING START =====
// =====================================================
// "DEMONSTRATE EXCEPTION" - triggered by the 'E' key
// Deliberately triggers each exception type used in the project
// and catches it right there, printing what was thrown and what
// was caught - a quick, self-contained viva demo of every
// try/catch path added above. Nothing here permanently disturbs
// the real program state: the queue overflow demo cleans up after
// itself, and the underflow demo only runs live if the queue is
// genuinely empty right now (otherwise it would dequeue a real
// waiting flight, so it just explains that instead).
// =====================================================

void demonstrateExceptions()
{
    cout << "\n============================================\n";
    cout << "     AERODESK EXCEPTION HANDLING DEMO\n";
    cout << "============================================\n";

    // [1] Invalid Passenger Search ID (bad format)
    try
    {
        string badInput = "AB12";

        for (size_t i = 0; i < badInput.length(); i++)
        {
            if (!isdigit((unsigned char)badInput[i]))
                throw InvalidPassengerIDException(
                    "Invalid Passenger ID format: \"" + badInput + "\" (digits only).");
        }

        cout << "[1] No exception thrown (unexpected).\n";
    }
    catch (const invalid_argument& e)
    {
        cout << "[1] Caught invalid_argument: " << e.what() << endl;
    }

    // [2] Passenger Search ID that doesn't exist
    try
    {
        int fakeID = 999999;

        if (findPassengerIndexByID(fakeID) < 0)
            throw PassengerNotFoundException(fakeID);   // AeroException.h
    }
    catch (const out_of_range& e)
    {
        cout << "[2] Caught out_of_range: " << e.what() << endl;
    }

    // [3] Constructor validation - negative speed
    try
    {
        PassengerFlight bad("DEMO-NEG", -50, 1000, 90, 100, 150);
    }
    catch (const invalid_argument& e)
    {
        cout << "[3] Caught invalid_argument: " << e.what() << endl;
    }

    // [4] Constructor validation - fuel > 100
    try
    {
        PassengerFlight bad("DEMO-FUEL", 250, 1000, 150, 100, 150);
    }
    catch (const invalid_argument& e)
    {
        cout << "[4] Caught invalid_argument: " << e.what() << endl;
    }

    // [5] Constructor validation - passengers > capacity
    try
    {
        PassengerFlight bad("DEMO-CAP", 250, 1000, 90, 200, 150);
    }
    catch (const invalid_argument& e)
    {
        cout << "[5] Caught invalid_argument: " << e.what() << endl;
    }

    // [6] Queue Overflow - uses the REAL enqueueFlight(), filling
    // it with temporary "DEMOQ..." entries until it throws, then
    // removing exactly the ones this demo added.
    {
        int added = 0;

        try
        {
            while (added <= QUEUE_SIZE)
            {
                enqueueFlight("DEMOQ" + to_string(added));
                added++;
            }
        }
        catch (const runtime_error& e)
        {
            cout << "[6] Caught runtime_error: " << e.what() << endl;
        }

        for (int i = 0; i < added; i++)
            removeFlightFromQueue("DEMOQ" + to_string(i));
    }

    // [7] Queue Underflow - only demonstrated live if the queue is
    // genuinely empty right now, so a real waiting flight is never
    // removed just for this demo.
    try
    {
        if (queueCount == 0)
        {
            dequeueFlight();
            cout << "[7] No exception thrown (unexpected).\n";
        }
        else
        {
            cout << "[7] Queue is not empty right now - skipping the "
                 << "live underflow demo (it would remove a real "
                 << "waiting flight). The same throw runtime_error(...) "
                 << "path fires the moment the real queue empties out.\n";
        }
    }
    catch (const runtime_error& e)
    {
        cout << "[7] Caught runtime_error: " << e.what() << endl;
    }

    cout << "============================================\n";
    cout << "     EXCEPTION DEMO COMPLETE\n";
    cout << "============================================\n";
}
// ===== EXCEPTION HANDLING END =====


// =====================================================
// MAIN
// =====================================================

int main(
    int argc,
    char** argv)
{
    // Create derived-class objects FIRST: buildFlightList() calls
    // updateOOPFlightObject(), which needs flightObjects[] ready.
    initFlightObjects();

    // ===== STL START =====
    buildFlightVector();
    applyFlightSort();
    // ===== STL END =====

    // Build linked list
    buildFlightList();

    // Build complete passenger database
    initializePassengerRecords();

    // ===== STL START =====
    buildPassengerSTL();
    // ===== STL END =====

    // ===== HASHING START =====
    hashLoadDemo();
    // ===== HASHING END =====

    // ===== STL EXTRAS START =====
    buildSTLExtras();
    // ===== STL EXTRAS END =====

    // ===== FLYING BIRDS START =====
    initializeBirds();
    // ===== FLYING BIRDS END =====

    // ===== PARALLAX CLOUDS START =====
    initParallaxClouds();
    // ===== PARALLAX CLOUDS END =====

    // ===== STAR FIELD START =====
    initStars();
    // ===== STAR FIELD END =====

    // ===== DASHBOARD TEXTURE START =====
    initRunwayTexture();
    // ===== DASHBOARD TEXTURE END =====

    // ===== ENGINE SMOKE START =====
    initEngineSmoke();
    // ===== ENGINE SMOKE END =====

    displayFlightList();

    // Current flight
    currentNode = head;

    updateOOPFlightObject();

    // Reset simulation
    resetFlightSimulation();

    // Run OOP testing (console)
    testConstructorAndEncapsulation();

    // GLUT initialization
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // Closing the secondary dashboard must not end the program.
    glutSetOption(
        GLUT_ACTION_ON_WINDOW_CLOSE,
        GLUT_ACTION_CONTINUE_EXECUTION
    );

    glutInitWindowSize((int)MAIN_W, (int)MAIN_H);
    glutInitWindowPosition(40, 20);

    mainWindow =
        glutCreateWindow("AeroDesk - Main Dashboard");

    initOrtho(MAIN_W, MAIN_H, 0.35f, 0.68f, 0.90f);

    // ===== TEXTURE MAPPING: create textures (main window context) =====
    initTextures();

    glutDisplayFunc(displayMain);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutCloseFunc(mainClosed);

    glutTimerFunc(40, update, 0);

    cout << "\n========================================\n";
    cout << "       AERODESK SYSTEM STARTED\n";
    cout << "========================================\n";
    cout << "Flights              : " << MAX_FLIGHTS << "\n";
    cout << "Passenger Records    : " << passengerRecordCount << "\n";
    cout << "\nMain Window Keys:\n";
    cout << "D = Open Management Dashboard\n";
    cout << "SPACE = Takeoff   L = Land   W = Wait   P = Pause\n";
    cout << "R = Reset   N/B = Next/Previous Flight\n";
    cout << "O = OOP Panel   I = Print Lists   Q = Print Queue\n";
    cout << "C = Control Word Panel (COA CO4)\n";
    cout << "E = Demonstrate Exception Handling\n";
    cout << "UP/DOWN = Speed   ESC = Exit\n";
    cout << "========================================\n";

    glutMainLoop();

    return 0;
}