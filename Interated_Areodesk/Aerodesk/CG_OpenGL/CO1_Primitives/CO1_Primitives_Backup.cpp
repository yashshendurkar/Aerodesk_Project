#include <GL/glut.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <stack>
#include <string>

using namespace std;

// =====================================================
// STACK MODULE
// =====================================================

#define STACK_SIZE 10

string operationStack[STACK_SIZE];
int stackTop = -1;

void pushOperation(string operation)
{
    if (stackTop == STACK_SIZE - 1)
    {
        cout << "\nStack Overflow!\n";
        return;
    }

    operationStack[++stackTop] = operation;

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

bool enqueueFlight(const string& flightID)
{
    if (queueCount == QUEUE_SIZE)
    {
        cout << "\n[QUEUE] Queue Overflow!\n";
        return false;
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

    cout << "\n[QUEUE] Flight Added: "
         << flightID << endl;

    return true;
}

string dequeueFlight()
{
    if (queueCount == 0)
    {
        cout << "\n[QUEUE] Queue is empty.\n";
        return "";
    }

    string flightID = flightQueue[queueFront];

    queueFront = (queueFront + 1) % QUEUE_SIZE;
    queueCount--;

    if (queueCount == 0)
    {
        queueFront = 0;
        queueRear = -1;
    }

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

const int MAX_FLIGHTS = 4;

Flight flights[MAX_FLIGHTS] =
{
    {"AI101", 220, 1000, 85, 100},
    {"AI202", 250, 2000, 100, 120},
    {"AI303", 280, 3000, 72, 150},
    {"AI404", 200, 1500, 92, 80}
};

// =====================================================
// OOP - CLASS AND OBJECT
// =====================================================

class OOPFlight
{
public:
    string flightID;
    int speed;
    int altitude;
    int fuel;
    int passengers;

    void setFlightData(const char* id, int spd, int alt, int fl, int pass)
    {
        flightID = id;
        speed = spd;
        altitude = alt;
        fuel = fl;
        passengers = pass;
    }
};

OOPFlight oopFlight;
bool showOOPPanel = true;

void updateOOPFlightObject()
{
    if (currentNode == NULL)
        return;

    oopFlight.setFlightData(
        currentNode->data.flightID,
        currentNode->data.speed,
        currentNode->data.altitude,
        currentNode->data.fuel,
        currentNode->data.passengers
    );
}

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

float aircraftX = 180.0f;
float aircraftY = 330.0f;

float speedValue = 0.0f;
float altitudeValue = 0.0f;
float fuelValue = 100.0f;

bool paused = false;
bool waiting = false;

// =====================================================
// WORLD SETTINGS
// =====================================================

const float RUNWAY_Y = 330.0f;
const float TAKEOFF_Y = 470.0f;
const float MAX_FLIGHT_Y = 850.0f;

const float LEFT_EDGE = 100.0f;
const float RIGHT_EDGE = 1350.0f;

// =====================================================
// DRAW RECTANGLE
// =====================================================

void drawRect(float x1, float y1, float x2, float y2)
{
    glBegin(GL_QUADS);

    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);

    glEnd();
}

// =====================================================
// DRAW TEXT
// =====================================================

void drawText(float x, float y, const char* text)
{
    glRasterPos2f(x, y);

    for (int i = 0; text[i] != '\0'; i++)
    {
        glutBitmapCharacter(
            GLUT_BITMAP_HELVETICA_18,
            text[i]
        );
    }
}

// =====================================================
// CLOUD
// =====================================================

void drawCloud(float x, float y)
{
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_POLYGON);

    for (int i = 0; i < 360; i += 10)
    {
        float angle =
            i * 3.14159f / 180.0f;

        glVertex2f(
            x + 70.0f * cos(angle),
            y + 30.0f * sin(angle)
        );
    }

    glEnd();

    glBegin(GL_POLYGON);

    for (int i = 0; i < 360; i += 10)
    {
        float angle =
            i * 3.14159f / 180.0f;

        glVertex2f(
            x + 35.0f +
            45.0f * cos(angle),

            y + 20.0f +
            25.0f * sin(angle)
        );
    }

    glEnd();
}

// =====================================================
// DRAW AIRCRAFT
// =====================================================

void drawAircraft(float x, float y)
{
    // BODY

    glColor3f(
        0.92f,
        0.92f,
        0.95f
    );

    drawRect(
        x,
        y,
        x + 120,
        y + 28
    );

    // NOSE

    glColor3f(
        0.75f,
        0.78f,
        0.82f
    );

    glBegin(GL_TRIANGLES);

    glVertex2f(
        x + 120,
        y
    );

    glVertex2f(
        x + 150,
        y + 14
    );

    glVertex2f(
        x + 120,
        y + 28
    );

    glEnd();

    // MAIN WING

    glColor3f(
        0.20f,
        0.25f,
        0.35f
    );

    glBegin(GL_TRIANGLES);

    glVertex2f(
        x + 60,
        y + 15
    );

    glVertex2f(
        x + 25,
        y - 35
    );

    glVertex2f(
        x + 85,
        y + 15
    );

    glEnd();

    // TOP WING

    glBegin(GL_TRIANGLES);

    glVertex2f(
        x + 75,
        y + 15
    );

    glVertex2f(
        x + 105,
        y + 60
    );

    glVertex2f(
        x + 92,
        y + 15
    );

    glEnd();

    // TAIL

    glColor3f(
        0.10f,
        0.20f,
        0.55f
    );

    glBegin(GL_TRIANGLES);

    glVertex2f(
        x + 15,
        y + 20
    );

    glVertex2f(
        x,
        y + 55
    );

    glVertex2f(
        x + 35,
        y + 25
    );

    glEnd();

    // WINDOWS

    glColor3f(
        0.0f,
        0.65f,
        0.90f
    );

    for (int i = 0; i < 5; i++)
    {
        drawRect(
            x + 35 + i * 14,
            y + 14,
            x + 43 + i * 14,
            y + 22
        );
    }
}

// =====================================================
// LINKED LIST
// =====================================================

FlightNode* createNode(Flight data)
{
    FlightNode* newNode =
        new FlightNode;

    newNode->data = data;
    newNode->next = NULL;

    return newNode;
}

void insertFlight(Flight data)
{
    FlightNode* newNode =
        createNode(data);

    if (head == NULL)
    {
        head = newNode;
        return;
    }

    FlightNode* temp = head;

    while (temp->next != NULL)
    {
        temp = temp->next;
    }

    temp->next = newNode;
}

void buildFlightList()
{
    head = NULL;

    for (int i = 0;
         i < MAX_FLIGHTS;
         i++)
    {
        insertFlight(flights[i]);
    }

    currentNode = head;
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
        cout << temp->data.flightID
             << " -> ";

        temp = temp->next;
    }

    cout << "NULL\n";
    cout << "====================================\n";
}

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

