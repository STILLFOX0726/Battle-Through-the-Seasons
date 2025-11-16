#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Window dimensions
const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 600;

// Game constants
const float BASKET_SPEED = 300.0f;
const float BASE_ITEM_FALL_SPEED = 100.0f;
const float BASE_ITEM_SPAWN_INTERVAL = 1.5f;
const int STARTING_LIVES = 3;

enum Season { SPRING, SUMMER, AUTUMN, WINTER };

// ===== Seasonal Scene Variables =====
float sunX = -50.0f;       // Sun starting X
float sunSpeed = 1.0f;     // Sun horizontal speed
float cloudX[3];           // Clouds horizontal positions
float fireOffset[5] = {0}; // Fire flicker offsets

struct Snowflake {
  float x, y, size;
};
std::vector<Snowflake> snowflakes;

// ===== Game Structures =====
struct Item {
  float x, y;
  float size;
  float r, g, b, a;
  bool isGood;
  float velocity;
  std::string type;
};

// ===== Utility Drawing Functions for Seasonal Scene =====
void drawRectangle(float x, float y, float w, float h, float r, float g,
                   float b) {
  glColor3f(r, g, b);
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x + w, y);
  glVertex2f(x + w, y + h);
  glVertex2f(x, y + h);
  glEnd();
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3,
                  float r, float g, float b) {
  glColor3f(r, g, b);
  glBegin(GL_TRIANGLES);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glVertex2f(x3, y3);
  glEnd();
}

void drawCircle(float cx, float cy, float r, int segments, float cr, float cg,
                float cb) {
  glColor3f(cr, cg, cb);
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(cx, cy);
  for (int i = 0; i <= segments; i++) {
    float ang = i * 2 * M_PI / segments;
    glVertex2f(cx + cos(ang) * r, cy + sin(ang) * r);
  }
  glEnd();
}

// ===== Heart Drawing Function =====
void drawHeart(float x, float y, float size, float r, float g, float b) {
  glColor3f(r, g, b);
  glBegin(GL_TRIANGLE_FAN);

  // Main body of the heart
  for (int i = 0; i <= 360; i++) {
    float angle = i * M_PI / 180.0f;
    // Heart parametric equations
    float xPos = size * 16 * pow(sin(angle), 3);
    float yPos = -size * (13 * cos(angle) - 5 * cos(2 * angle) -
                          2 * cos(3 * angle) - cos(4 * angle));

    glVertex2f(x + xPos, y + yPos);
  }
  glEnd();
}

// ===== Seasonal Scene Drawing Functions =====
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

void drawCloud(float x, float y) {
  drawCircle(x, y, 30, 20, 1, 1, 1);
  drawCircle(x + 25, y + 10, 25, 20, 1, 1, 1);
  drawCircle(x - 25, y + 10, 25, 20, 1, 1, 1);
  drawCircle(x, y + 15, 28, 20, 1, 1, 1);
}

void drawFlower(float x, float y) {
  float petalR = 10;
  drawCircle(x, y + petalR, petalR, 20, 1, 0.6f, 0.8f);
  drawCircle(x, y - petalR, petalR, 20, 1, 0.6f, 0.8f);
  drawCircle(x + petalR, y, petalR, 20, 1, 0.6f, 0.8f);
  drawCircle(x - petalR, y, petalR, 20, 1, 0.6f, 0.8f);
  drawCircle(x, y, petalR * 0.7f, 20, 1, 1, 0.2f);
}

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

void drawFire(float x, float y, float offset) {
  drawTriangle(x - 20, y, x + 20, y, x, y + 40 + offset, 1.0f, 0.3f, 0.0f);
  drawTriangle(x - 15, y + 20, x + 15, y + 20, x, y + 55 + offset, 1.0f, 0.6f,
               0.0f);
  drawTriangle(x - 10, y + 35, x + 10, y + 35, x, y + 65 + offset, 1.0f, 0.9f,
               0.0f);
}

