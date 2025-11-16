// Four Seasons Animated Scene with Updated House & Trees
#include <GL/glut.h>
#include <GL/glu.h>
#include <cmath>
#include <vector>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 600;

int currentSeason = 3; // 0=spring,1=summer,2=autumn,3=winter

float sunX = -50.0f;      // Sun starting X
float sunSpeed = 1.0f;    // Sun horizontal speed
float cloudX[3];           // Clouds horizontal positions
float fireOffset[5] = { 0 };    // Fire flicker offsets

struct Snowflake { float x, y, size; };
std::vector<Snowflake> snowflakes;

// ===== Utility Drawing Functions =====
void drawRectangle(float x, float y, float w, float h, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

void drawCircle(float cx, float cy, float r, int segments, float cr, float cg, float cb) {
    glColor3f(cr, cg, cb);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0;i <= segments;i++) {
        float ang = i * 2 * M_PI / segments;
        glVertex2f(cx + cos(ang) * r, cy + sin(ang) * r);
    }
    glEnd();
}

// ===== Sun with Parabolic Path =====
void drawSun(float x) {
    float h = WINDOW_WIDTH / 2.0f;
    float k = 520;
    float a = -0.0015f;
    float y = a * (x - h) * (x - h) + k;

    drawCircle(x, y, 50, 60, 1.0f, 0.9f, 0.0f);

    for (int i = 0;i < 12;i++) {
        float ang = i * (2 * M_PI / 12);
        float x1 = x + cos(ang) * 60;
        float y1 = y + sin(ang) * 60;
        float x2 = x + cos(ang + 0.2f) * 75;
        float y2 = y + sin(ang + 0.2f) * 75;
        float x3 = x + cos(ang - 0.2f) * 75;
        float y3 = y + sin(ang - 0.2f) * 75;
        drawTriangle(x1, y1, x2, y2, x3, y3, 1.0f, 0.8f, 0.0f);
    }
}

// ===== Cloud =====
void drawCloud(float x, float y) {
    drawCircle(x, y, 30, 20, 1, 1, 1);
    drawCircle(x + 25, y + 10, 25, 20, 1, 1, 1);
    drawCircle(x - 25, y + 10, 25, 20, 1, 1, 1);
    drawCircle(x, y + 15, 28, 20, 1, 1, 1);
}

// ===== Flowers =====
void drawFlower(float x, float y) {
    float petalR = 10;
    drawCircle(x, y + petalR, petalR, 20, 1, 0.6f, 0.8f);
    drawCircle(x, y - petalR, petalR, 20, 1, 0.6f, 0.8f);
    drawCircle(x + petalR, y, petalR, 20, 1, 0.6f, 0.8f);
    drawCircle(x - petalR, y, petalR, 20, 1, 0.6f, 0.8f);
    drawCircle(x, y, petalR * 0.7f, 20, 1, 1, 0.2f);
}

// ===== Tree =====
void drawTree(float x, float y, bool flowers, bool autumn, bool winter) {
    // trunk
    drawRectangle(x, y, 35, 120, 0.55f, 0.27f, 0.07f);

    float lx = x + 18;
    float ly = y + 110;

    if (winter) {
        drawCircle(lx, ly, 55, 30, 1.0f, 1.0f, 1.0f);

        return;
    }
    if (autumn) {
        drawCircle(lx, ly, 55, 30, 0.929f, 0.608f, 0.125f);
        drawCircle(lx - 35, ly - 10, 45, 30, 0.929f, 0.608f, 0.125f);
        drawCircle(lx + 35, ly - 10, 45, 30, 0.929f, 0.608f, 0.125f);
        drawCircle(lx, ly + 30, 50, 30, 0.95f, 0.65f, 0.2f);

        return;
    }

    // leaves
    drawCircle(lx, ly, 55, 30, 0.0f, 0.7f, 0.1f);
    drawCircle(lx - 35, ly - 10, 45, 30, 0.0f, 0.75f, 0.05f);
    drawCircle(lx + 35, ly - 10, 45, 30, 0.0f, 0.75f, 0.05f);
    drawCircle(lx, ly + 30, 50, 30, 0.0f, 0.8f, 0.1f);

    if (flowers) {
        drawFlower(x + 5, y + 170);
        drawFlower(x + 40, y + 140);
        drawFlower(x + 10, y + 120);
    }
}

// ===== Fire =====
void drawFire(float x, float y, float offset) {
    drawTriangle(x - 20, y, x + 20, y, x, y + 40 + offset, 1.0f, 0.3f, 0.0f);
    drawTriangle(x - 15, y + 20, x + 15, y + 20, x, y + 55 + offset, 1.0f, 0.6f, 0.0f);
    drawTriangle(x - 10, y + 35, x + 10, y + 35, x, y + 65 + offset, 1.0f, 0.9f, 0.0f);
}