// =====================================================
// RESET
// =====================================================

void resetFlightSimulation()
{
    if (currentNode == NULL)
        return;

    aircraftX = 180.0f;
    aircraftY = RUNWAY_Y;

    speedValue = 0.0f;
    altitudeValue = 0.0f;

    fuelValue =
        currentNode->data.fuel;

    phase = ON_GROUND;

    paused = false;

    removeFlightFromQueue(currentNode->data.flightID);
    waiting = false;

    pushOperation(string("RESET - ") + currentNode->data.flightID);
}

// =====================================================
// NEXT FLIGHT
// =====================================================

void nextFlight()
{
    if (currentNode != NULL &&
        currentNode->next != NULL)
    {
        removeFlightFromQueue(currentNode->data.flightID);
        waiting = false;

        currentNode =
            currentNode->next;

        aircraftX = 180.0f;
        aircraftY = RUNWAY_Y;

        speedValue = 0.0f;
        altitudeValue = 0.0f;

        fuelValue =
            currentNode->data.fuel;

        phase = ON_GROUND;

        paused = false;
        waiting = false;

        cout << "\nCurrent Flight: "
             << currentNode->data.flightID
             << endl;
    }
    else
    {
        cout << "\nAlready at last flight.\n";
    }

    glutPostRedisplay();
}

