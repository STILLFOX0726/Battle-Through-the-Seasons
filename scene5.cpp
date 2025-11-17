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

// === Textures ===
GLuint bearTexture;

// Height & Width
float bearWidth = 750;
float bearHeight = 420;

// === Animation variables for text ===
int textStep = 0; // 0: no text, 1: first text, 2: second, 3: third

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

void loadTexture(const char* filename, GLuint& texture) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 4);
    if (!data) {
        printf("Failed to load image: %s\n", filename);
        return;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}

void drawTexture(GLuint texture, float x, float y, float w, float h) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(x, y);
    glTexCoord2f(1, 0); glVertex2f(x + w, y);
    glTexCoord2f(1, 1); glVertex2f(x + w, y + h);
    glTexCoord2f(0, 1); glVertex2f(x, y + h);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// Draw text at given position
void drawText(const char* text, float x, float y) {
    glColor3f(0, 0, 0);
    glRasterPos2f(x, y);
    for (int i = 0; text[i]; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

// === Interior Scene ===
void drawInteriorScene() {
    // Walls
    drawRectangle(0, 80, WINDOW_WIDTH, WINDOW_HEIGHT - 80, 0.98f, 0.76f, 0.29f);
    // Floor
    drawRectangle(0, 0, WINDOW_WIDTH, 80, 0.55f, 0.27f, 0.07f);

    // Center the bear
    float bearX = (WINDOW_WIDTH - bearWidth) / 2.0f;
    float bearY = 80;
    drawTexture(bearTexture, bearX, bearY, bearWidth, bearHeight);

    // Draw text based on step
    if (textStep >= 1) {
        drawText("Oyy you ate all our porridges!", 110, 400);
    }
    if (textStep >= 2) {
        const char* text2 = "Now, you must serve us for a year!";
        float textWidth2 = strlen(text2) * 9; // 
        drawText(text2, (WINDOW_WIDTH - textWidth2) / 1.6, 370);
    }
    if (textStep >= 3) {
        const char* text3 = "Mess up, and we'll eat you!";
        float textWidth3 = strlen(text3) * 9;
        drawText(text3, WINDOW_WIDTH - textWidth3 - 50, 300);
    }
}


// === Timer callback for text animation ===
void timer(int value) {
    if (textStep < 3) {
        textStep++;
        glutPostRedisplay();
        glutTimerFunc(1500, timer, 0); // 1 second delay
    }
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);

    loadTexture("C:\\Users\\Luis\\Documents\\openglcpp\\Final Project\\assets\\angry_bears.png", bearTexture);
}

// === Main ===
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Scene 5 - Bears Focus with Text");

    initGL();
    glutDisplayFunc(display);
    glutTimerFunc(500, timer, 0);
    glutMainLoop();
    return 0;
}
