#include <GL/glut.h>
#include <cstdio>
#include <cstring>

// =====================================================
// AERODESK PHASE 1 - DYNAMIC CG FLIGHT SIMULATION
// =====================================================

// Aircraft variables
float aircraftX = -0.70f;
float aircraftY = -0.48f;

float speed = 0.0f;
float altitude = 0.0f;
float fuel = 100.0f;

int passengers = 120;

bool simulationRunning = false;
bool paused = false;
bool landing = false;

int windowWidth = 1000;
int windowHeight = 700;

// =====================================================
// TEXT
// =====================================================

void drawText(float x, float y, const char* text)
{
    glRasterPos2f(x, y);

    for(int i = 0; text[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

// =====================================================
// CLOUD
// =====================================================

void drawCloud(float x, float y)
{
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_POLYGON);

    glVertex2f(x - 0.12f, y);
    glVertex2f(x - 0.08f, y + 0.08f);
    glVertex2f(x, y + 0.12f);
    glVertex2f(x + 0.08f, y + 0.08f);
    glVertex2f(x + 0.14f, y);
    glVertex2f(x + 0.08f, y - 0.05f);
    glVertex2f(x - 0.10f, y - 0.05f);

    glEnd();
}

// =====================================================
// TREE
// =====================================================

void drawTree(float x, float y)
{
    // Trunk
    glColor3f(0.45f, 0.25f, 0.10f);

    glBegin(GL_QUADS);

    glVertex2f(x - 0.025f, y);
    glVertex2f(x + 0.025f, y);
    glVertex2f(x + 0.025f, y + 0.18f);
    glVertex2f(x - 0.025f, y + 0.18f);

    glEnd();

    // Leaves
    glColor3f(0.05f, 0.45f, 0.12f);

    glBegin(GL_TRIANGLES);

    glVertex2f(x, y + 0.42f);
    glVertex2f(x - 0.13f, y + 0.18f);
    glVertex2f(x + 0.13f, y + 0.18f);

    glVertex2f(x, y + 0.34f);
    glVertex2f(x - 0.15f, y + 0.10f);
    glVertex2f(x + 0.15f, y + 0.10f);

    glEnd();
}

// =====================================================
// AIRPLANE
// =====================================================

void drawAirplane(float x, float y)
{
    // Body
    glColor3f(0.95f, 0.95f, 0.95f);

    glBegin(GL_QUADS);

    glVertex2f(x - 0.15f, y);
    glVertex2f(x + 0.15f, y);
    glVertex2f(x + 0.15f, y + 0.05f);
    glVertex2f(x - 0.15f, y + 0.05f);

    glEnd();

    // Wings
    glColor3f(0.1f, 0.3f, 0.9f);

    glBegin(GL_TRIANGLES);

    glVertex2f(x, y + 0.03f);
    glVertex2f(x - 0.20f, y - 0.10f);
    glVertex2f(x + 0.02f, y + 0.03f);

    glVertex2f(x, y + 0.03f);
    glVertex2f(x + 0.20f, y - 0.10f);
    glVertex2f(x - 0.02f, y + 0.03f);

    glEnd();

    // Tail
    glBegin(GL_TRIANGLES);

    glVertex2f(x - 0.15f, y + 0.03f);
    glVertex2f(x - 0.22f, y + 0.10f);
    glVertex2f(x - 0.10f, y + 0.03f);

    glEnd();
}

// =====================================================
// TERMINAL
// =====================================================

void drawTerminal()
{
    // Main building
    glColor3f(0.75f, 0.78f, 0.85f);

    glBegin(GL_QUADS);

    glVertex2f(-0.65f, -0.25f);
    glVertex2f(0.30f, -0.25f);
    glVertex2f(0.30f, 0.15f);
    glVertex2f(-0.65f, 0.15f);

    glEnd();

    // Roof
    glColor3f(0.15f, 0.25f, 0.50f);

    glBegin(GL_TRIANGLES);

    glVertex2f(-0.70f, 0.15f);
    glVertex2f(0.35f, 0.15f);
    glVertex2f(-0.17f, 0.38f);

    glEnd();

    // Entrance
    glColor3f(0.10f, 0.15f, 0.25f);

    glBegin(GL_QUADS);

    glVertex2f(-0.15f, -0.25f);
    glVertex2f(0.05f, -0.25f);
    glVertex2f(0.05f, 0.05f);
    glVertex2f(-0.15f, 0.05f);

    glEnd();

    // Windows
    glColor3f(0.10f, 0.70f, 0.90f);

    for(float x = -0.55f; x <= -0.25f; x += 0.15f)
    {
        glBegin(GL_QUADS);

        glVertex2f(x, -0.02f);
        glVertex2f(x + 0.08f, -0.02f);
        glVertex2f(x + 0.08f, 0.08f);
        glVertex2f(x, 0.08f);

        glEnd();
    }
}

// =====================================================
// CONTROL TOWER
// =====================================================

void drawControlTower()
{
    // Tower body
    glColor3f(0.65f, 0.65f, 0.70f);

    glBegin(GL_QUADS);

    glVertex2f(0.40f, -0.25f);
    glVertex2f(0.55f, -0.25f);
    glVertex2f(0.55f, 0.30f);
    glVertex2f(0.40f, 0.30f);

    glEnd();

    // Tower top
    glColor3f(0.15f, 0.25f, 0.35f);

    glBegin(GL_QUADS);

    glVertex2f(0.35f, 0.30f);
    glVertex2f(0.60f, 0.30f);
    glVertex2f(0.55f, 0.40f);
    glVertex2f(0.40f, 0.40f);

    glEnd();

    // Window
    glColor3f(0.10f, 0.80f, 0.90f);

    glBegin(GL_QUADS);

    glVertex2f(0.42f, 0.32f);
    glVertex2f(0.53f, 0.32f);
    glVertex2f(0.52f, 0.37f);
    glVertex2f(0.43f, 0.37f);

    glEnd();
}

// =====================================================
// RUNWAY
// =====================================================

void drawRunway()
{
    glColor3f(0.12f, 0.12f, 0.15f);

    glBegin(GL_QUADS);

    glVertex2f(-0.95f, -0.85f);
    glVertex2f(0.95f, -0.85f);
    glVertex2f(0.95f, -0.55f);
    glVertex2f(-0.95f, -0.55f);

    glEnd();

    // Runway markings
    glColor3f(1.0f, 1.0f, 1.0f);

    for(float x = -0.85f; x < 0.9f; x += 0.30f)
    {
        glBegin(GL_QUADS);

        glVertex2f(x, -0.71f);
        glVertex2f(x + 0.15f, -0.71f);
        glVertex2f(x + 0.15f, -0.68f);
        glVertex2f(x, -0.68f);

        glEnd();
    }
}

// =====================================================
// BUTTON
// =====================================================

void drawButton(
    float left,
    float right,
    float bottom,
    float top,
    const char* text
)
{
    glColor3f(0.10f, 0.15f, 0.25f);

    glBegin(GL_QUADS);

    glVertex2f(left, bottom);
    glVertex2f(right, bottom);
    glVertex2f(right, top);
    glVertex2f(left, top);

    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);

    float textX = (left + right) / 2.0f - 0.055f;

    drawText(textX, bottom + 0.025f, text);
}

// =====================================================
// CONTROL PANEL
// =====================================================

void drawControlPanel()
{
    // Panel background
    glColor3f(0.05f, 0.07f, 0.12f);

    glBegin(GL_QUADS);

    glVertex2f(-0.95f, 0.82f);
    glVertex2f(0.95f, 0.82f);
    glVertex2f(0.95f, 0.98f);
    glVertex2f(-0.95f, 0.98f);

    glEnd();

    // Status
    glColor3f(1.0f, 1.0f, 1.0f);

    char info[100];

    sprintf(info, "Speed: %.0f km/h", speed);
    drawText(-0.92f, 0.90f, info);

    sprintf(info, "Altitude: %.0f m", altitude);
    drawText(-0.65f, 0.90f, info);

    sprintf(info, "Fuel: %.0f%%", fuel);
    drawText(-0.40f, 0.90f, info);

    sprintf(info, "Passengers: %d", passengers);
    drawText(-0.18f, 0.90f, info);

    if(paused)
        drawText(0.15f, 0.90f, "STATUS: PAUSED");
    else if(landing)
        drawText(0.15f, 0.90f, "STATUS: LANDING");
    else if(simulationRunning)
        drawText(0.15f, 0.90f, "STATUS: FLYING");
    else
        drawText(0.15f, 0.90f, "STATUS: READY");
}

// =====================================================
// DRAW BUTTONS
// =====================================================

void drawControls()
{
    drawButton(-0.92f, -0.68f, -0.98f, -0.88f, "TAKEOFF");

    drawButton(-0.64f, -0.45f, -0.98f, -0.88f, "LAND");

    drawButton(-0.41f, -0.20f, -0.98f, -0.88f, "PAUSE");

    drawButton(-0.16f, 0.05f, -0.98f, -0.88f, "RESET");

    drawButton(0.10f, 0.28f, -0.98f, -0.88f, "SPEED+");

    drawButton(0.33f, 0.51f, -0.98f, -0.88f, "SPEED-");

    drawButton(0.56f, 0.69f, -0.98f, -0.88f, "ALT+");

    drawButton(0.74f, 0.87f, -0.98f, -0.88f, "ALT-");
}

// =====================================================
// RESET AIRCRAFT
// =====================================================

void resetAircraft()
{
    aircraftX = -0.70f;
    aircraftY = -0.48f;

    speed = 0.0f;
    altitude = 0.0f;
    fuel = 100.0f;

    simulationRunning = false;
    paused = false;
    landing = false;

    glutPostRedisplay();
}

// =====================================================
// TAKEOFF
// =====================================================

void takeoff()
{
    landing = false;
    simulationRunning = true;
    paused = false;

    if(speed < 100)
        speed = 100;
}

// =====================================================
// LAND
// =====================================================

void landAircraft()
{
    landing = true;
    paused = false;
}

// =====================================================
// UPDATE SIMULATION
// =====================================================

void update(int value)
{
    if(simulationRunning && !paused)
    {
        // Fuel consumption
        if(speed > 0 && fuel > 0)
        {
            fuel -= 0.03f;

            if(fuel < 0)
                fuel = 0;
        }

        // TAKEOFF / FLYING
        if(!landing)
        {
            // Aircraft climbs
            if(aircraftY < 0.45f)
            {
                aircraftY += 0.0025f;
                altitude += 8.0f;
            }
            else
            {
                // Aircraft moves horizontally
                aircraftX += 0.002f + (speed / 100000.0f);

                if(aircraftX > 1.10f)
                    aircraftX = -1.10f;
            }
        }

        // LANDING
        if(landing)
        {
            if(aircraftY > -0.48f)
            {
                aircraftY -= 0.003f;

                if(altitude > 0)
                    altitude -= 10.0f;
            }
            else
            {
                aircraftY = -0.48f;
                altitude = 0;

                speed = 0;
                simulationRunning = false;
                landing = false;
            }
        }

        if(fuel <= 0)
        {
            speed = 0;
            paused = true;
        }
    }

    glutPostRedisplay();

    glutTimerFunc(30, update, 0);
}

// =====================================================
// KEYBOARD CONTROLS
// =====================================================

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case ' ':
            takeoff();
            break;

        case 'p':
        case 'P':
            paused = !paused;
            break;

        case 'r':
        case 'R':
            resetAircraft();
            break;

        case 'l':
        case 'L':
            landAircraft();
            break;

        case 'w':
        case 'W':
            altitude += 50;

            if(aircraftY < 0.75f)
                aircraftY += 0.02f;

            break;

        case 's':
        case 'S':
            if(altitude > 0)
                altitude -= 50;

            if(aircraftY > -0.48f)
                aircraftY -= 0.02f;

            break;

        case 'a':
        case 'A':
            aircraftX -= 0.03f;
            break;

        case 'd':
        case 'D':
            aircraftX += 0.03f;
            break;

        case 27:
            exit(0);
    }

    glutPostRedisplay();
}

