#include <GL/glut.h>
#include <cmath>
#include <cstring>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 600;

// === Goldilocks Player ===
GLuint playerTexture;
float playerX;      // X position
float playerY;      // Y position
float playerWidth;  // width
float playerHeight; // height

// === Animation States ===
int state = 0; // 0=enter,1=move to red,2=move to blue,3=move to brown, 10-13 = text wait states
float speed = 2.2f;
bool showText = false;
std::string currentText = "";

// Text position
float textX = 0;
float textY = 0;

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

void drawBowl(float cx, float cy, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 20; i++) {
        float ang = M_PI * i / 20.0f;
        glVertex2f(cx + cos(ang) * radius, cy - sin(ang) * radius);
    }
    glEnd();
}

// === Load Goldilocks Texture ===
void loadPlayerTexture(const char* filename) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 4);
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

// === Draw Goldilocks ===
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

// === Draw Text ===
void drawText(const char* text, float x, float y) {
    glColor3f(0, 0, 0);
    glRasterPos2f(x, y);
    for (int i = 0; text[i]; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

// === Interior Scene ===
void drawInteriorScene() {
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
    drawBowl(320, 220, 35, 1.0f, 0.0f, 0.0f);  // red
    drawBowl(450, 220, 35, 0.0f, 0.0f, 1.0f);  // blue
    drawBowl(580, 220, 35, 0.82f, 0.71f, 0.55f); // brown

    // Goldilocks
    drawPlayer(playerX, playerY, playerWidth, playerHeight);

    // Draw current text if any
    if (showText) {
        drawText(currentText.c_str(), textX, textY);
    }
}

// === Animation Update ===
void update(int value) {
    switch (state) {
    case 0: // entering from right
        playerX -= speed;
        if (playerX <= 400) { // stop at table
            showText = true;
            currentText = "Inside was 3 porridges and tried each one. One was hot, one was cold, and the other one was just right.";
            textX = 33;
            textY = 540;
            state = 10; // wait state
            glutPostRedisplay();
            glutTimerFunc(3000, update, 0); // show text 3 seconds
            return;
        }
        break;
    case 1: // move to red bowl
        playerX -= speed;
        if (playerX <= 290) {
            showText = true;
            currentText = "Too Hot!";
            textX = playerX - 90;
            textY = playerY + 200;
            state = 11;
            glutPostRedisplay();
            glutTimerFunc(1500, update, 0);
            return;
        }
        break;
    case 2: // move to blue bowl
        playerX += speed;
        if (playerX >= 430) {
            showText = true;
            currentText = "Too Cold!";
            textX = playerX - 90;
            textY = playerY + 200;
            state = 12;
            glutPostRedisplay();
            glutTimerFunc(1500, update, 0);
            return;
        }
        break;
    case 3: // move to brown bowl
        playerX += speed;
        if (playerX >= 560) {
            showText = true;
            currentText = "Just Right!";
            textX = playerX - 100;
            textY = playerY + 200;
            state = 13;
            glutPostRedisplay();
            return;
        }
        break;
        // Text wait states
    case 10:
        showText = false;
        state = 1; // move to red bowl
        break;
    case 11:
        showText = false;
        state = 2; // move to blue bowl
        break;
    case 12:
        showText = false;
        state = 3; // move to brown bowl
        break;
    case 13:
        // final state, no movement
        break;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// === Display callback ===
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawInteriorScene();
    glutSwapBuffers();
}

// === OpenGL initialization ===
void initGL() {
    glClearColor(0.46f, 0.92f, 1.0f, 1.0f);

    //Enable transparent background for goldi
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);

    stbi_set_flip_vertically_on_load(true);
    playerX = 900;
    playerY = 78;
    playerWidth = 150;
    playerHeight = 250;
    loadPlayerTexture("C:\\Users\\Luis\\Documents\\openglcpp\\Final Project\\assets\\goldilocks.png");
}

// === Main ===
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Scene 3 - Goldilocks Animation");

    initGL();
    glutDisplayFunc(display);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