void DrawSeasonalBackground(Season currentSeason) {
  // Ground & Sky by Season
  if (currentSeason == SPRING) {
    drawRectangle(0, 120, WINDOW_WIDTH, 480, 0.46f, 0.92f, 0.96f);
    drawSun(sunX);
    drawRectangle(0, 0, WINDOW_WIDTH, 120, 0.0f, 0.75f, 0.29f);
  } else if (currentSeason == SUMMER) {
    drawRectangle(0, 120, WINDOW_WIDTH, 480, 0.46f, 0.92f, 0.96f);
    drawSun(sunX);
    drawRectangle(0, 0, WINDOW_WIDTH, 120, 0.0f, 0.75f, 0.29f);
  } else if (currentSeason == AUTUMN) {
    drawRectangle(0, 120, WINDOW_WIDTH, 480, 0.45f, 0.65f, 1.0f);
    drawSun(sunX);
    drawRectangle(0, 0, WINDOW_WIDTH, 120, 0.8588f, 0.5882f, 0.1843f);
  } else if (currentSeason == WINTER) {
    drawRectangle(0, 120, WINDOW_WIDTH, 480, 0.8f, 0.9f, 1.0f);
    drawSun(sunX);
    drawRectangle(0, 0, WINDOW_WIDTH, 120, 1.0f, 1.0f, 1.0f);
  }

  // Clouds
  drawCloud(cloudX[0], 500);
  drawCloud(cloudX[1], 550);
  drawCloud(cloudX[2], 480);

  // House
  drawRectangle(120, 120, 150, 100, 0.98f, 0.76f, 0.29f);          // base
  drawTriangle(100, 220, 290, 220, 195, 300, 0.45f, 0.17f, 0.02f); // roof
  drawRectangle(180, 120, 40, 70, 0.05f, 0.05f, 0.05f);            // door

  // Summer Fire
  if (currentSeason == SUMMER) {
    drawFire(150, 250, fireOffset[0]);
    drawFire(230, 252, fireOffset[1]);
  }

  // Trees
  drawTree(600, 120, currentSeason == SPRING, currentSeason == AUTUMN,
           currentSeason == WINTER);
  drawTree(700, 120, currentSeason == SPRING, currentSeason == AUTUMN,
           currentSeason == WINTER);
  drawTree(800, 120, currentSeason == SPRING, currentSeason == AUTUMN,
           currentSeason == WINTER);

  // Additional fire near trees (summer)
  if (currentSeason == SUMMER) {
    drawFire(613, 250, fireOffset[2]);
    drawFire(710, 245, fireOffset[3]);
    drawFire(817, 253, fireOffset[4]);
  }

  // Snow for winter
  if (currentSeason == WINTER) {
    for (auto &s : snowflakes)
      drawCircle(s.x, s.y, s.size, 10, 1, 1, 1);
  }
}

// ===== Game Class =====
class Game {
private:
  float basketX, basketY;
  float basketWidth, basketHeight;

  int score;
  int lives;
  int level;
  Season currentSeason;
  bool paused;
  bool gameOver;
  bool hasCompletedWinter; // Track if we've completed winter for level up
  bool showLevelUp;        // Flag to show level up message
  float levelUpTimer;      // Timer for level up message display

  std::vector<Item> items;

  float itemSpawnTimer;
  float deltaTime;
  float lastFrame;

  bool leftKey, rightKey, spaceKey, escapeKey;

public:
  Game()
      : score(0), lives(STARTING_LIVES), level(1), currentSeason(SPRING),
        paused(false), gameOver(false), hasCompletedWinter(false),
        showLevelUp(false), levelUpTimer(0.0f), itemSpawnTimer(0.0f),
        deltaTime(0.0f), lastFrame(0.0f), leftKey(false), rightKey(false),
        spaceKey(false), escapeKey(false) {

    basketWidth = 100.0f;
    basketHeight = 60.0f;
    // Position basket at the bottom of the screen
    basketX = WINDOW_WIDTH / 2 - basketWidth / 2;
    basketY = 20; // Near the bottom

    srand(time(nullptr));
  }

  float GetItemFallSpeed() {
    return BASE_ITEM_FALL_SPEED * level; // 2x faster per level
  }

  float GetItemSpawnInterval() {
    return BASE_ITEM_SPAWN_INTERVAL / level; // 2x faster spawning per level
  }

