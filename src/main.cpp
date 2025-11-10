#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SH1106_ESP32.h"

// ------------------- OLED CONFIG -------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SH1106_ESP32 display(21, 22);

// ------------------- INPUT CONFIG -------------------
#define TOUCH_PIN 26   // TTP223 output

// ------------------- GAME VARIABLES -------------------
int playerY = 48;
int playerVY = 0;
bool isJumping = false;

int obstacleX = SCREEN_WIDTH;
int obstacleY = 48;
int obstacleSpeed = 4;

bool gameOver = false;
unsigned long score = 0;
unsigned long lastUpdate = 0;

// ------------------- SETUP -------------------
void setup() {
  Serial.begin(115200);
  pinMode(TOUCH_PIN, INPUT);

  Wire.begin(21, 22);
  Wire.setClock(400000);
  display.begin(SH1106_SWITCHCAPVCC, SH1106_I2C_ADDRESS, true);
  display.clearDisplay();
  display.display();

  Serial.println("PropelX Labs | Speed Run Game Started!");
}

// ------------------- LOOP -------------------
void loop() {
  bool touch = digitalRead(TOUCH_PIN);
  unsigned long now = millis();

  if (!gameOver) {
    // --- Physics ---
    if (touch && !isJumping) {
      playerVY = -6;      // Jump impulse
      isJumping = true;
    }

    playerY += playerVY;
    playerVY += 1;         // gravity

    if (playerY >= 48) {   // Ground contact
      playerY = 48;
      isJumping = false;
    }

    // --- Obstacle movement ---
    obstacleX -= obstacleSpeed;
    if (obstacleX < -10) {
      obstacleX = SCREEN_WIDTH + random(10, 40);
      obstacleSpeed = random(4, 7); // Random speed for variety
    }

    // --- Collision detection ---
    if (obstacleX < 18 && obstacleX > 4 && playerY > 40) {
      gameOver = true;
    }

    // --- Score update ---
    if (now - lastUpdate > 100) {  // Every 0.1s
      score++;
      lastUpdate = now;
    }

    // --- Drawing ---
    display.clearDisplay();

    // Title
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(5, 0);
    display.println("PropelX Labs");

    // Ground
    display.drawLine(0, 58, SCREEN_WIDTH, 58, WHITE);

    // Player
    display.fillRect(10, playerY, 8, 8, WHITE);

    // Obstacle
    display.fillRect(obstacleX, obstacleY, 8, 8, WHITE);

    // Score
    display.setTextSize(1);
    display.setCursor(95, 0);
    display.print(score);

    display.display();
  } else {
    // --- Game Over Screen ---
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(15, 20);
    display.println("Game Over");

    display.setTextSize(1);
    display.setCursor(25, 45);
    display.print("Score: ");
    display.print(score);
    display.setCursor(10, 55);
    display.println("Touch to Restart");
    display.display();

    if (touch) {
      // Reset game quickly
      delay(200);
      playerY = 48;
      playerVY = 0;
      isJumping = false;
      obstacleX = SCREEN_WIDTH;
      obstacleSpeed = 4;
      score = 0;
      gameOver = false;
      delay(200);
    }
  }

  delay(16); // ~60 FPS feel
}
