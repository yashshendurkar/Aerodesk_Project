#include <iostream>
#include <string>
using namespace std;

struct Flight
{
    string flightID;
    string flightNumber;
    string destination;
    int passengers;
    Flight* next;
};

// Add a new flight
void addFlight(Flight*& head)
{
    Flight* newFlight = new Flight;

    cout << "\nEnter Flight ID: ";
    cin >> newFlight->flightID;

    cout << "Enter Flight Number: ";
    cin >> newFlight->flightNumber;

    cout << "Enter Destination: ";
    cin >> newFlight->destination;

    cout << "Enter Number of Passengers: ";
    cin >> newFlight->passengers;

    // Handle invalid passenger input
    if (cin.fail())
    {
        cin.clear();
        cin.ignore(1000, '\n');

        cout << "\nInvalid passenger number!\n";
        delete newFlight;
        return;
    }

    newFlight->next = nullptr;

    // First node
    if (head == nullptr)
    {
        head = newFlight;
    }
    else
    {
        Flight* temp = head;

        // Go to last node
        while (temp->next != nullptr)
        {
            temp = temp->next;
        }

        temp->next = newFlight;
    }

    cout << "\nFlight added successfully!\n";
}

// Display all flights
void displayFlights(Flight* head)
{
    if (head == nullptr)
    {
        cout << "\nNo flights available.\n";
        return;
    }

    Flight* temp = head;

    cout << "\n====================================\n";
    cout << "        AERODESK FLIGHT LIST\n";
    cout << "====================================\n";

    while (temp != nullptr)
    {
        cout << "\nFlight ID    : " << temp->flightID;
        cout << "\nFlight No    : " << temp->flightNumber;
        cout << "\nDestination  : " << temp->destination;
        cout << "\nPassengers   : " << temp->passengers;
        cout << "\n-----------------------------";

        temp = temp->next;
    }

    cout << endl;
}

// Search flight
void searchFlight(Flight* head)
{
    string id;

    cout << "\nEnter Flight ID to search: ";
    cin >> id;

    Flight* temp = head;

    while (temp != nullptr)
    {
        if (temp->flightID == id)
        {
            cout << "\nFlight Found!\n";
            cout << "Flight ID   : " << temp->flightID << endl;
            cout << "Flight No   : " << temp->flightNumber << endl;
            cout << "Destination : " << temp->destination << endl;
            cout << "Passengers  : " << temp->passengers << endl;

            return;
        }

        temp = temp->next;
    }

    cout << "\nFlight not found.\n";
}

// Delete flight
void deleteFlight(Flight*& head)
{
    string id;

    cout << "\nEnter Flight ID to delete: ";
    cin >> id;

    if (head == nullptr)
    {
        cout << "\nNo flights available.\n";
        return;
    }

    // Delete first node
    if (head->flightID == id)
    {
        Flight* temp = head;

        head = head->next;

        delete temp;

        cout << "\nFlight deleted successfully!\n";
        return;
    }

    // Delete middle or last node
    Flight* temp = head;

    while (temp->next != nullptr)
    {
        if (temp->next->flightID == id)
        {
            Flight* deleteNode = temp->next;

            temp->next = temp->next->next;

            delete deleteNode;

            cout << "\nFlight deleted successfully!\n";
            return;
        }

        temp = temp->next;
    }

    cout << "\nFlight not found.\n";
}

// PL CO2 Module
void runPLCO2Module()
{
    Flight* head = nullptr;

    int choice;

    do
    {
        cout << "\n====================================\n";
        cout << "       AERODESK PL - CO2\n";
        cout << "      SINGLY LINKED LIST\n";
        cout << "====================================\n";

        cout << "1. Add Flight\n";
        cout << "2. Display Flights\n";
        cout << "3. Search Flight\n";
        cout << "4. Delete Flight\n";
        cout << "5. Exit\n";

        cout << "\nEnter your choice: ";
        cin >> choice;

        // Handle invalid menu input
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');

            cout << "\nInvalid input! Please enter a number from 1 to 5.\n";
            continue;
        }

        switch (choice)
        {
            case 1:
                addFlight(head);
                break;

            case 2:
                displayFlights(head);
                break;

            case 3:
                searchFlight(head);
                break;

            case 4:
                deleteFlight(head);
                break;

            case 5:
                cout << "\nExiting CO2 Module...\n";
                break;

            default:
                cout << "\nInvalid choice!\n";
        }

    } while (choice != 5);

    // Free remaining linked-list nodes
    while (head != nullptr)
    {
        Flight* temp = head;

        head = head->next;

        delete temp;
    }
}