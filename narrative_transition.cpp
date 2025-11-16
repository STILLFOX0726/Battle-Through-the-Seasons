#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 600;

// === Scene Control ===
int currentScene = 1; // 1 = Summer, 2 = House, 3 = Interior
int frameCounter = 0; // used for timing scene switch

// === Scene 1: Summer Scene Variables ===
float sunX = -50.0f;
float sunSpeed = 1.0f;
float cloudX1[3];

// === Scene 2: Clouds variables ===
float cloudX2[3] = { 0.5f, 0.7f, 0.9f };
float cloudSpeed2[3] = { 0.002f, 0.0015f, 0.0025f };

// === Drawing Utilities ===
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
    for (int i = 0; i <= segments; i++) {
        float ang = i * 2 * M_PI / segments;
        glVertex2f(cx + cos(ang) * r, cy + sin(ang) * r);
    }
    glEnd();
}

// === Bowl Utility ===
void drawBowl(float cx, float cy, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 20; i++) {
        float ang = M_PI * i / 20.0f; // half circle
        glVertex2f(cx + cos(ang) * radius, cy - sin(ang) * radius);
    }
    glEnd();
}

// === Tree Utility ===
void drawTree(float x, float y) {
    drawRectangle(x, y, 35, 120, 0.55f, 0.27f, 0.07f);
    float lx = x + 18;
    float ly = y + 110;
    drawCircle(lx, ly, 55, 30, 0.0f, 0.7f, 0.1f);
    drawCircle(lx - 35, ly - 10, 45, 30, 0.0f, 0.75f, 0.05f);
    drawCircle(lx + 35, ly - 10, 45, 30, 0.0f, 0.75f, 0.05f);
    drawCircle(lx, ly + 30, 50, 30, 0.0f, 0.8f, 0.1f);
}

// === Scene 1 Drawing ===
void drawScene1() {
    // Sky
    drawRectangle(0, 120, WINDOW_WIDTH, 480, 0.46f, 0.92f, 0.96f);

    // Sun
    float h = WINDOW_WIDTH / 2.0f;
    float k = 520;
    float a = -0.0015f;
    float y = a * (sunX - h) * (sunX - h) + k;
    drawCircle(sunX, y, 50, 60, 1.0f, 0.9f, 0.0f);
    for (int i = 0; i < 12; i++) {
        float ang = i * (2 * M_PI / 12);
        float x1 = sunX + cos(ang) * 60;
        float y1 = y + sin(ang) * 60;
        float x2 = sunX + cos(ang + 0.2f) * 75;
        float y2 = y + sin(ang + 0.2f) * 75;
        float x3 = sunX + cos(ang - 0.2f) * 75;
        float y3 = y + sin(ang - 0.2f) * 75;
        drawTriangle(x1, y1, x2, y2, x3, y3, 1.0f, 0.8f, 0.0f);
    }

    // Clouds
    for (int i = 0; i < 3; i++) {
        drawCircle(cloudX1[i], 500, 30, 20, 1, 1, 1);
        drawCircle(cloudX1[i] + 25, 510, 25, 20, 1, 1, 1);
        drawCircle(cloudX1[i] - 25, 510, 25, 20, 1, 1, 1);
        drawCircle(cloudX1[i], 515, 28, 20, 1, 1, 1);
    }

    // Ground
    drawRectangle(0, 0, WINDOW_WIDTH, 120, 0.0f, 0.75f, 0.29f);

    // House
    //House Base
    drawRectangle(120, 120, 150, 100, 0.98f, 0.76f, 0.29f);

    // House Roof
    drawTriangle(100, 220, 290, 220, 195, 300, 0.45f, 0.17f, 0.02f);

    //Door
    drawRectangle(180, 120, 40, 70, 0.05f, 0.05f, 0.05f);

    // Trees
    drawTree(600, 120);
    drawTree(700, 120);
    drawTree(800, 120);
}

