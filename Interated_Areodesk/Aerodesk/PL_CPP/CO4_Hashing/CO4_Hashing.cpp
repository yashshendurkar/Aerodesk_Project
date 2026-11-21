
#include <iostream>
#include <string>
using namespace std;

const int TABLE_SIZE = 10;

// Passenger record
struct Passenger {
    int passengerID;
    string passengerName;
};

// Hash table status:
// 0 = Empty
// 1 = Occupied
// 2 = Deleted
Passenger hashTable[TABLE_SIZE];
int status[TABLE_SIZE] = {0};

// Hash function using Passenger ID
int hashFunction(int id) {
    int index = id % TABLE_SIZE;
    return (index < 0) ? index + TABLE_SIZE : index;
}

// Search passenger using Linear Probing
int searchPassenger(int id) {
    int start = hashFunction(id);

    for (int i = 0; i < TABLE_SIZE; i++) {
        int index = (start + i) % TABLE_SIZE;

        // An empty slot means the ID is not present
        if (status[index] == 0) {
            return -1;
        }

        if (status[index] == 1 &&
            hashTable[index].passengerID == id) {
            return index;
        }
    }

    return -1;
}

// Insert passenger into hash table
void insertPassenger(int id, const string& name) {
    if (searchPassenger(id) != -1) {
        cout << "Passenger ID already exists!\n";
        return;
    }

    int start = hashFunction(id);
    int deletedIndex = -1;

    for (int i = 0; i < TABLE_SIZE; i++) {
        int index = (start + i) % TABLE_SIZE;

        if (status[index] == 2 && deletedIndex == -1) {
            deletedIndex = index;
        }

        if (status[index] == 0) {
            int target = (deletedIndex != -1)
                         ? deletedIndex : index;

            hashTable[target].passengerID = id;
            hashTable[target].passengerName = name;
            status[target] = 1;

            cout << "Passenger inserted successfully!\n";
            cout << "Hash Table Index: " << target << '\n';
            return;
        }
    }

    // Reuse deleted slot if no empty slot remains
    if (deletedIndex != -1) {
        hashTable[deletedIndex].passengerID = id;
        hashTable[deletedIndex].passengerName = name;
        status[deletedIndex] = 1;

        cout << "Passenger inserted successfully!\n";
        cout << "Hash Table Index: " << deletedIndex << '\n';
        return;
    }

    cout << "Hash table is full!\n";
}

// Search and display passenger details
void searchAndDisplayPassenger(int id) {
    int index = searchPassenger(id);

    if (index == -1) {
        cout << "Passenger not found!\n";
    } else {
        cout << "\nPassenger Found!\n";
        cout << "Passenger ID   : "
             << hashTable[index].passengerID << '\n';
        cout << "Passenger Name : "
             << hashTable[index].passengerName << '\n';
        cout << "Hash Index     : " << index << '\n';
    }
}

// Delete passenger
void deletePassenger(int id) {
    int index = searchPassenger(id);

    if (index == -1) {
        cout << "Passenger not found!\n";
        return;
    }

    // Do not mark as Empty; preserve the probing chain
    status[index] = 2;

    cout << "Passenger deleted successfully!\n";
}

// Display all hash table slots
void displayHashTable() {
    cout << "\n====== AeroDesk Passenger Hash Table ======\n";

    for (int i = 0; i < TABLE_SIZE; i++) {
        cout << "Index " << i << " : ";

        if (status[i] == 1) {
            cout << "ID = " << hashTable[i].passengerID
                 << ", Name = " << hashTable[i].passengerName;
        } else if (status[i] == 2) {
            cout << "Deleted";
        } else {
            cout << "Empty";
        }

        cout << '\n';
    }
}

// Integration function called by AeroDesk_Main.cpp
void runHashingModule() {
    int choice;
    int id;
    string name;

    do {
        cout << "\n====================================\n";
        cout << "     AERODESK PL CO4 - HASHING\n";
        cout << "====================================\n";
        cout << "1. Insert Passenger\n";
        cout << "2. Search Passenger\n";
        cout << "3. Delete Passenger\n";
        cout << "4. Display Hash Table\n";
        cout << "5. Return to AeroDesk\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Returning to AeroDesk.\n";
            return;
        }

        switch (choice) {
            case 1:
                cout << "Enter Passenger ID: ";

                if (!(cin >> id)) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid ID. Returning to AeroDesk.\n";
                    return;
                }

                cout << "Enter Passenger Name: ";
                cin >> ws;
                getline(cin, name);

                insertPassenger(id, name);
                break;

            case 2:
                cout << "Enter Passenger ID to search: ";

                if (!(cin >> id)) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid ID. Returning to AeroDesk.\n";
                    return;
                }

                searchAndDisplayPassenger(id);
                break;

            case 3:
                cout << "Enter Passenger ID to delete: ";

                if (!(cin >> id)) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid ID. Returning to AeroDesk.\n";
                    return;
                }

                deletePassenger(id);
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