  void UpdateSeason() {
    // Fixed point thresholds for each season (regardless of level)
    const int SPRING_THRESHOLD = 0;
    const int SUMMER_THRESHOLD = 50;
    const int AUTUMN_THRESHOLD = 100;
    const int WINTER_THRESHOLD = 150;
    const int LEVEL_UP_THRESHOLD = 200; // Complete all seasons

    Season previousSeason = currentSeason;

    if (score < SUMMER_THRESHOLD)
      currentSeason = SPRING;
    else if (score < AUTUMN_THRESHOLD)
      currentSeason = SUMMER;
    else if (score < WINTER_THRESHOLD)
      currentSeason = AUTUMN;
    else if (score < LEVEL_UP_THRESHOLD)
      currentSeason = WINTER;

    // Check if we just completed winter and reached level up threshold
    if (previousSeason == WINTER && score >= LEVEL_UP_THRESHOLD &&
        !hasCompletedWinter) {
      hasCompletedWinter = true;
      LevelUp();
    }

    // Reset completion flag if we go back to spring
    if (currentSeason == SPRING && hasCompletedWinter) {
      hasCompletedWinter = false;
    }
  }

  void LevelUp() {
    level++;
    score = 0;                  // Reset score for new level
    currentSeason = SPRING;     // Start from spring again
    hasCompletedWinter = false; // Reset completion flag
    showLevelUp = true;         // Show level up message
    levelUpTimer = 3.0f;        // Display for 3 seconds

    std::cout << "LEVEL UP! Now at level " << level << "!" << std::endl;
    std::cout << "Items are now " << (level * 100) << "% faster!" << std::endl;
  }

  void GetSeasonItemColor(Season season, bool isGood, float &r, float &g,
                          float &b) {
    if (!isGood) {
      r = 0.3f;
      g = 0.3f;
      b = 0.3f; // Bad items are gray
      return;
    }

    switch (season) {
    case SPRING:
      r = 1.0f;
      g = 0.7f;
      b = 0.8f;
      break; // Pink cherry blossoms
    case SUMMER:
      r = 1.0f;
      g = 0.9f;
      b = 0.0f;
      break; // Bright yellow fruits
    case AUTUMN:
      r = 1.0f;
      g = 0.5f;
      b = 0.0f;
      break; // Orange/red leaves
    case WINTER:
      r = 0.9f;
      g = 0.9f;
      b = 1.0f;
      break; // White snowflakes
    default:
      r = 1.0f;
      g = 1.0f;
      b = 1.0f;
    }
  }

  std::string GetRandomItemName(Season season, bool isGood) {
    if (!isGood) {
      std::vector<std::string> badItems = {"Trash", "Rotten", "Broken"};
      return badItems[rand() % badItems.size()];
    }

    switch (season) {
    case SPRING: {
      std::vector<std::string> items = {"Cherry Blossom", "Flower", "Honey",
                                        "Apple"};
      return items[rand() % items.size()];
    }
    case SUMMER: {
      std::vector<std::string> items = {"Lemon", "Sunflower", "IceCream",
                                        "Sun"};
      return items[rand() % items.size()];
    }
    case AUTUMN: {
      std::vector<std::string> items = {"Maple Leaf", "Pumpkin", "Corn",
                                        "Apple"};
      return items[rand() % items.size()];
    }
    case WINTER: {
      std::vector<std::string> items = {"Snowflake", "Cocoa", "Cookie",
                                        "Scarf"};
      return items[rand() % items.size()];
    }
    default:
      return "Item";
    }
  }

  void SpawnItem() {
    Item item;
    item.size = 40.0f;
    item.x = rand() % (WINDOW_WIDTH - (int)item.size);
    // Spawn items from the TOP of the screen
    item.y = WINDOW_HEIGHT; // Start at top
    item.velocity = GetItemFallSpeed() + (score / 10.0f);

    int goodChance = 70;
    bool spawnGood = (rand() % 100) < goodChance;

    item.isGood = spawnGood;
    GetSeasonItemColor(currentSeason, spawnGood, item.r, item.g, item.b);
    item.a = 1.0f;
    item.type = GetRandomItemName(currentSeason, spawnGood);

    items.push_back(item);
  }

