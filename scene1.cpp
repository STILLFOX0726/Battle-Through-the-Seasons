#include <GL/glut.h>
#include <GL/glu.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 600;

enum GoldiState { ENTERING, PAUSED, SHOW_TEXT, TO_DOOR };
GoldiState goldiState = ENTERING;
int goldiTimer = 0; // counts frames (~60 per second)


// --- Menu ---
bool menuActive = true;

// --- Sun and Clouds ---
float sunX = -50.0f;
float sunSpeed = 1.0f;
float cloudX[3];

// --- Goldilocks ---
GLuint playerTexture;
float playerX, playerY;
float playerWidth = 65, playerHeight = 95;

// --- Utility Functions ---
void drawRectangle(float x, float y, float w, float h, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y); glVertex2f(x + w, y);
    glVertex2f(x + w, y + h); glVertex2f(x, y + h);
    glEnd();
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1); glVertex2f(x2, y2); glVertex2f(x3, y3);
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

// --- Clouds ---
void drawCloud(float x, float y) {
    drawCircle(x, y, 30, 20, 1, 1, 1);
    drawCircle(x + 25, y + 10, 25, 20, 1, 1, 1);
    drawCircle(x - 25, y + 10, 25, 20, 1, 1, 1);
    drawCircle(x, y + 15, 28, 20, 1, 1, 1);
}

// --- Trees ---
void drawTree(float x, float y) {
    drawRectangle(x, y, 35, 120, 0.55f, 0.27f, 0.07f);
    float lx = x + 18;
    float ly = y + 110;
    drawCircle(lx, ly, 55, 30, 0.0f, 0.7f, 0.1f);
    drawCircle(lx - 35, ly - 10, 45, 30, 0.0f, 0.75f, 0.05f);
    drawCircle(lx + 35, ly - 10, 45, 30, 0.0f, 0.75f, 0.05f);
    drawCircle(lx, ly + 30, 50, 30, 0.0f, 0.8f, 0.1f);
}

// --- Text ---
void drawText(const char* text, float x, float y) {
    glColor3f(0, 0, 0);
    glRasterPos2f(x, y);
    for (int i = 0;text[i];i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}

// --- Player (Goldilocks) ---
void drawPlayer(float x, float y, float w, float h) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, playerTexture);
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(x, y);
    glTexCoord2f(1, 0); glVertex2f(x + w, y);
    glTexCoord2f(1, 1); glVertex2f(x + w, y + h);
    glTexCoord2f(0, 1); glVertex2f(x, y + h);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void loadPlayerTexture(const char* filename) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 4);
    if (!data) { printf("Failed to load %s\n", filename); return; }
    glGenTextures(1, &playerTexture);
    glBindTexture(GL_TEXTURE_2D, playerTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
}

// --- Sun ---
void drawSun(float x) {
    float h = WINDOW_WIDTH / 2.0f, k = 520, a = -0.0015f;
    float y = a * (x - h) * (x - h) + k;
    drawCircle(x, y, 50, 60, 1.0f, 0.9f, 0.0f);
    for (int i = 0;i < 12;i++) {
        float ang = i * 2 * M_PI / 12;
        float x1 = x + cos(ang) * 60, y1 = y + sin(ang) * 60;
        float x2 = x + cos(ang + 0.2f) * 75, y2 = y + sin(ang + 0.2f) * 75;
        float x3 = x + cos(ang - 0.2f) * 75, y3 = y + sin(ang - 0.2f) * 75;
        drawTriangle(x1, y1, x2, y2, x3, y3, 1.0f, 0.8f, 0.0f);
    }
}

// --- Menu Drawing ---
void drawMenu() {
    // Background
    drawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0.46f, 0.92f, 0.96f);
    drawRectangle(0, 0, WINDOW_WIDTH, 120, 0.0f, 0.75f, 0.29f);

    // Clouds
    drawCloud(cloudX[0], 500);
    drawCloud(cloudX[1], 550);

    // House & trees
    drawRectangle(120, 120, 150, 120, 0.98f, 0.76f, 0.29f);
    drawTriangle(120, 240, 270, 240, 195, 300, 0.45f, 0.17f, 0.02f);
    drawRectangle(180, 120, 40, 70, 0.05f, 0.05f, 0.05f);
    drawTree(600, 120);
    drawTree(700, 120);
    drawTree(800, 120);

    // Buttons
    drawRectangle(350, 300, 200, 60, 1.0f, 0.0f, 0.0f); drawText("PLAY", 425, 335);
    drawRectangle(350, 200, 200, 60, 1.0f, 0.0f, 0.0f); drawText("EXIT", 425, 230);
    drawText("Battle Through the Seasons", 335, 425);
}

// --- Summer Scene Display ---
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (menuActive) {
        drawMenu();
    }
    else {
        // Sky and ground
        drawRectangle(0, 120, WINDOW_WIDTH, 480, 0.46f, 0.92f, 0.96f);
        drawSun(sunX);
        drawRectangle(0, 0, WINDOW_WIDTH, 120, 0.0f, 0.75f, 0.29f);

        // Clouds
        drawCloud(cloudX[0], 500);
        drawCloud(cloudX[1], 550);
        drawCloud(cloudX[2], 480);

        // House
        drawRectangle(120, 120, 150, 100, 0.98f, 0.76f, 0.29f);
        drawTriangle(100, 220, 290, 220, 195, 300, 0.45f, 0.17f, 0.02f);
        drawRectangle(180, 120, 40, 70, 0.05f, 0.05f, 0.05f);

        // Trees
        drawTree(600, 120); drawTree(700, 120); drawTree(800, 120);

        // Goldilocks
        drawPlayer(playerX, playerY, playerWidth, playerHeight);

        // Narration
        const char* narration = "Goldilocks was wandering around the woods hungry when she stumbled upon a hut.";
        int textWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)narration);
        drawText(narration, (WINDOW_WIDTH - textWidth) / 2.0f, WINDOW_HEIGHT - 50);
    }

    glutSwapBuffers();
}

// --- Update ---
void update(int value) {
    if (!menuActive) {
        sunX += sunSpeed; if (sunX > WINDOW_WIDTH + 50) sunX = -50.0f;
        for (int i = 0;i < 3;i++) { cloudX[i] += 0.5f + i * 0.2f; if (cloudX[i] > WINDOW_WIDTH + 50) cloudX[i] = -50.0f; }

        // Goldilocks movement
        float doorX = 180 + 20;
        float speed = 1.2f;
        if (playerX > doorX - playerWidth / 2) playerX -= speed;
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// --- Mouse ---
void mouse(int button, int state, int x, int y) {
    if (menuActive && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        y = WINDOW_HEIGHT - y; // invert Y
        // Play
        if (x >= 350 && x <= 550 && y >= 300 && y <= 360) { menuActive = false; glutPostRedisplay(); }
        // Exit
        if (x >= 350 && x <= 550 && y >= 200 && y <= 260) exit(0);
    }
}

// --- Init ---
void initGL() {
    glClearColor(0.46f, 0.92f, 0.96f, 1.0f);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);

    for (int i = 0;i < 3;i++) cloudX[i] = rand() % WINDOW_WIDTH;

    playerX = WINDOW_WIDTH + 50; playerY = 120;
    stbi_set_flip_vertically_on_load(true);
    loadPlayerTexture("C:\\Users\\Luis\\Documents\\openglcpp\\Final Project\\assets\\goldilocks.png");
}

// --- Main ---
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Battle Through the Seasons");
    initGL();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
