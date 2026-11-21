#include <iostream>
#include <string>
using namespace std;

#define SIZE 5

class Queue
{
private:
    string flights[SIZE];
    int front;
    int rear;

public:

    Queue()
    {
        front = -1;
        rear = -1;
    }

    void enqueue(string flight)
    {
        if (rear == SIZE - 1)
        {
            cout << "\nQueue Overflow!";
            return;
        }

        if (front == -1)
        {
            front = 0;
        }

        rear++;
        flights[rear] = flight;

        cout << "\nFlight added to waiting queue: "
             << flight << endl;
    }

    void dequeue()
    {
        if (front == -1 || front > rear)
        {
            cout << "\nQueue Underflow!";
            return;
        }

        cout << "\nFlight departed from queue: "
             << flights[front] << endl;

        front++;

        if (front > rear)
        {
            front = -1;
            rear = -1;
        }
    }

    void showFront()
    {
        if (front == -1)
        {
            cout << "\nQueue is empty!";
            return;
        }

        cout << "\nNext Flight: "
             << flights[front] << endl;
    }

    void display()
    {
        if (front == -1)
        {
            cout << "\nNo waiting flights.";
            return;
        }

        cout << "\n====================================\n";
        cout << "       AERODESK WAITING QUEUE\n";
        cout << "====================================\n";

        for (int i = front; i <= rear; i++)
        {
            cout << i - front + 1 << ". "
                 << flights[i] << endl;
        }
    }
};


// ===============================
// AERODESK QUEUE MODULE
// ===============================
void runQueueModule()
{
    Queue q;

    int choice;
    string flight;

    do
    {
        cout << "\n====================================\n";
        cout << "          AERODESK QUEUE\n";
        cout << "====================================\n";

        cout << "1. Add Flight\n";
        cout << "2. Depart Flight\n";
        cout << "3. Show Next Flight\n";
        cout << "4. Display Waiting Flights\n";
        cout << "5. Exit Queue Module\n";

        cout << "\nEnter choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            cout << "Enter Flight Number: ";
            cin >> flight;

            q.enqueue(flight);
            break;

        case 2:
            q.dequeue();
            break;

        case 3:
            q.showFront();
            break;

        case 4:
            q.display();
            break;

        case 5:
            cout << "\nReturning to AeroDesk...\n";
            break;

        default:
            cout << "\nInvalid choice!";
        }

    } while (choice != 5);
}