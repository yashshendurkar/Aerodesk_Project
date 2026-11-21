// #include <iostream>
// #include <string>
// using namespace std;

// class Flight
// {
// private:
//     string flightNumber;
//     string destination;
//     int passengers;

// public:

//     // Constructor
//     Flight(string fNo, string dest, int pass)
//     {
//         flightNumber = fNo;
//         destination = dest;
//         passengers = pass;
//     }

//     // Setter functions
//     void setDestination(string dest)
//     {
//         destination = dest;
//     }

//     void setPassengers(int pass)
//     {
//         passengers = pass;
//     }

//     // Getter functions
//     string getFlightNumber()
//     {
//         return flightNumber;
//     }

//     string getDestination()
//     {
//         return destination;
//     }

//     int getPassengers()
//     {
//         return passengers;
//     }

//     // Display function
//     void display()
//     {
//         cout << "\n====================================\n";
//         cout << "        AERODESK FLIGHT\n";
//         cout << "====================================\n";

//         cout << "Flight Number : " << flightNumber << endl;
//         cout << "Destination   : " << destination << endl;
//         cout << "Passengers    : " << passengers << endl;
//     }
// };

// // OOP CO2 Module
// void runOOPCO2Module()
// {
//     string flightNumber;
//     string destination;
//     int passengers;

//     cout << "\n====================================\n";
//     cout << "       AERODESK OOP - CO2\n";
//     cout << "   CONSTRUCTOR + ENCAPSULATION\n";
//     cout << "====================================\n";

//     cout << "\nEnter Flight Number: ";
//     cin >> flightNumber;

//     cout << "Enter Destination: ";
//     cin >> destination;

//     cout << "Enter Number of Passengers: ";
//     cin >> passengers;

//     // Object creation using constructor
//     Flight flight1(flightNumber, destination, passengers);

//     cout << "\nFlight created using Constructor!\n";

//     flight1.display();

//     // Update using setter
//     cout << "\nUpdating flight information...\n";

//     cout << "Enter New Destination: ";
//     cin >> destination;

//     cout << "Enter New Number of Passengers: ";
//     cin >> passengers;

//     flight1.setDestination(destination);
//     flight1.setPassengers(passengers);

//     cout << "\nAfter Update:\n";

//     flight1.display();
// }
// #ifndef AERODESK_INTEGRATION

// int main()
// {
//     runOOPCO2Module();
//     return 0;
// }

// #endif


#include <iostream>
#include <string>
#include <stdexcept>

using namespace std;

// =====================================================
// BASE CLASS - FLIGHT
// =====================================================
class Flight
{
private:
    string flightNumber;
    string destination;
    int passengers;

public:

    // Constructor
    Flight(string fNo, string dest, int pass)
    {
        if (pass < 0)
        {
            throw invalid_argument("Passengers cannot be negative!");
        }

        flightNumber = fNo;
        destination = dest;
        passengers = pass;
    }

    // =================================================
    // ENCAPSULATION - SETTERS
    // =================================================
    void setDestination(string dest)
    {
        if (dest.empty())
        {
            throw invalid_argument("Destination cannot be empty!");
        }

        destination = dest;
    }

    void setPassengers(int pass)
    {
        if (pass < 0)
        {
            throw invalid_argument("Passengers cannot be negative!");
        }

        passengers = pass;
    }

    // =================================================
    // ENCAPSULATION - GETTERS
    // =================================================
    string getFlightNumber()
    {
        return flightNumber;
    }

    string getDestination()
    {
        return destination;
    }

    int getPassengers()
    {
        return passengers;
    }

    // =================================================
    // FUNCTION OVERLOADING
    // =================================================
    void display()
    {
        cout << "\n====================================\n";
        cout << "          AERODESK FLIGHT\n";
        cout << "====================================\n";

        cout << "Flight Number : " << flightNumber << endl;
        cout << "Destination   : " << destination << endl;
        cout << "Passengers    : " << passengers << endl;
    }

    void display(string title)
    {
        cout << "\n====================================\n";
        cout << "          " << title << endl;
        cout << "====================================\n";

        cout << "Flight Number : " << flightNumber << endl;
        cout << "Destination   : " << destination << endl;
        cout << "Passengers    : " << passengers << endl;
    }

    // =================================================
    // VIRTUAL FUNCTION
    // =================================================
    virtual void flightInfo()
    {
        cout << "\nFlight Information\n";
        cout << "Flight Number : " << flightNumber << endl;
        cout << "Destination   : " << destination << endl;
    }

    // Virtual Destructor
    virtual ~Flight()
    {
    }
};


