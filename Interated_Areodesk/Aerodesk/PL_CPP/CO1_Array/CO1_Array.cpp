#include <iostream>
using namespace std;

void runPLModule()
{
    const int MAX = 5;

    int passengerID[MAX];
    string passengerName[MAX];
    string flightNumber[MAX];

    int count = 0;
    int choice;

    do
    {
        cout << "\n====================================\n";
        cout << "     AERODESK PASSENGER MANAGEMENT\n";
        cout << "====================================\n";
        cout << "1. Add Passenger\n";
        cout << "2. Display Passengers\n";
        cout << "3. Search Passenger\n";
        cout << "4. Update Passenger\n";
        cout << "5. Delete Passenger\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        // 1. Add Passenger
        if(choice == 1)
        {
            if(count == MAX)
            {
                cout << "Passenger storage is full!\n";
            }
            else
            {
                cout << "\nEnter Passenger ID: ";
                cin >> passengerID[count];

                cout << "Enter Passenger Name: ";
                cin >> passengerName[count];

                cout << "Enter Flight Number: ";
                cin >> flightNumber[count];

                count++;

                cout << "Passenger added successfully!\n";
            }
        }

        // 2. Display Passengers
        else if(choice == 2)
        {
            if(count == 0)
            {
                cout << "No passenger records available.\n";
            }
            else
            {
                cout << "\n----- Passenger Records -----\n";

                for(int i = 0; i < count; i++)
                {
                    cout << "\nPassenger " << i + 1 << endl;
                    cout << "ID: " << passengerID[i] << endl;
                    cout << "Name: " << passengerName[i] << endl;
                    cout << "Flight: " << flightNumber[i] << endl;
                }
            }
        }

        // 3. Search Passenger
        else if(choice == 3)
        {
            int searchID;
            bool found = false;

            cout << "Enter Passenger ID to search: ";
            cin >> searchID;

            for(int i = 0; i < count; i++)
            {
                if(passengerID[i] == searchID)
                {
                    cout << "\nPassenger Found!\n";
                    cout << "Name: " << passengerName[i] << endl;
                    cout << "Flight: " << flightNumber[i] << endl;

                    found = true;
                    break;
                }
            }

            if(found == false)
            {
                cout << "Passenger not found.\n";
            }
        }

        // 4. Update Passenger
        else if(choice == 4)
        {
            int updateID;
            bool found = false;

            cout << "Enter Passenger ID to update: ";
            cin >> updateID;

            for(int i = 0; i < count; i++)
            {
                if(passengerID[i] == updateID)
                {
                    cout << "Enter New Name: ";
                    cin >> passengerName[i];

                    cout << "Enter New Flight Number: ";
                    cin >> flightNumber[i];

                    cout << "Passenger updated successfully!\n";

                    found = true;
                    break;
                }
            }

            if(found == false)
            {
                cout << "Passenger not found.\n";
            }
        }

        // 5. Delete Passenger
        else if(choice == 5)
        {
            int deleteID;
            bool found = false;

            cout << "Enter Passenger ID to delete: ";
            cin >> deleteID;

            for(int i = 0; i < count; i++)
            {
                if(passengerID[i] == deleteID)
                {
                    for(int j = i; j < count - 1; j++)
                    {
                        passengerID[j] = passengerID[j + 1];
                        passengerName[j] = passengerName[j + 1];
                        flightNumber[j] = flightNumber[j + 1];
                    }

                    count--;

                    cout << "Passenger deleted successfully!\n";

                    found = true;
                    break;
                }
            }

            if(found == false)
            {
                cout << "Passenger not found.\n";
            }
        }

        else if(choice == 6)
        {
            cout << "Exiting AeroDesk...\n";
        }

        else
        {
            cout << "Invalid choice!\n";
        }

    } while(choice != 6);

    
}
#ifndef AERODESK_INTEGRATION
int main()
{
    runPLModule();
    return 0;
}
#endif