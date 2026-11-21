
#include <iostream>
#include <windows.h>
#include <limits>

using namespace std;

// ==========================================
// MODULE FUNCTION DECLARATIONS
// ==========================================

// Programming Language modules
void runPLModule();
void runPLCO2Module();
void runHashingModule();
void runStackModule();
void runQueueModule();

// OOP modules
void runOOPModule();
void runOOPCO2Module();
void runOOPCO3Module();

// Other modules
void showCOA();
void showCG();

// ==========================================
// INPUT VALIDATION FUNCTION
// ==========================================

int getValidChoice()
{
    int choice;

    if (!(cin >> choice))
    {
        cin.clear();
        cin.ignore(
            numeric_limits<streamsize>::max(), '\n'
        );

        cout << "\nInvalid input! Please enter a number.\n";
        return -1;
    }

    return choice;
}

// ==========================================
// LAUNCH EXTERNAL WINDOWS PROGRAM
// ==========================================

bool launchProgram(const char* exePath)
{
    DWORD attributes = GetFileAttributesA(exePath);

    if (attributes == INVALID_FILE_ATTRIBUTES ||
        (attributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        cout << "\nERROR: Executable not found.\n";
        cout << "Path: " << exePath << '\n';
        cout << "Windows Error Code: " << GetLastError() << '\n';

        return false;
    }

    STARTUPINFOA startupInfo = {};
    PROCESS_INFORMATION processInfo = {};

    startupInfo.cb = sizeof(startupInfo);

    BOOL success = CreateProcessA(
        exePath,
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &startupInfo,
        &processInfo
    );

    if (!success)
    {
        DWORD errorCode = GetLastError();

        cout << "\nERROR: Could not start program.\n";
        cout << "Path: " << exePath << '\n';
        cout << "Windows Error Code: " << errorCode << '\n';

        return false;
    }

    cout << "\nProgram launched successfully.\n";
    cout << "Waiting for program to finish...\n";

    WaitForSingleObject(processInfo.hProcess, INFINITE);

    DWORD exitCode = 0;

    if (GetExitCodeProcess(processInfo.hProcess, &exitCode))
    {
        cout << "\nExternal program closed.\n";
        cout << "Exit Code: " << exitCode << '\n';
    }

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    return true;
}

// ==========================================
// COA 80386 MODULE
// ==========================================

void showCOA()
{
    int coaChoice = 0;

    do
    {
        cout << "\n====================================\n";
        cout << "          COA 80386 MODULE\n";
        cout << "====================================\n";

        cout << "1. COA CO1 - 80386 Architecture\n";
        cout << "2. COA CO2 - Assembly Language\n";
        cout << "3. COA CO3 - Arithmetic Operations\n";
        cout << "4. COA CO4 - Control / Pipeline\n";
        cout << "5. Back to AeroDesk Main Menu\n";

        cout << "\nEnter COA choice: ";

        coaChoice = getValidChoice();

        if (coaChoice == -1)
            continue;

        switch (coaChoice)
        {
            case 1:
                cout << "\nLaunching COA CO1...\n";

                launchProgram(
                    R"(C:\Users\ASUS\Interated_Areodesk\Aerodesk\COA_8086\CO1_Architecture\CO1_80386_Architecture.exe)"
                );
                break;

            case 2:
                cout << "\nLaunching COA CO2...\n";

                launchProgram(
                    R"(C:\Users\ASUS\Interated_Areodesk\Aerodesk\COA_8086\CO2_Assembly\CO2_80386_Assembly.exe)"
                );
                break;

            // COA CO3 - ADD AND SHIFT + STRINGS
            case 3:
                cout << "\nLaunching COA CO3 Arithmetic...\n";

                cout << "\nImplemented Concepts:\n";
                cout << "1. Add-and-Shift Multiplication\n";
                cout << "2. Flight Fuel Calculation\n";
                cout << "3. String Length\n";
                cout << "4. String Copy\n";
                cout << "5. String Concatenation\n";
                cout << "6. String Comparison\n";
                cout << "7. Character Search\n";

                launchProgram(
                    R"(C:\Users\ASUS\Interated_Areodesk\Aerodesk\COA_8086\CO3_Arithmetic\CO3_Arithmetic.exe)"
                );
                break;

            case 4:
                cout << "\nLaunching COA CO4 Control / Pipeline...\n";

                cout << "\nImplemented Concepts:\n";
                cout << "1. Control Signal Generation\n";
                cout << "2. Control Unit Organization\n";
                cout << "3. Instruction Pipelining\n";

                launchProgram(
                    R"(C:\Users\ASUS\Interated_Areodesk\Aerodesk\COA_8086\CO4_Control_Pipelining\CO4_Control_Pipelining.exe)"
                );
                break;

            case 5:
                cout << "\nReturning to AeroDesk Main Menu...\n";
                break;

            default:
                cout << "\nInvalid choice! Select 1 to 5.\n";
        }

    } while (coaChoice != 5);
}

// ==========================================
// COMPUTER GRAPHICS MODULE
// ==========================================

void showCG()
{
    cout << "\n====================================\n";
    cout << "          CG OPENGL MODULE\n";
    cout << "====================================\n";

    cout << "CG CO1 : OpenGL Primitives\n";
    cout << "Execution: OpenGL / FreeGLUT\n";

    cout << "\nLaunching CG Module...\n";

    launchProgram(
        R"(C:\Users\ASUS\Interated_Areodesk\Aerodesk\CG_OpenGL\CO1_Primitives\CO1_Primitives.exe)"
    );
}

// ==========================================
// MAIN AERODESK MENU
// ==========================================

int main()
{
    int choice = 0;

    do
    {
        cout << "\n====================================\n";
        cout << "             AERODESK SYSTEM\n";
        cout << "====================================\n";

        cout << "\n--- PL MODULES ---\n";
        cout << "1. PL CO1 - Array\n";
        cout << "2. PL CO2 - Linked List\n";
        cout << "3. PL CO4 - Hashing\n";
        cout << "4. PL CO3 - Stack\n";
        cout << "5. PL CO3 - Queue\n";

        cout << "\n--- OOP MODULES ---\n";
        cout << "6. OOP CO1 - Class and Object\n";
        cout << "7. OOP CO2 - Constructor + Encapsulation\n";
        cout << "8. OOP CO3 - Inheritance, Polymorphism, Exceptions, STL\n";

        cout << "\n--- OTHER MODULES ---\n";
        cout << "9. COA 80386 Module\n";
        cout << "10. CG OpenGL Module\n";
        cout << "11. Exit\n";

        cout << "\nEnter your choice: ";

        choice = getValidChoice();

        if (choice == -1)
            continue;

        switch (choice)
        {
            case 1:
                cout << "\nOpening PL CO1 Array Module...\n";
                runPLModule();
                break;

            case 2:
                cout << "\nOpening PL CO2 Linked List Module...\n";
                runPLCO2Module();
                break;

            case 3:
                cout << "\nOpening PL CO4 Hashing Module...\n";
                runHashingModule();
                break;

            case 4:
                cout << "\nOpening PL CO3 Stack Module...\n";
                runStackModule();
                break;

            case 5:
                cout << "\nOpening PL CO3 Queue Module...\n";
                runQueueModule();
                break;

            case 6:
                cout << "\nOpening OOP CO1 Module...\n";
                runOOPModule();
                break;

            case 7:
                cout << "\nOpening OOP CO2 Module...\n";
                runOOPCO2Module();
                break;

            case 8:
                cout << "\nOpening OOP CO3 Module...\n";
                runOOPCO3Module();
                break;

            case 9:
                showCOA();
                break;

            case 10:
                showCG();
                break;

            case 11:
                cout << "\nExiting AeroDesk...\n";
                break;

            default:
                cout << "\nInvalid choice! Select 1 to 11.\n";
        }

    } while (choice != 11);

    return 0;
}