// =====================================================
// DOMESTIC FLIGHT
// INHERITANCE + FUNCTION OVERRIDING
// =====================================================
class DomesticFlight : public Flight
{
private:
    string airline;

public:

    DomesticFlight(
        string fNo,
        string dest,
        int pass,
        string air
    )
        : Flight(fNo, dest, pass)
    {
        airline = air;
    }

    // Function Overriding
    void flightInfo() override
    {
        cout << "\n====================================\n";
        cout << "       DOMESTIC FLIGHT\n";
        cout << "====================================\n";

        cout << "Flight Number : " << getFlightNumber() << endl;
        cout << "Destination   : " << getDestination() << endl;
        cout << "Passengers    : " << getPassengers() << endl;
        cout << "Airline       : " << airline << endl;
    }
};


// =====================================================
// ABSTRACT CLASS
// PURE VIRTUAL FUNCTION
// =====================================================
class AirportService
{
public:

    // Pure Virtual Function
    virtual void serviceInfo() = 0;

    // Virtual Destructor
    virtual ~AirportService()
    {
    }
};


// =====================================================
// DERIVED CLASS FROM ABSTRACT CLASS
// =====================================================
class BoardingService : public AirportService
{
public:

    // Implementation of Pure Virtual Function
    void serviceInfo() override
    {
        cout << "\n====================================\n";
        cout << "        BOARDING SERVICE\n";
        cout << "====================================\n";

        cout << "Passenger boarding service is active.\n";
    }
};


// =====================================================
// ANOTHER DERIVED CLASS
// =====================================================
class BaggageService : public AirportService
{
public:

    // Implementation of Pure Virtual Function
    void serviceInfo() override
    {
        cout << "\n====================================\n";
        cout << "        BAGGAGE SERVICE\n";
        cout << "====================================\n";

        cout << "Baggage handling service is active.\n";
    }
};


// =====================================================
// OOP CO2 MODULE
// =====================================================
void runOOPCO2Module()
{
    string flightNumber;
    string destination;
    int passengers;

    cout << "\n====================================\n";
    cout << "       AERODESK OOP - CO2\n";
    cout << "====================================\n";

    try
    {
        // =================================================
        // USER INPUT
        // =================================================
        cout << "\nEnter Flight Number: ";
        cin >> flightNumber;

        cout << "Enter Destination: ";
        cin >> destination;

        cout << "Enter Number of Passengers: ";
        cin >> passengers;


        // =================================================
        // CLASS + OBJECT + CONSTRUCTOR
        // =================================================
        Flight flight1(
            flightNumber,
            destination,
            passengers
        );

        cout << "\nFlight created successfully!\n";


        // =================================================
        // FUNCTION OVERLOADING
        // =================================================
        cout << "\n--- Function Overloading ---\n";

        flight1.display();

        flight1.display("FLIGHT DETAILS");


        // =================================================
        // ENCAPSULATION
        // =================================================
        cout << "\n--- Encapsulation ---\n";

        cout << "Enter New Destination: ";
        cin >> destination;

        cout << "Enter New Number of Passengers: ";
        cin >> passengers;

        flight1.setDestination(destination);
        flight1.setPassengers(passengers);

        cout << "\nUpdated Flight:\n";

        flight1.display();


        // =================================================
        // INHERITANCE
        // =================================================
        cout << "\n--- Inheritance ---\n";

        DomesticFlight domestic(
            "AI101",
            "Mumbai",
            120,
            "Air India"
        );

        domestic.flightInfo();


        // =================================================
        // RUNTIME POLYMORPHISM
        // =================================================
        cout << "\n--- Runtime Polymorphism ---\n";

        Flight* flightPtr = &domestic;

        flightPtr->flightInfo();


        // =================================================
        // ABSTRACT CLASS
        // =================================================
        cout << "\n--- Abstract Class + Pure Virtual Function ---\n";

        BoardingService boarding;
        BaggageService baggage;


        // Base class pointers
        AirportService* service1 = &boarding;
        AirportService* service2 = &baggage;


        // Runtime Polymorphism
        service1->serviceInfo();
        service2->serviceInfo();


        cout << "\n====================================\n";
        cout << "       OOP MODULE COMPLETED\n";
        cout << "====================================\n";
    }

    // =================================================
    // EXCEPTION HANDLING
    // =================================================
    catch (const invalid_argument& e)
    {
        cout << "\n====================================\n";
        cout << "             ERROR\n";
        cout << "====================================\n";

        cout << e.what() << endl;
    }

    catch (...)
    {
        cout << "\nUnknown error occurred!\n";
    }
}


// =====================================================
// STANDALONE MAIN
// =====================================================
#ifndef AERODESK_INTEGRATION

int main()
{
    runOOPCO2Module();

    return 0;
}

#endif