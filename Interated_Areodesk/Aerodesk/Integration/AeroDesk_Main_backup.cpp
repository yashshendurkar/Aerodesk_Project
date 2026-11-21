#include <iostream>
using namespace std;

// ===============================
// MODULE FUNCTIONS
// ===============================

void runPLModule();
void runPLCO2Module();

void runOOPModule();
void runOOPCO2Module();


// ===============================
// COA 80386 MODULE
// ===============================

void showCOA()
{
    cout << "\n==============================\n";
    cout << "        COA MODULE\n";
    cout << "==============================\n";

    cout << "COA CO1 : 80386 Architecture\n";
    cout << "COA CO2 : 80386 Assembly Language\n";
    cout << "COA CO3 : 80386 Arithmetic\n";
    cout << "COA CO4 : 80386 Control / Pipeline\n";

    cout << "\nExecution Platform:\n";
    cout << "NASM + 32-bit GCC\n";

    cout << "\n80386 Programs:\n";
    cout << "1. CO1_80386_Architecture.exe\n";
    cout << "2. CO2_80386_Assembly.exe\n";
    cout << "3. CO3_Arithmetic.exe\n";
    cout << "4. CO4_Control_Pipeline.exe\n";

    cout << "\nCOA Module Status: READY\n";


    // ===============================
    // COA CO1
    // ===============================

    cout << "\nLaunching COA 80386 CO1...\n";

   system("..\\COA_8086\\CO1_Architecture\\CO1_80386_Architecture.exe");


    // ===============================
    // COA CO2
    // ===============================

    cout << "\nLaunching COA 80386 CO2...\n";

    system("..\\COA_8086\\CO2_Assembly\\CO2_80386_Assembly.exe");


    // ===============================
    // COA CO3
    // ===============================

    cout << "\nLaunching COA 80386 CO3...\n";

    system("..\\COA_8086\\CO3_Arithmetic\\CO3_Arithmetic.exe");
}


// ===============================
// CG OPENGL MODULE
// ===============================

void showCG()
{
    cout << "\n==============================\n";
    cout << "        CG MODULE\n";
    cout << "==============================\n";

    cout << "CG CO1 : OpenGL Primitives\n";
    cout << "CG Program: CO1_Primitives.cpp\n";
    cout << "Execution: OpenGL / FreeGLUT\n";

    cout << "\nLaunching CG Module...\n";

    system("\"C:\\Users\\ASUS\\Interated_Areodesk\\Aerodesk\\CG_OpenGL\\CO1_Primitives\\CO1_Primitives.exe\"");
}


// ===============================
// MAIN AERODESK SYSTEM
// ===============================

int main()
{
    int choice;

    do
    {
        cout << "\n====================================\n";
        cout << "          AERODESK SYSTEM\n";
        cout << "====================================\n";

        cout << "1. PL CO1 - Array\n";
        cout << "2. PL CO2 - Linked List\n";
        cout << "3. OOP CO1 - Class and Object\n";
        cout << "4. OOP CO2 - Constructor + Encapsulation\n";
        cout << "5. COA 80386 Module\n";
        cout << "6. CG OpenGL Module\n";
        cout << "7. Exit\n";

        cout << "\nEnter your choice: ";
        cin >> choice;

        switch(choice)
        {
            case 1:
                runPLModule();
                break;

            case 2:
                runPLCO2Module();
                break;

            case 3:
                runOOPModule();
                break;

            case 4:
                runOOPCO2Module();
                break;

            case 5:
                showCOA();
                break;

            case 6:
                showCG();
                break;

            case 7:
                cout << "\nExiting AeroDesk...\n";
                break;

            default:
                cout << "\nInvalid choice!\n";
        }

    } while(choice != 7);

    return 0;
}