// =====================================================
// PREVIOUS FLIGHT
// =====================================================

void previousFlight()
{
    if (currentNode == head)
    {
        cout << "\nAlready at first flight.\n";
        return;
    }

    FlightNode* temp = head;

    while (
        temp != NULL &&
        temp->next != currentNode
    )
    {
        temp = temp->next;
    }

    if (temp != NULL)
    {
        if (currentNode != NULL)
            removeFlightFromQueue(currentNode->data.flightID);

        waiting = false;
        currentNode = temp;

        aircraftX = 180.0f;
        aircraftY = RUNWAY_Y;

        speedValue = 0.0f;
        altitudeValue = 0.0f;

        fuelValue =
            currentNode->data.fuel;

        phase = ON_GROUND;

        paused = false;
        waiting = false;

        cout << "\nCurrent Flight: "
             << currentNode->data.flightID
             << endl;
    }

    glutPostRedisplay();
}

// =====================================================
// TAKEOFF
// =====================================================

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

        cout << "Flight: "
             << currentNode->data.flightID
             << endl;
    }
}

// =====================================================
// LANDING
// =====================================================

void startLanding()
{
    if (currentNode == NULL)
        return;

    if (phase == TAKING_OFF ||
        phase == CRUISING)
    {
        phase = LANDING;

        pushOperation(
            string("LANDING - ") +
            currentNode->data.flightID
        );

        paused = false;
        waiting = false;

        cout << "\nLANDING STARTED\n";
    }
}

// =====================================================
// WAIT
// =====================================================

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
}

// =====================================================
// PAUSE
// =====================================================

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

// =====================================================
// UPDATE
// =====================================================

void update(int value)
{
    if (!paused &&
        !waiting &&
        currentNode != NULL)
    {
        // OOP CLASS & OBJECT PANEL TOGGLE
    if (worldX >= 30 && worldX <= 235 &&
        worldY >= 690 && worldY <= 900)
    {
        showOOPPanel = !showOOPPanel;
        cout << "\n[MOUSE] OOP CLASS & OBJECT PANEL "
             << (showOOPPanel ? "SHOWN" : "HIDDEN") << "\n";
        glutPostRedisplay();
        return;
    }

    // TAKEOFF

        if (phase == TAKING_OFF)
        {
            if (speedValue <
                currentNode->data.speed)
            {
                speedValue += 5.0f;
            }

            aircraftX +=
                1.0f +
                speedValue * 0.01f;

            if (aircraftY < TAKEOFF_Y)
            {
                aircraftY += 7.0f;

                altitudeValue += 40.0f;
            }
            else
            {
                altitudeValue += 20.0f;

                if (
                    altitudeValue >=
                    currentNode->data.altitude
                )
                {
                    altitudeValue =
                        currentNode->data.altitude;

                    aircraftY =
                        MAX_FLIGHT_Y;

                    phase = CRUISING;

                    cout <<
                        "\nFLIGHT REACHED CRUISING ALTITUDE\n";

                    pushOperation(
                        string("CRUISE - ") +
                        currentNode->data.flightID
                    );
                }
                else
                {
                    float ratio =
                        altitudeValue /
                        (float)
                        currentNode->data.altitude;

                    aircraftY =
                        RUNWAY_Y +
                        ratio *
                        (MAX_FLIGHT_Y -
                         RUNWAY_Y);
                }
            }

            fuelValue -= 0.02f;

            if (fuelValue < 0)
                fuelValue = 0;
        }

        // CRUISING

        else if (phase == CRUISING)
        {
            aircraftX +=
                1.0f +
                speedValue * 0.015f;

            if (aircraftX >
                RIGHT_EDGE)
            {
                aircraftX =
                    LEFT_EDGE;
            }

            fuelValue -= 0.01f;

            if (fuelValue < 0)
                fuelValue = 0;
        }

        // LANDING

        else if (phase == LANDING)
        {
            if (speedValue > 80)
                speedValue -= 2.0f;

            if (aircraftY > RUNWAY_Y)
            {
                aircraftY -= 5.0f;

                altitudeValue -= 30.0f;

                if (altitudeValue < 0)
                    altitudeValue = 0;
            }
            else
            {
                aircraftY =
                    RUNWAY_Y;

                altitudeValue = 0;

                speedValue = 0;

                phase = ON_GROUND;

                cout <<
                    "\nLANDING COMPLETED\n";

                pushOperation(
                    string("LANDED - ") +
                    currentNode->data.flightID
                );
            }

            aircraftX += 1.0f;
        }
    }

    glutPostRedisplay();

    glutTimerFunc(
        40,
        update,
        0
    );
}