  bool CheckCollision(float x1, float y1, float w1, float h1, float x2,
                      float y2, float w2, float h2) {
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
  }

  void Update() {
    if (gameOver || paused)
      return;

    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    deltaTime = currentTime - lastFrame;
    lastFrame = currentTime;

    // Update level up timer
    if (showLevelUp) {
      levelUpTimer -= deltaTime;
      if (levelUpTimer <= 0) {
        showLevelUp = false;
      }
    }

    if (leftKey) {
      basketX -= BASKET_SPEED * deltaTime;
      if (basketX < 0)
        basketX = 0;
    }
    if (rightKey) {
      basketX += BASKET_SPEED * deltaTime;
      if (basketX + basketWidth > WINDOW_WIDTH)
        basketX = WINDOW_WIDTH - basketWidth;
    }

    itemSpawnTimer += deltaTime;
    if (itemSpawnTimer >= GetItemSpawnInterval()) {
      SpawnItem();
      itemSpawnTimer = 0.0f;
    }

    for (auto it = items.begin(); it != items.end();) {
      // Items fall DOWNWARD (negative Y direction)
      it->y -= it->velocity * deltaTime;

      if (CheckCollision(it->x, it->y, it->size, it->size, basketX, basketY,
                         basketWidth, basketHeight)) {
        if (it->isGood) {
          score += 10;
          UpdateSeason();
        } else {
          lives--;
          if (lives <= 0) {
            gameOver = true;
          }
        }
        it = items.erase(it);
      } else if (it->y < -it->size) { // Item falls below the screen
        it = items.erase(it);
      } else {
        ++it;
      }
    }
  }

  void DrawRect(float x, float y, float width, float height, float r, float g,
                float b, float a = 1.0f) {
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
  }

  void DrawText(const std::string &text, float x, float y, float r, float g,
                float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (char c : text) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
  }

  void DrawLargeText(const std::string &text, float x, float y, float r,
                     float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (char c : text) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
  }

  void DrawHearts() {
    float startX = 60; // Moved right to make space for "Heart" text
    float startY = WINDOW_HEIGHT - 45;
    float spacing = 30;

    for (int i = 0; i < STARTING_LIVES; i++) {
      if (i < lives) {
        // Full hearts for remaining lives
        drawHeart(startX + i * spacing, startY, 0.15f, 1.0f, 0.3f, 0.3f);
      } else {
        // Empty/gray hearts for lost lives
        drawHeart(startX + i * spacing, startY, 0.15f, 0.5f, 0.5f, 0.5f);
      }
    }
  }

  void RenderLevelUpMessage() {
    // Semi-transparent background covering most of the screen
    DrawRect(WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2 - 100, 400, 200, 0.0f,
             0.0f, 0.0f, 0.85f);

    // Yellow border for emphasis
    DrawRect(WINDOW_WIDTH / 2 - 202, WINDOW_HEIGHT / 2 - 102, 404, 204, 1.0f,
             1.0f, 0.0f, 1.0f);

    // Main "LEVEL UP!" text - large and centered
    DrawLargeText("LEVEL UP!", WINDOW_WIDTH / 2 - 70, WINDOW_HEIGHT / 2 + 40,
                  1.0f, 1.0f, 0.0f);

    // Level information - medium size
    DrawLargeText("Now at Level " + std::to_string(level),
                  WINDOW_WIDTH / 2 - 90, WINDOW_HEIGHT / 2, 1.0f, 1.0f, 1.0f);

    // Speed increase info
    DrawText("Items are now " + std::to_string(level * 100) + "% faster!",
             WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT / 2 - 30, 0.8f, 0.8f, 1.0f);

    // Countdown timer
    DrawText("Message disappears in " + std::to_string((int)levelUpTimer + 1) +
                 " seconds...",
             WINDOW_WIDTH / 2 - 140, WINDOW_HEIGHT / 2 - 60, 1.0f, 0.5f, 0.5f);
  }

  void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw seasonal background
    DrawSeasonalBackground(currentSeason);