// ===== Display =====
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Ground & Sky by Season
    if (currentSeason == 0) { // Spring
        drawRectangle(0, 120, WINDOW_WIDTH, 480, 0.46f, 0.92f, 0.96f);
        drawSun(sunX);
        drawRectangle(0, 0, WINDOW_WIDTH, 120, 0.0f, 0.75f, 0.29f);
    }
    else if (currentSeason == 1) { // Summer
        drawRectangle(0, 120, WINDOW_WIDTH, 480, 0.46f, 0.92f, 0.96f);
        drawSun(sunX);
        drawRectangle(0, 0, WINDOW_WIDTH, 120, 0.0f, 0.75f, 0.29f);
    }
    else if (currentSeason == 2) { // Autumn
        drawRectangle(0, 120, WINDOW_WIDTH, 480, 0.45f, 0.65f, 1.0f);
        drawSun(sunX);
        drawRectangle(0, 0, WINDOW_WIDTH, 120, 0.8588f, 0.5882f, 0.1843f);
    }
    else if (currentSeason == 3) { // Winter
        drawRectangle(0, 120, WINDOW_WIDTH, 480, 0.8f, 0.9f, 1.0f);
        drawSun(sunX);
        drawRectangle(0, 0, WINDOW_WIDTH, 120, 1.0f, 1.0f, 1.0f);
    }

    // Clouds
    drawCloud(cloudX[0], 500);
    drawCloud(cloudX[1], 550);
    drawCloud(cloudX[2], 480);

    // House (updated)
    drawRectangle(120, 120, 150, 100, 0.98f, 0.76f, 0.29f); // base
    drawTriangle(100, 220, 290, 220, 195, 300, 0.45f, 0.17f, 0.02f); // roof
    drawRectangle(180, 120, 40, 70, 0.05f, 0.05f, 0.05f); // door

    // Summer Fire
    if (currentSeason == 1) {
        drawFire(150, 250, fireOffset[0]);
        drawFire(230, 252, fireOffset[1]);
    }

    // Trees (updated)
    drawTree(600, 120, currentSeason == 0, currentSeason == 2, currentSeason == 3);
    drawTree(700, 120, currentSeason == 0, currentSeason == 2, currentSeason == 3);
    drawTree(800, 120, currentSeason == 0, currentSeason == 2, currentSeason == 3);

    // Additional fire near trees (summer)
    if (currentSeason == 1) {
        drawFire(613, 250, fireOffset[2]);
        drawFire(710, 245, fireOffset[3]);
        drawFire(817, 253, fireOffset[4]);
    }

    // Snow for winter
    if (currentSeason == 3) {
        for (auto& s : snowflakes)
            drawCircle(s.x, s.y, s.size, 10, 1, 1, 1);
    }

    // Season label
    glColor3f(0, 0, 0);
    glRasterPos2f(420, 30);
    const char* labels[] = { "Spring Time","Summer Time","Autumn","Winter" };
    const char* text = labels[currentSeason];
    for (int i = 0;text[i];i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);

    glutSwapBuffers();
}

// ===== Keyboard =====
void keyboard(unsigned char key, int x, int y) {
    if (key == 'n') {
        currentSeason = (currentSeason + 1) % 4;
        glutPostRedisplay();
    }
}

// ===== Idle Update =====
void update(int value) {
    // Sun
    sunX += sunSpeed;
    if (sunX > WINDOW_WIDTH + 50) sunX = -50.0f;

    // Clouds
    for (int i = 0;i < 3;i++) {
        cloudX[i] += 0.5f + i * 0.2f;
        if (cloudX[i] > WINDOW_WIDTH + 50) cloudX[i] = -50.0f;
    }

    // Fire flicker
    for (int i = 0;i < 5;i++)
        fireOffset[i] = rand() % 10;

    // Snow
    if (currentSeason == 3) {
        for (auto& s : snowflakes) {
            s.y -= 2;
            if (s.y < 0) s.y = WINDOW_HEIGHT;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// ===== OpenGL Init =====
void initGL() {
    glClearColor(0.46f, 0.92f, 0.96f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);

    // Snowflakes
    for (int i = 0;i < 100;i++)
        snowflakes.push_back({ (float)(rand() % WINDOW_WIDTH), (float)(rand() % WINDOW_HEIGHT), 2 + rand() % 3 });

    // Clouds
    for (int i = 0;i < 3;i++)
        cloudX[i] = rand() % WINDOW_WIDTH;
}

// ===== Main =====
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Four Seasons Animated Scene - Updated House & Trees");

    initGL();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}