// =====================================================
// DISPLAY
// =====================================================

void display()
{
    updateOOPFlightObject();

    glClear(
        GL_COLOR_BUFFER_BIT
    );

    glLoadIdentity();

    // SKY

    glColor3f(
        0.35f,
        0.68f,
        0.90f
    );

    drawRect(
        0,
        0,
        1450,
        1100
    );

    // HEADER

    glColor3f(
        0.03f,
        0.05f,
        0.10f
    );

    drawRect(
        30,
        1010,
        1420,
        1090
    );

    // FLIGHT INFO

    if (currentNode != NULL)
    {
        char info[300];

        sprintf(
            info,
            "Flight: %s   Node: %d/4   Speed: %.0f km/h   Altitude: %.0f m   Fuel: %.0f%%   Passengers: %d",
            currentNode->data.flightID,
            getCurrentIndex(),
            speedValue,
            altitudeValue,
            fuelValue,
            currentNode->data.passengers
        );

        glColor3f(
            1.0f,
            1.0f,
            1.0f
        );

        drawText(
            55,
            1050,
            info
        );
    }

    // STATUS

    const char* status =
        "READY";

    if (phase == TAKING_OFF)
        status = "TAKING OFF";

    else if (phase == CRUISING)
        status = "FLYING";

    else if (phase == LANDING)
        status = "LANDING";

    if (waiting)
        status = "WAITING";

    if (paused)
        status = "PAUSED";

    char statusText[100];

    sprintf(
        statusText,
        "STATUS: %s",
        status
    );

    glColor3f(
        1.0f,
        1.0f,
        1.0f
    );

    drawText(
        1140,
        1050,
        statusText
    );

    // LATEST STACK OPERATION

    char stackText[150];

    sprintf(
        stackText,
        "LATEST OPERATION: %s",
        getLatestOperation().c_str()
    );

    glColor3f(
        0.0f,
        0.0f,
        0.0f
    );

    drawText(
        55,
        920,
        stackText
    );

    char queueText[200];

    sprintf(
        queueText,
        "QUEUE: %d/%d   NEXT: %s",
        queueCount,
        QUEUE_SIZE,
        getNextQueuedFlight().c_str()
    );

    glColor3f(
        0.0f,
        0.0f,
        0.0f
    );

    drawText(
        55,
        890,
        queueText
    );

    // TITLE

    glColor3f(
        0.0f,
        0.0f,
        0.0f
    );

    drawText(
        560,
        950,
        "AERODESK AIRPORT"
    );

    // CLOUDS

    drawCloud(
        250,
        880
    );

    drawCloud(
        1150,
        900
    );

    // TERMINAL

    glColor3f(
        0.75f,
        0.78f,
        0.86f
    );

    drawRect(
        250,
        400,
        950,
        650
    );

    // ROOF

    glColor3f(
        0.12f,
        0.20f,
        0.50f
    );

    glBegin(GL_TRIANGLES);

    glVertex2f(
        210,
        650
    );

    glVertex2f(
        600,
        790
    );

    glVertex2f(
        990,
        650
    );

    glEnd();

    // WINDOWS

    glColor3f(
        0.0f,
        0.65f,
        0.85f
    );

    drawRect(
        320,
        520,
        390,
        580
    );

    drawRect(
        430,
        520,
        500,
        580
    );

    drawRect(
        540,
        520,
        610,
        580
    );

    // DOOR

    glColor3f(
        0.05f,
        0.10f,
        0.20f
    );

    drawRect(
        625,
        400,
        775,
        550
    );

    glColor3f(
        0.0f,
        0.0f,
        0.0f
    );

    drawText(
        370,
        370,
        "TERMINAL"
    );

    // CONTROL TOWER

    glColor3f(
        0.65f,
        0.65f,
        0.70f
    );

    drawRect(
        1030,
        400,
        1140,
        730
    );

    glColor3f(
        0.05f,
        0.12f,
        0.20f
    );

    drawRect(
        1000,
        700,
        1170,
        760
    );

    // RUNWAY

    glColor3f(
        0.20f,
        0.20f,
        0.22f
    );

    drawRect(
        80,
        250,
        1370,
        330
    );

    // RUNWAY MARKINGS

    glColor3f(
        1.0f,
        1.0f,
        1.0f
    );

    for (int x = 100;
         x < 1350;
         x += 100)
    {
        drawRect(
            x,
            287,
            x + 50,
            295
        );
    }

    // AIRCRAFT

    drawAircraft(
        aircraftX,
        aircraftY
    );

    // FLIGHT LABEL

    if (currentNode != NULL)
    {
        glColor3f(
            0.0f,
            0.0f,
            0.0f
        );

        drawText(
            aircraftX,
            aircraftY + 80,
            currentNode->data.flightID
        );
    }

    // BUTTON PANEL

    glColor3f(
        0.03f,
        0.05f,
        0.10f
    );

    drawRect(
        20,
        20,
        1430,
        110
    );

    // TAKEOFF

    glColor3f(
        0.1f,
        0.7f,
        0.2f
    );

    drawRect(
        40,
        30,
        220,
        95
    );

    // LAND

    glColor3f(
        0.8f,
        0.3f,
        0.2f
    );

    drawRect(
        235,
        30,
        400,
        95
    );

    // PAUSE

    glColor3f(
        0.8f,
        0.6f,
        0.1f
    );

    drawRect(
        415,
        30,
        580,
        95
    );

    // WAIT

    glColor3f(
        0.6f,
        0.35f,
        0.15f
    );

    drawRect(
        595,
        30,
        760,
        95
    );

    // RESET

    glColor3f(
        0.4f,
        0.4f,
        0.8f
    );

    drawRect(
        775,
        30,
        940,
        95
    );

    // SPEED+

    glColor3f(
        0.2f,
        0.6f,
        0.8f
    );

    drawRect(
        955,
        30,
        1120,
        95
    );

    // SPEED-

    glColor3f(
        0.2f,
        0.5f,
        0.7f
    );

    drawRect(
        1135,
        30,
        1260,
        95
    );

    // ALT+

    glColor3f(
        0.5f,
        0.3f,
        0.8f
    );

    drawRect(
        1275,
        30,
        1400,
        95
    );

    // BUTTON TEXT

    glColor3f(
        1.0f,
        1.0f,
        1.0f
    );

    drawText(
        75,
        62,
        "TAKEOFF"
    );

    drawText(
        275,
        62,
        "LAND"
    );

    drawText(
        450,
        62,
        "PAUSE"
    );

    drawText(
        630,
        62,
        "WAIT"
    );

    drawText(
        810,
        62,
        "RESET"
    );

    drawText(
        985,
        62,
        "SPEED+"
    );

    drawText(
        1160,
        62,
        "SPEED-"
    );

    drawText(
        1300,
        62,
        "ALT+"
    );

    // HELP

    glColor3f(
        0.0f,
        0.0f,
        0.0f
    );

    drawText(
        60,
        180,
        "SPACE: Takeoff | L: Land | W: Queue/Wait | P: Pause | R: Reset | N/B: Flight | O: OOP Panel | Q: Queue | D: All | ESC: Exit"
    );


    // OOP CLASS AND OBJECT PANEL
    if (showOOPPanel)
    {
        glColor3f(0.03f, 0.05f, 0.10f);
        drawRect(30, 690, 235, 900);

        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(45, 870, "OOP: CLASS & OBJECT");

        drawText(45, 835, "CLASS: OOPFlight");

        char oopText[120];
        sprintf(oopText, "OBJECT: %s", oopFlight.flightID.c_str());
        drawText(45, 800, oopText);

        sprintf(oopText, "Speed: %d km/h", oopFlight.speed);
        drawText(45, 765, oopText);

        sprintf(oopText, "Altitude: %d m", oopFlight.altitude);
        drawText(45, 730, oopText);

        sprintf(oopText, "Fuel: %d%%", oopFlight.fuel);
        drawText(45, 705, oopText);
    }

    // WAITING QUEUE PANEL
    glColor3f(0.03f, 0.05f, 0.10f);
    drawRect(1050, 500, 1420, 850);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(1080, 815, "WAITING QUEUE");

    char queueInfo[80];
    sprintf(queueInfo, "QUEUE SIZE: %d / %d", queueCount, QUEUE_SIZE);
    drawText(1080, 775, queueInfo);

    char nextInfo[80];
    sprintf(nextInfo, "NEXT FLIGHT: %s", getNextQueuedFlight().c_str());
    drawText(1080, 740, nextInfo);

    if (queueCount == 0)
    {
        drawText(1080, 690, "No waiting flights");
    }
    else
    {
        int queueY = 690;
        for (int i = 0; i < queueCount; i++)
        {
            int index = (queueFront + i) % QUEUE_SIZE;
            char flightText[60];
            sprintf(flightText, "%d. %s", i + 1, flightQueue[index].c_str());
            drawText(1080, queueY, flightText);
            queueY -= 35;
        }
    }

    glutSwapBuffers();
}

