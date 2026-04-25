#ifndef UI_H
#define UI_H

#include "player.h"   // Player, Room, ItemType
#include "dungeon.h"  // DungeonGenerator, Floor, Room
#include "puzzle_engine.h"

// Main menu and game entry point
void displayMainMenu();
void displayDifficultySelect();
void runGame(Player& player, int difficulty);

// Game loop helpers
void gameLoop(Player& p, DungeonGenerator& gen, int chosenDifficulty);
bool runPuzzle(Player& p, Room& dRoom, PuzzleEngine& engine); // true if solved
void runTreasure(Player& p);
void runBossFight(Player& p, Room& bossRoom, PuzzleEngine& engine);

// Display helpers
void displayHPBar(const Player& p);
void displayRoomDescription(const Room& r);
void drawASCII(const std::string& type);
void displayDeathScreen(const Player& p);
void displayWinScreen(const Player& p);
void displayFloorInfo(int floor);

// Puzzle mini-games
bool runAnagramPuzzle(Player& p, const std::string& answer, PuzzleEngine& engine);
bool runCaesarPuzzle(Player& p, const std::string& answer, PuzzleEngine& engine);
bool runSubstitutionPuzzle(Player& p, const std::string& answer, PuzzleEngine& engine);
bool runHangmanPuzzle(Player& p, const std::string& answer);
bool runRiddlePuzzle(Player& p, PuzzleEngine& engine);
bool runWordChainPuzzle(Player& p, const std::string& answer, PuzzleEngine& engine);
bool runSpeedRound(Player& p, const std::string& answer, PuzzleEngine& engine, int timeLimit);

// Hint generation (fills a Player::Room)
Player::Room makeHintRoom(const std::string& answer);

// Save/Load extension for inventory
void saveFullGame(const Player& p, const Floor& f);
bool loadFullGame(Player& p, Floor& f, DungeonGenerator& gen, int difficulty);

#endif