// =====================================================
// SPECIAL KEYBOARD
// =====================================================

void specialKeyboard(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_UP:

            speed += 10;

            if(speed > 500)
                speed = 500;

            break;

        case GLUT_KEY_DOWN:

            speed -= 10;

            if(speed < 0)
                speed = 0;

            break;

        case GLUT_KEY_LEFT:

            aircraftX -= 0.03f;
            break;

        case GLUT_KEY_RIGHT:

            aircraftX += 0.03f;
            break;
    }

    glutPostRedisplay();
}

// =====================================================
// MOUSE BUTTONS
// =====================================================

void mouse(int button, int state, int mouseX, int mouseY)
{
    if(button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
        return;

    // Convert mouse coordinates to OpenGL coordinates

    float x = (2.0f * mouseX / windowWidth) - 1.0f;

    float y = 1.0f - (2.0f * mouseY / windowHeight);

    // TAKEOFF
    if(x >= -0.92f && x <= -0.68f &&
       y >= -0.98f && y <= -0.88f)
    {
        takeoff();
    }

    // LAND
    else if(x >= -0.64f && x <= -0.45f &&
            y >= -0.98f && y <= -0.88f)
    {
        landAircraft();
    }

    // PAUSE
    else if(x >= -0.41f && x <= -0.20f &&
            y >= -0.98f && y <= -0.88f)
    {
        paused = !paused;
    }

    // RESET
    else if(x >= -0.16f && x <= 0.05f &&
            y >= -0.98f && y <= -0.88f)
    {
        resetAircraft();
    }

    // SPEED+
    else if(x >= 0.10f && x <= 0.28f &&
            y >= -0.98f && y <= -0.88f)
    {
        speed += 10;

        if(speed > 500)
            speed = 500;
    }

    // SPEED-
    else if(x >= 0.33f && x <= 0.51f &&
            y >= -0.98f && y <= -0.88f)
    {
        speed -= 10;

        if(speed < 0)
            speed = 0;
    }

    // ALT+
    else if(x >= 0.56f && x <= 0.69f &&
            y >= -0.98f && y <= -0.88f)
    {
        altitude += 50;

        if(aircraftY < 0.75f)
            aircraftY += 0.02f;
    }

    // ALT-
    else if(x >= 0.74f && x <= 0.87f &&
            y >= -0.98f && y <= -0.88f)
    {
        if(altitude > 0)
            altitude -= 50;

        if(aircraftY > -0.48f)
            aircraftY -= 0.02f;
    }

    glutPostRedisplay();
}

// =====================================================
// DISPLAY
// =====================================================

void display()
{
    // Sky
    glClearColor(0.35f, 0.70f, 0.95f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);

    // Clouds
    drawCloud(-0.65f, 0.65f);
    drawCloud(0.65f, 0.70f);

    // Airport
    drawTerminal();
    drawControlTower();
    drawRunway();

    // Trees
    drawTree(-0.85f, -0.52f);
    drawTree(-0.70f, -0.52f);
    drawTree(0.70f, -0.52f);
    drawTree(0.85f, -0.52f);

    // Dynamic Aircraft
    drawAirplane(aircraftX, aircraftY);

    // Main title
    glColor3f(0.05f, 0.05f, 0.10f);

    drawText(-0.30f, 0.78f, "AERODESK AIRPORT");

    drawText(-0.50f, -0.40f, "TERMINAL");

    drawText(0.35f, -0.40f, "CONTROL TOWER");

    drawText(-0.15f, -0.78f, "RUNWAY 01");

    // Flight number
    char flightText[50];

    sprintf(flightText, "AI202");

    glColor3f(0.05f, 0.05f, 0.10f);

    drawText(aircraftX - 0.05f, aircraftY + 0.12f, flightText);

    // Control panel
    drawControlPanel();

    // Buttons
    drawControls();

    glFlush();
}

// =====================================================
// RESHAPE
// =====================================================

void reshape(int width, int height)
{
    windowWidth = width;
    windowHeight = height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
}

// =====================================================
// MAIN
// =====================================================

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(1000, 700);

    glutInitWindowPosition(100, 50);

    glutCreateWindow("AeroDesk Airport - Dynamic Flight Simulation");

    // Projection
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    // Callbacks
    glutDisplayFunc(display);

    glutReshapeFunc(reshape);

    glutKeyboardFunc(keyboard);

    glutSpecialFunc(specialKeyboard);

    glutMouseFunc(mouse);

    // Animation
    glutTimerFunc(30, update, 0);

    glutMainLoop();

    return 0;
}