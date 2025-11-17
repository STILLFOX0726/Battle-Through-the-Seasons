#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Cloud positions
float cloudX[3] = { 0.5f, 0.7f, 0.9f };
float cloudSpeed[3] = { 0.002f, 0.0015f, 0.0025f };

// === Goldilocks Player ===
GLuint playerTexture;
float playerX;       // X position
float playerY;       // Y position
float playerWidth;   // width
float playerHeight;  // height

enum GoldiState { ENTERING, PAUSE, WALKING, DONE };
GoldiState goldiState = ENTERING;

bool showText = false;

// Draw a single cloud
void drawCloud(float x, float y) {
    glColor3f(1.0f, 1.0f, 1.0f);

    auto drawCircle = [](float cx, float cy, float r) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= 20; i++) {
            float angle = i * 2 * 3.1415926f / 20;
            glVertex2f(cx + cos(angle) * r, cy + sin(angle) * r);
        }
        glEnd();
        };

    drawCircle(x, y, 0.05f);
    drawCircle(x + 0.06f, y + 0.02f, 0.04f);
    drawCircle(x - 0.06f, y + 0.01f, 0.03f);
    drawCircle(x, y + 0.04f, 0.045f);
}

// Load Goldilocks texture
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

// Draw Goldilocks
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

// Text function
void drawText(const char* text, float x, float y) {
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

// Display Scene 2
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Sky
    glColor3f(0.459f, 0.922f, 0.961f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    // Clouds
    drawCloud(cloudX[0], 0.8f);
    drawCloud(cloudX[1], 0.9f);
    drawCloud(cloudX[2], 0.85f);

    // Ground
    glColor3f(0.0f, 0.749f, 0.290f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, -0.4f);
    glVertex2f(-1.0f, -0.4f);
    glEnd();

    // House Body
    glColor3f(0.980f, 0.761f, 0.290f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -0.4f);
    glVertex2f(-0.3f, -0.4f);
    glVertex2f(-0.3f, 0.7f);
    glVertex2f(-1.0f, 0.7f);
    glEnd();

    // Roof
    glColor3f(0.451f, 0.169f, 0.02f);
    glBegin(GL_POLYGON);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(-0.3f, 1.0f);
    glVertex2f(-0.1f, 0.7f);
    glVertex2f(-1.0f, 0.7f);
    glEnd();

    // Door
    glColor3f(0.051f, 0.051f, 0.051f);
    glBegin(GL_QUADS);
    glVertex2f(-0.9f, 0.35f);
    glVertex2f(-0.55f, 0.35f);
    glVertex2f(-0.55f, -0.4f);
    glVertex2f(-0.9f, -0.4f);
    glEnd();

    // Draw text if active
    if (showText) {
        drawText("I'm so hungry!", playerX - 0.05f, playerY + 0.75f);
    }

    // Draw Goldilocks if not DONE
    if (goldiState != DONE) {
        drawPlayer(playerX, playerY, playerWidth, playerHeight);
    }

    glutSwapBuffers();
}

// Hide text after 2s
void hideText(int value) {
    showText = false;
    goldiState = WALKING; // Start walking towards house
}

// Update clouds and Goldilocks
void update(int value) {
    // Move clouds
    for (int i = 0; i < 3; i++) {
        cloudX[i] += cloudSpeed[i];
        if (cloudX[i] > 1.2f) cloudX[i] = -1.2f;
    }

    // Goldilocks animation states
    if (goldiState == ENTERING) {
        float enterTargetX = 0.6f;
        float speed = 0.007f; // faster entry
        if (playerX > enterTargetX) {
            playerX -= speed;
        }
        else {
            goldiState = PAUSE;
            showText = true;
            // Pause 2s before walking
            glutTimerFunc(2000, hideText, 0);
        }
    }
    else if (goldiState == WALKING) {
        float doorX = -0.55f - playerWidth - 0.05f;
        float speed = 0.008f;
        if (playerX > doorX) {
            playerX -= speed;
        }
        else {
            goldiState = DONE;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// Init GL
void initGL() {
    glClearColor(0.46f, 0.92f, 0.96f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    // Clouds random positions
    for (int i = 0; i < 3; i++)
        cloudX[i] = (float)(rand() % 2000) / 1000.0f - 1.0f;

    // Goldilocks 
    stbi_set_flip_vertically_on_load(true);
    playerX = 1.2f;
    playerY = -0.6f;
    playerWidth = 0.3f;
    playerHeight = 0.7f;
    loadPlayerTexture("C:\\Users\\Luis\\Documents\\openglcpp\\Final Project\\assets\\goldilocks.png");
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Scene 2 - House with Goldilocks");

    initGL();
    glutDisplayFunc(display);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
