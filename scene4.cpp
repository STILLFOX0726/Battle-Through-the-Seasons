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
GLuint bearTexture, goldiTexture;

//Height & Width
float bearWidth = 600, bearHeight = 350;
float goldiWidth = 100, goldiHeight = 150;

// === Animation variables ===
float bearX = -bearWidth;       // start off-screen left
const float bearFinalX = 20;    // target position
float goldiX = WINDOW_WIDTH - goldiWidth - 130;
float goldiY = 80;
float bearY = 80;
bool bearsStopped = false;       // flag to show text

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

// Draw Text
void drawText(const char* text, float y) {
    glColor3f(0, 0, 0);
    int textWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)text);
    float textX = (WINDOW_WIDTH - textWidth) / 2.0f;
    glRasterPos2f(textX, y);
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

    // Draw bears and Goldilocks
    drawTexture(bearTexture, bearX, bearY, bearWidth, bearHeight);
    drawTexture(goldiTexture, goldiX, goldiY, goldiWidth, goldiHeight);

    // Draw text only if bears stopped
    if (bearsStopped) {
        drawText("Suddenly, the three bears woke up and were furious.", WINDOW_HEIGHT - 100);
    }

}

// === Timer callback for animation ===
void timer(int value) {
    // Move bears right until they reach final position
    if (bearX < bearFinalX) {
        bearX += 6; // slower speed
        if (bearX >= bearFinalX) {
            bearX = bearFinalX;
            bearsStopped = true; // show text now
        }
    }
    glutPostRedisplay(); // always redraw
    glutTimerFunc(30, timer, 0); // call again in 30 ms
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
    loadTexture("C:\\Users\\Luis\\Documents\\openglcpp\\Final Project\\assets\\goldilocks.png", goldiTexture);
}

// === Main ===
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Scene 4 - Bears and Goldilocks");

    initGL();
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0); // start animation
    glutMainLoop();
    return 0;
}