// === Scene 2 Drawing ===
void drawScene2() {
    // Switch to normalized coordinates for Scene 2
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();        // save current projection
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT);

    // === Sky background ===
    glColor3f(0.459f, 0.922f, 0.961f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    // === Clouds ===
    auto drawCloud = [](float x, float y) {
        glColor3f(1, 1, 1);
        auto drawCircle = [](float cx, float cy, float r) {
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(cx, cy);
            for (int i = 0; i <= 20; i++) {
                float angle = i * 2 * M_PI / 20;
                glVertex2f(cx + cos(angle) * r, cy + sin(angle) * r);
            }
            glEnd();
            };
        drawCircle(x, y, 0.05f);
        drawCircle(x + 0.06f, y + 0.02f, 0.04f);
        drawCircle(x - 0.06f, y + 0.01f, 0.03f);
        drawCircle(x, y + 0.04f, 0.045f);
        };

    drawCloud(cloudX2[0], 0.8f);
    drawCloud(cloudX2[1], 0.9f);
    drawCloud(cloudX2[2], 0.85f);

    // === Ground ===
    glColor3f(0.0f, 0.749f, 0.290f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, -0.4f);
    glVertex2f(-1.0f, -0.4f);
    glEnd();

    // === House Body ===
    glColor3f(0.980f, 0.761f, 0.290f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -0.4f);
    glVertex2f(-0.3f, -0.4f);
    glVertex2f(-0.3f, 0.7f);
    glVertex2f(-1.0f, 0.7f);
    glEnd();

    // === Roof ===
    glColor3f(0.451f, 0.169f, 0.02f);
    glBegin(GL_POLYGON);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(-0.3f, 1.0f);
    glVertex2f(-0.1f, 0.7f);
    glVertex2f(-1.0f, 0.7f);
    glEnd();

    // === Door ===
    glColor3f(0.051f, 0.051f, 0.051f);
    glBegin(GL_QUADS);
    glVertex2f(-0.9f, 0.35f);
    glVertex2f(-0.55f, 0.35f);
    glVertex2f(-0.55f, -0.4f);
    glVertex2f(-0.9f, -0.4f);
    glEnd();

    // Restore original pixel projection
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}


// === Scene 3 Drawing ===
void drawScene3() {
    // Walls and floor
    drawRectangle(0, 80, WINDOW_WIDTH, WINDOW_HEIGHT - 80, 0.98f, 0.76f, 0.29f);
    drawRectangle(0, 0, WINDOW_WIDTH, 80, 0.55f, 0.27f, 0.07f);

    // Window
    drawRectangle(375, 350, 150, 150, 0.6f, 0.85f, 0.95f);
    drawRectangle(385, 360, 130, 130, 0.8f, 0.95f, 1.0f);
    drawRectangle(450, 360, 5, 130, 0.6f, 0.85f, 0.95f);
    drawRectangle(385, 425, 130, 5, 0.6f, 0.85f, 0.95f);

    // Table
    drawRectangle(220, 160, 460, 25, 0.55f, 0.27f, 0.07f);
    drawRectangle(250, 80, 25, 80, 0.55f, 0.27f, 0.07f);
    drawRectangle(605, 80, 25, 80, 0.55f, 0.27f, 0.07f);

    // Bowls
    drawBowl(320, 220, 35, 1.0f, 0.0f, 0.0f);
    drawBowl(450, 220, 35, 0.0f, 0.0f, 1.0f);
    drawBowl(580, 220, 35, 0.82f, 0.71f, 0.55f);
}

// === Display ===
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (currentScene == 1) drawScene1();
    else if (currentScene == 2) drawScene2();
    else if (currentScene == 3) drawScene3();

    glutSwapBuffers();
}

// === Update ===
void update(int value) {
    frameCounter++;

    // Scene 1 updates
    if (currentScene == 1) {
        sunX += sunSpeed;
        if (sunX > WINDOW_WIDTH + 50) sunX = -50.0f;
        for (int i = 0;i < 3;i++) {
            cloudX1[i] += 0.5f + i * 0.2f;
            if (cloudX1[i] > WINDOW_WIDTH + 50) cloudX1[i] = -50.0f;
        }
    }
    // Scene 2 updates
    if (currentScene == 2) {
        for (int i = 0;i < 3;i++) {
            cloudX2[i] += cloudSpeed2[i];
            if (cloudX2[i] > 1.2f) cloudX2[i] = -1.2f;
        }
    }

    // Switch scene every ~10 seconds (~600 frames at 60 FPS)
    if (frameCounter > 600) {
        currentScene++;
        if (currentScene > 3) currentScene = 3; // stop at last scene
        frameCounter = 0;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// === Init ===
void initGL() {
    glClearColor(0.46f, 0.92f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);

    // Initialize Scene1 clouds
    for (int i = 0;i < 3;i++)
        cloudX1[i] = rand() % WINDOW_WIDTH;
}

// === Main ===
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Connected Scenes");

    initGL();
    glutDisplayFunc(display);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
