#include <iostream>
#include <string>
using namespace std;

#define SIZE 5

class Stack
{
private:
    string operations[SIZE];
    int top;

public:

    Stack()
    {
        top = -1;
    }

    void push(string operation)
    {
        if (top == SIZE - 1)
        {
            cout << "\nStack Overflow!";
            return;
        }

        operations[++top] = operation;

        cout << "\nOperation added: "
             << operation << endl;
    }

    void pop()
    {
        if (top == -1)
        {
            cout << "\nStack Underflow!";
            return;
        }

        cout << "\nRemoved operation: "
             << operations[top] << endl;

        top--;
    }

    void peek()
    {
        if (top == -1)
        {
            cout << "\nStack is empty!";
            return;
        }

        cout << "\nLatest Operation: "
             << operations[top] << endl;
    }

    void display()
    {
        if (top == -1)
        {
            cout << "\nNo operations available.";
            return;
        }

        cout << "\n====================================\n";
        cout << "       AERODESK OPERATION HISTORY\n";
        cout << "====================================\n";

        for (int i = top; i >= 0; i--)
        {
            cout << i + 1 << ". "
                 << operations[i] << endl;
        }
    }
};


// ===============================
// AERODESK STACK MODULE
// ===============================
void runStackModule()
{
    Stack s;

    int choice;
    string operation;

    do
    {
        cout << "\n====================================\n";
        cout << "          AERODESK STACK\n";
        cout << "====================================\n";

        cout << "1. Push Operation\n";
        cout << "2. Pop Operation\n";
        cout << "3. Peek Latest Operation\n";
        cout << "4. Display History\n";
        cout << "5. Exit Stack Module\n";

        cout << "\nEnter choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            cin.ignore();

            cout << "Enter operation: ";
            getline(cin, operation);

            s.push(operation);
            break;

        case 2:
            s.pop();
            break;

        case 3:
            s.peek();
            break;

        case 4:
            s.display();
            break;

        case 5:
            cout << "\nReturning to AeroDesk...\n";
            break;

        default:
            cout << "\nInvalid choice!";
        }

    } while (choice != 5);
}