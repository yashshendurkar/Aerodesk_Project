
#include <iostream>
#include <string>
using namespace std;

// OOP CO1: Class and Object

class Passenger
{
private:
    int passengerID;
    string passengerName;
    string flightNumber;

public:

    // Constructor
    Passenger(int id, string name, string flight)
    {
        passengerID = id;
        passengerName = name;
        flightNumber = flight;
    }

    // Display passenger details
    void display()
    {
        cout << "\n-----------------------------\n";
        cout << "Passenger ID   : " << passengerID << endl;
        cout << "Passenger Name : " << passengerName << endl;
        cout << "Flight Number  : " << flightNumber << endl;
        cout << "-----------------------------\n";
    }
};

void runOOPModule()
{
    cout << "====================================\n";
    cout << "       AERODESK OOP MODULE\n";
    cout << "====================================\n";

    // Creating objects
    Passenger p1(101, "Rahul", "AI202");
    Passenger p2(102, "Amit", "6E305");
    Passenger p3(103, "Riya", "UK410");

    // Displaying passenger information
    p1.display();
    p2.display();
    p3.display();

    
}