// =====================================================
// KEYBOARD
// =====================================================

void keyboard(
    unsigned char key,
    int x,
    int y)
{
    switch (key)
    {
        case 'n':
        case 'N':
            nextFlight();
            break;

        case 'b':
        case 'B':
            previousFlight();
            break;

        case 'd':
        case 'D':
            displayFlightList();
            displayStackHistory();
            displayQueue();
            break;

        case 'q':
        case 'Q':
            displayQueue();
            break;

        case 'o':
        case 'O':
            showOOPPanel = !showOOPPanel;
            cout << "\n[OOP] CLASS & OBJECT PANEL "
                 << (showOOPPanel ? "SHOWN" : "HIDDEN") << "\n";
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

        case 27:
            exit(0);
            break;
    }

    glutPostRedisplay();
}

// =====================================================
// SPECIAL KEYS
// =====================================================

void specialKeys(
    int key,
    int x,
    int y)
{
    if (key == GLUT_KEY_UP)
    {
        speedValue += 10;

        if (currentNode != NULL &&
            speedValue >
            currentNode->data.speed)
        {
            speedValue =
                currentNode->data.speed;
        }
    }

    else if (key == GLUT_KEY_DOWN)
    {
        speedValue -= 10;

        if (speedValue < 0)
            speedValue = 0;
    }

    glutPostRedisplay();
}

