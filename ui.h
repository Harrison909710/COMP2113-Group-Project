#ifndef UI_H
#define UI_H

#include "player.h"   // Player, Room, ItemType
#include "dungeon.h"  // DungeonGenerator, Floor, Room
#include "puzzle_engine.h"

// ===== Main Menu & Game Flow =====

// What it does: Displays the main menu (New Game, Continue, Leaderboard, Quit).
// What the inputs are: None.
// What the outputs are: None (prints to console).
void displayMainMenu();

// What it does: Displays the difficulty selection menu (Easy, Medium, Hard).
// What the inputs are: None.
// What the outputs are: None (prints to console).
void displayDifficultySelect();

// What it does: Starts a new game or loads a saved game, then launches the game loop.
// What the inputs are: player - reference to a Player struct; difficulty - 1 (Easy), 2 (Medium), 3 (Hard), or -1 to load a saved game.
// What the outputs are: None (runs the game until completion or player death).
void runGame(Player& player, int difficulty);

// ===== Main Game Loop =====

// What it does: Runs the main game loop: movement, puzzle rooms, treasure, boss fight, and floor progression.
// What the inputs are: p - reference to the Player; gen - reference to the DungeonGenerator; chosenDifficulty - game difficulty level.
// What the outputs are: None (modifies player state and floors until win/loss).
void gameLoop(Player& p, DungeonGenerator& gen, int chosenDifficulty);

// ===== Puzzle Routing =====

// What it does: Routes a puzzle room to the correct puzzle function based on room type.
// What the inputs are: p - reference to the Player; dRoom - reference to the Room containing the puzzle; engine - reference to the PuzzleEngine.
// What the outputs are: Returns true if the puzzle is solved, false otherwise.
bool runPuzzle(Player& p, Room& dRoom, PuzzleEngine& engine);

// ===== Room Encounters =====

// What it does: Handles a treasure room encounter: gives the player a random item.
// What the inputs are: p - reference to the Player.
// What the outputs are: None (adds an item to the player's inventory).
void runTreasure(Player& p);

// What it does: Runs the final boss fight (Floor 10) consisting of three puzzle phases.
// What the inputs are: p - reference to the Player; bossRoom - reference to the boss Room; engine - reference to the PuzzleEngine.
// What the outputs are: None (modifies player stats and room cleared state).
void runBossFight(Player& p, Room& bossRoom, PuzzleEngine& engine);

// ===== Display Helpers =====

// What it does: Displays the player's HP bar, current floor, and score.
// What the inputs are: p - constant reference to the Player.
// What the outputs are: None (prints to console).
void displayHPBar(const Player& p);

// What it does: Prints a description of the room the player has entered.
// What the inputs are: r - constant reference to the Room.
// What the outputs are: None (prints to console).
void displayRoomDescription(const Room& r);

// What it does: Displays ASCII art for the title screen or boss encounter.
// What the inputs are: type - a string, either "title" or "boss".
// What the outputs are: None (prints to console).
void drawASCII(const std::string& type);

// What it does: Shows the death screen with the player's final score.
// What the inputs are: p - constant reference to the Player.
// What the outputs are: None (prints to console).
void displayDeathScreen(const Player& p);

// What it does: Shows the victory screen with the player's final score.
// What the inputs are: p - constant reference to the Player.
// What the outputs are: None (prints to console).
void displayWinScreen(const Player& p);

// What it does: Displays the current floor number banner.
// What the inputs are: floor - the current floor number.
// What the outputs are: None (prints to console).
void displayFloorInfo(int floor);

// ===== Individual Puzzle Mini-Games =====

// What it does: Runs the anagram puzzle: the player must unscramble a word.
// What the inputs are: p - reference to the Player; r - reference to the Room; engine - reference to the PuzzleEngine.
// What the outputs are: Returns true if the player answers correctly, false otherwise.
bool runAnagramPuzzle(Player& p, Room& r, PuzzleEngine& engine);

// What it does: Runs the Caesar cipher puzzle: the player must decrypt the word.
// What the inputs are: p - reference to the Player; r - reference to the Room; engine - reference to the PuzzleEngine.
// What the outputs are: Returns true if the player answers correctly, false otherwise.
bool runCaesarPuzzle(Player& p, Room& r, PuzzleEngine& engine);

// What it does: Runs the substitution cipher puzzle: the player must decrypt the word.
// What the inputs are: p - reference to the Player; r - reference to the Room; engine - reference to the PuzzleEngine.
// What the outputs are: Returns true if the player answers correctly, false otherwise.
bool runSubstitutionPuzzle(Player& p, Room& r, PuzzleEngine& engine);

// What it does: Runs the hangman puzzle: the player guesses letters until the word is complete.
// What the inputs are: p - reference to the Player; r - reference to the Room.
// What the outputs are: Returns true if the player completes the word, false if they exceed wrong guesses.
bool runHangmanPuzzle(Player& p, Room& r);

// What it does: Runs the riddle puzzle: the player answers a riddle.
// What the inputs are: p - reference to the Player; r - reference to the Room; engine - reference to the PuzzleEngine (provides the riddle).
// What the outputs are: Returns true if the player answers correctly, false otherwise.
bool runRiddlePuzzle(Player& p, Room& r, PuzzleEngine& engine);

// What it does: Runs the word chain puzzle: the player provides a middle word that differs by one letter.
// What the inputs are: p - reference to the Player; r - reference to the Room (including answer used for the start and end of the chain); engine - reference to the PuzzleEngine.
// What the outputs are: Returns true if the chain is valid, false otherwise.
bool runWordChainPuzzle(Player& p, Room& r, PuzzleEngine& engine);

// What it does: Runs the speed round puzzle: the player must unscramble a word within a time limit.
// What the inputs are: p - reference to the Player; r - reference to the Room; engine - reference to the PuzzleEngine; timeLimit - seconds allowed.
// What the outputs are: Returns true if solved in time, false otherwise.
bool runSpeedRound(Player& p, Room& r, PuzzleEngine& engine, int timeLimit);

// ===== Hint & Inventory Helpers =====

// What it does: Creates a Room struct filled with pre-written hints for a given answer.
// What the inputs are: answer - the correct word for which hints are generated.
// What the outputs are: Returns a Room object containing hint1, hint2, hint3.
Room makeHintRoom(const std::string& answer);

// ===== Save / Load =====

// What it does: Saves the full game state including the player's inventory.
// What the inputs are: p - constant reference to the Player; f - constant reference to the current Floor.
// What the outputs are: None (writes to save files).
void saveFullGame(const Player& p, const Floor& f);

// What it does: Loads a full saved game, regenerating the floor layout and restoring inventory.
// What the inputs are: p - reference to Player (will be overwritten); f - reference to Floor; gen - reference to DungeonGenerator; difficulty - ignored (re-obtained from save).
// What the outputs are: Returns true if loading succeeded, false otherwise.
bool loadFullGame(Player& p, Floor& f, DungeonGenerator& gen, int difficulty);

// What it does: Pauses execution until the user presses any key.
// What the inputs are: None.
// What the outputs are: None (blocks until a keypress is received).
void pauseForUser();

#endif