    // Draw basket (at bottom)
    DrawRect(basketX, basketY, basketWidth, basketHeight, 0.6f, 0.4f, 0.2f);

    // Draw items (falling from top to bottom)
    for (const auto &item : items) {
      DrawRect(item.x, item.y, item.size, item.size, item.r, item.g, item.b,
               item.a);
    }

    // Draw UI with semi-transparent background (moved to top)
    DrawRect(5, WINDOW_HEIGHT - 85, 150, 80, 0.0f, 0.0f, 0.0f, 0.5f);
    DrawText("Score: " + std::to_string(score), 10, WINDOW_HEIGHT - 70, 1.0f,
             1.0f, 1.0f);
    DrawText("Level: " + std::to_string(level), 10, WINDOW_HEIGHT - 55, 1.0f,
             1.0f, 1.0f);

    // Draw "Heart" text and hearts
    DrawText("Heart:", 10, WINDOW_HEIGHT - 45, 1.0f, 1.0f, 1.0f);
    DrawHearts();

    DrawText("Season: " + std::string(GetSeasonName()), 10, WINDOW_HEIGHT - 30,
             1.0f, 1.0f, 1.0f);

    // Draw Level Up message in center (on top of everything)
    if (showLevelUp) {
      RenderLevelUpMessage();
    }

    if (paused && !gameOver) {
      DrawRect(WINDOW_WIDTH / 2 - 60, WINDOW_HEIGHT / 2 - 15, 120, 30, 0.0f,
               0.0f, 0.0f, 0.8f);
      DrawText("PAUSED", WINDOW_WIDTH / 2 - 30, WINDOW_HEIGHT / 2 - 5, 1.0f,
               1.0f, 0.0f);
    }

    if (gameOver) {
      DrawRect(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 50, 200, 100, 0.0f,
               0.0f, 0.0f, 0.9f);
      DrawText("GAME OVER", WINDOW_WIDTH / 2 - 45, WINDOW_HEIGHT / 2 - 30, 1.0f,
               0.0f, 0.0f);
      DrawText("Score: " + std::to_string(score), WINDOW_WIDTH / 2 - 40,
               WINDOW_HEIGHT / 2, 1.0f, 1.0f, 1.0f);
      DrawText("Level: " + std::to_string(level), WINDOW_WIDTH / 2 - 40,
               WINDOW_HEIGHT / 2 + 15, 1.0f, 1.0f, 1.0f);
      DrawText("Press SPACE to restart", WINDOW_WIDTH / 2 - 80,
               WINDOW_HEIGHT / 2 + 35, 1.0f, 1.0f, 0.0f);
    }

    glutSwapBuffers();
  }

  void HandleKeyPress(unsigned char key, int x, int y) {
    switch (key) {
    case 27: // ESC key
      exit(0);
      break;
    case ' ': // Space key
      if (gameOver) {
        Reset();
      } else {
        paused = !paused;
      }
      break;
    case 'a':
    case 'A':
      leftKey = true;
      break;
    case 'd':
    case 'D':
      rightKey = true;
      break;
    case 'n': // Switch season manually (for testing)
      currentSeason = (Season)((currentSeason + 1) % 4);
      break;
    case 'l': // Level up manually (for testing)
      LevelUp();
      break;
    }
  }

  void HandleKeyRelease(unsigned char key, int x, int y) {
    switch (key) {
    case 'a':
    case 'A':
      leftKey = false;
      break;
    case 'd':
    case 'D':
      rightKey = false;
      break;
    }
  }

  void HandleSpecialKeyPress(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
      leftKey = true;
      break;
    case GLUT_KEY_RIGHT:
      rightKey = true;
      break;
    }
  }

  void HandleSpecialKeyRelease(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
      leftKey = false;
      break;
    case GLUT_KEY_RIGHT:
      rightKey = false;
      break;
    }
  }

  const char *GetSeasonName() {
    switch (currentSeason) {
    case SPRING:
      return "SPRING";
    case SUMMER:
      return "SUMMER";
    case AUTUMN:
      return "AUTUMN";
    case WINTER:
      return "WINTER";
    default:
      return "UNKNOWN";
    }
  }

  void Reset() {
    score = 0;
    lives = STARTING_LIVES;
    level = 1;
    currentSeason = SPRING;
    gameOver = false;
    paused = false;
    hasCompletedWinter = false;
    showLevelUp = false;
    items.clear();
    itemSpawnTimer = 0.0f;
    basketX = WINDOW_WIDTH / 2 - basketWidth / 2;
    basketY = 20; // Reset to bottom position
    leftKey = false;
    rightKey = false;
  }

  Season GetCurrentSeason() const { return currentSeason; }
};