// =====================================================
// MOUSE
// =====================================================

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

    int windowWidth =
        glutGet(GLUT_WINDOW_WIDTH);

    int windowHeight =
        glutGet(GLUT_WINDOW_HEIGHT);

    float worldX =
        (float)x *
        1450.0f /
        windowWidth;

    float worldY =
        (float)(windowHeight - y) *
        1100.0f /
        windowHeight;

    // TAKEOFF

    if (
        worldX >= 40 &&
        worldX <= 220 &&
        worldY >= 30 &&
        worldY <= 95
    )
    {
        cout <<
            "\n[MOUSE] TAKEOFF CLICKED\n";

        startTakeoff();
    }

    // LAND

    else if (
        worldX >= 235 &&
        worldX <= 400 &&
        worldY >= 30 &&
        worldY <= 95
    )
    {
        cout <<
            "\n[MOUSE] LAND CLICKED\n";

        startLanding();
    }

    // PAUSE

    else if (
        worldX >= 415 &&
        worldX <= 580 &&
        worldY >= 30 &&
        worldY <= 95
    )
    {
        cout <<
            "\n[MOUSE] PAUSE CLICKED\n";

        togglePause();
    }

    // WAIT

    else if (
        worldX >= 595 &&
        worldX <= 760 &&
        worldY >= 30 &&
        worldY <= 95
    )
    {
        cout <<
            "\n[MOUSE] WAIT CLICKED\n";

        toggleWaiting();
    }

    // RESET

    else if (
        worldX >= 775 &&
        worldX <= 940 &&
        worldY >= 30 &&
        worldY <= 95
    )
    {
        cout <<
            "\n[MOUSE] RESET CLICKED\n";

        resetFlightSimulation();
    }

    // SPEED+

    else if (
        worldX >= 955 &&
        worldX <= 1120 &&
        worldY >= 30 &&
        worldY <= 95
    )
    {
        cout <<
            "\n[MOUSE] SPEED+ CLICKED\n";

        speedValue += 20;

        if (
            currentNode != NULL &&
            speedValue >
            currentNode->data.speed
        )
        {
            speedValue =
                currentNode->data.speed;
        }
    }

    // SPEED-

    else if (
        worldX >= 1135 &&
        worldX <= 1260 &&
        worldY >= 30 &&
        worldY <= 95
    )
    {
        cout <<
            "\n[MOUSE] SPEED- CLICKED\n";

        speedValue -= 20;

        if (speedValue < 0)
            speedValue = 0;
    }

    // ALT+

    else if (
        worldX >= 1275 &&
        worldX <= 1400 &&
        worldY >= 30 &&
        worldY <= 95
    )
    {
        cout <<
            "\n[MOUSE] ALT+ CLICKED\n";

        altitudeValue += 100;

        if (
            currentNode != NULL &&
            altitudeValue >
            currentNode->data.altitude
        )
        {
            altitudeValue =
                currentNode->data.altitude;
        }
    }

    glutPostRedisplay();
}

