// Summer Background: Sun + Clouds Move Only
#include <GL/glut.h>
#include <GL/glu.h>
#include <cmath>
#include <vector>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const int WINDOW_WIDTH = 900;

const int WINDOW_HEIGHT = 600;

// Sun movement
float sunX = -50.0f;
float sunSpeed = 1.0f;

// Clouds
float cloudX[3];

// === Goldilocks Player ===
GLuint playerTexture;   // Goldilocks texture
float playerX;          // X position
float playerY;          // Y position
float playerWidth = 65;  // adjust size
float playerHeight = 95; // adjust size



// === Drawing House / Trees ===
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

// === Parabolic Sun ===
void drawSun(float x) {
    float h = WINDOW_WIDTH / 2.0f;
    float k = 520;
    float a = -0.0015f;
    float y = a * (x - h) * (x - h) + k;

    drawCircle(x, y, 50, 60, 1.0f, 0.9f, 0.0f);

    for (int i = 0; i < 12; i++) {
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

// === Cloud ===
void drawCloud(float x, float y) {
    drawCircle(x, y, 30, 20, 1, 1, 1);
    drawCircle(x + 25, y + 10, 25, 20, 1, 1, 1);
    drawCircle(x - 25, y + 10, 25, 20, 1, 1, 1);
    drawCircle(x, y + 15, 28, 20, 1, 1, 1);
}

// === Tree ===
void drawTree(float x, float y) {
    // trunk
    drawRectangle(x, y, 35, 120, 0.55f, 0.27f, 0.07f);

    float lx = x + 18;
    float ly = y + 110;

    // leaves
    drawCircle(lx, ly, 55, 30, 0.0f, 0.7f, 0.1f);
    drawCircle(lx - 35, ly - 10, 45, 30, 0.0f, 0.75f, 0.05f);
    drawCircle(lx + 35, ly - 10, 45, 30, 0.0f, 0.75f, 0.05f);
    drawCircle(lx, ly + 30, 50, 30, 0.0f, 0.8f, 0.1f);
}


// === Text Narration ===
void drawText(const char* text, float x, float y) {
    glColor3f(0.0f, 0.0f, 0.0f); // black
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}


// === Player (Goldilocks) ===
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
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 4); // RGBA
    if (!data) {
        printf("Failed to load image: %s\n", filename);
        return;
    }

    glGenTextures(1, &playerTexture);
    glBindTexture(GL_TEXTURE_2D, playerTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}

// === Display Summer Scene ===
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Sky
    drawRectangle(0, 120, WINDOW_WIDTH, 480, 0.46f, 0.92f, 0.96f);

    // Sun
    drawSun(sunX);

    // Ground
    drawRectangle(0, 0, WINDOW_WIDTH, 120, 0.0f, 0.75f, 0.29f);

    // Clouds
    drawCloud(cloudX[0], 500);
    drawCloud(cloudX[1], 550);
    drawCloud(cloudX[2], 480);

    // Narration text
    const char* narration = "Goldilocks was wandering around the woods hungry when she stumbled upon a hut.";
    int textWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)narration);
    drawText(narration, (WINDOW_WIDTH - textWidth) / 2.0f, WINDOW_HEIGHT - 50);


    // House
    drawRectangle(120, 120, 150, 100, 0.98f, 0.76f, 0.29f);
    drawTriangle(100, 220, 290, 220, 195, 300, 0.45f, 0.17f, 0.02f);
    drawRectangle(180, 120, 40, 70, 0.05f, 0.05f, 0.05f);

    // Trees
    drawTree(600, 120);
    drawTree(700, 120);
    drawTree(800, 120);

    // Goldilocks
    drawPlayer(playerX, playerY, playerWidth, playerHeight);

    glutSwapBuffers();
}

// === Update only sun + clouds ===
void update(int value) {
    sunX += sunSpeed;
    if (sunX > WINDOW_WIDTH + 50) sunX = -50.0f;

    for (int i = 0; i < 3; i++) {
        cloudX[i] += 0.5f + i * 0.2f;
        if (cloudX[i] > WINDOW_WIDTH + 50) cloudX[i] = -50.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// === Init ===
void initGL() {

    //Background color
    glClearColor(0.46f, 0.92f, 0.96f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);

    //Clouds
    for (int i = 0; i < 3; i++)
        cloudX[i] = rand() % WINDOW_WIDTH;

    // Goldilocks
    stbi_set_flip_vertically_on_load(true);  // Flip the image
    playerX = 700;  // Place inside visible area
    playerY = 120;
    loadPlayerTexture("C:\\Users\\Luis\\Documents\\openglcpp\\Final Project\\assets\\goldilocks.png"); //Change this to the exact file path of the image




}

// === Main ===
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Summer Background - Moving Sun & Clouds");

    initGL();
    glutDisplayFunc(display);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