Game *game = nullptr;

void display() {
  if (game) {
    game->Render();
  }
}

void updateScene(int value) {
  // Sun animation
  sunX += sunSpeed;
  if (sunX > WINDOW_WIDTH + 50)
    sunX = -50.0f;

  // Clouds animation
  for (int i = 0; i < 3; i++) {
    cloudX[i] += 0.5f + i * 0.2f;
    if (cloudX[i] > WINDOW_WIDTH + 50)
      cloudX[i] = -50.0f;
  }

  // Fire flicker
  for (int i = 0; i < 5; i++)
    fireOffset[i] = rand() % 10;

  // Snow animation (only in winter)
  if (game && game->GetCurrentSeason() == WINTER) {
    for (auto &s : snowflakes) {
      s.y -= 2;
      if (s.y < 0)
        s.y = WINDOW_HEIGHT;
    }
  }

  glutPostRedisplay();
  glutTimerFunc(16, updateScene, 0);
}

void updateGame(int value) {
  if (game) {
    game->Update();
  }
  glutTimerFunc(16, updateGame, 0);
}

void keyboard(unsigned char key, int x, int y) {
  if (game) {
    game->HandleKeyPress(key, x, y);
  }
}

void keyboardUp(unsigned char key, int x, int y) {
  if (game) {
    game->HandleKeyRelease(key, x, y);
  }
}

void special(int key, int x, int y) {
  if (game) {
    game->HandleSpecialKeyPress(key, x, y);
  }
}

void specialUp(int key, int x, int y) {
  if (game) {
    game->HandleSpecialKeyRelease(key, x, y);
  }
}

void initGL() {
  glClearColor(0.46f, 0.92f, 0.96f, 1.0f);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Initialize snowflakes
  for (int i = 0; i < 100; i++)
    snowflakes.push_back({(float)(rand() % WINDOW_WIDTH),
                          (float)(rand() % WINDOW_HEIGHT), 2 + rand() % 3});

  // Initialize clouds
  for (int i = 0; i < 3; i++)
    cloudX[i] = rand() % WINDOW_WIDTH;
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Seasonal Catcher with Animated Background");

  initGL();

  game = new Game();

  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutKeyboardUpFunc(keyboardUp);
  glutSpecialFunc(special);
  glutSpecialUpFunc(specialUp);
  glutTimerFunc(0, updateScene, 0);
  glutTimerFunc(0, updateGame, 0);

  std::cout << "=== SEASONAL CATCHER ===" << std::endl;
  std::cout << "Arrow Keys or A/D: Move basket" << std::endl;
  std::cout << "SPACE: Pause/Resume (or Restart after game over)" << std::endl;
  std::cout << "N: Switch season manually" << std::endl;
  std::cout << "L: Level up manually (for testing)" << std::endl;
  std::cout << "ESC: Exit game" << std::endl;
  std::cout << "\nCatch good items (+10 points)" << std::endl;
  std::cout << "Avoid bad items (-1 life)" << std::endl;
  std::cout << "Seasons progression:" << std::endl;
  std::cout << "  Spring (0-49): Catch pink cherry blossoms" << std::endl;
  std::cout << "  Summer (50-99): Catch bright yellow fruits" << std::endl;
  std::cout << "  Autumn (100-149): Catch orange/red leaves" << std::endl;
  std::cout << "  Winter (150-199): Catch white snowflakes" << std::endl;
  std::cout << "  Complete all seasons (reach 200) to level up!" << std::endl;
  std::cout << "  Each level: 2x faster items!" << std::endl;
  std::cout << "\nGood luck!" << std::endl;

  glutMainLoop();

  delete game;
  return 0;
}