// =====================================================
// INITIALIZATION
// =====================================================

void init()
{
    glClearColor(
        0.35f,
        0.68f,
        0.90f,
        1.0f
    );

    glMatrixMode(
        GL_PROJECTION
    );

    glLoadIdentity();

    gluOrtho2D(
        0,
        1450,
        0,
        1100
    );

    glMatrixMode(
        GL_MODELVIEW
    );

    glLoadIdentity();
}

// =====================================================
// MAIN
// =====================================================

int main(
    int argc,
    char** argv)
{
    buildFlightList();

    displayFlightList();

    currentNode = head;
    updateOOPFlightObject();

    resetFlightSimulation();

    glutInit(
        &argc,
        argv
    );

    glutInitDisplayMode(
        GLUT_DOUBLE |
        GLUT_RGB
    );

    glutInitWindowSize(
        1450,
        1100
    );

    glutInitWindowPosition(
        50,
        20
    );

    glutCreateWindow(
        "AeroDesk - Final Dynamic Airport"
    );

    init();

    glutDisplayFunc(display);

    glutKeyboardFunc(keyboard);

    glutSpecialFunc(specialKeys);

    glutMouseFunc(mouse);

    glutTimerFunc(
        40,
        update,
        0
    );

    glutMainLoop();

    return 0;
}
