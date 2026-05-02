#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>

using namespace std;

struct Player;
struct Floor;

// One leaderboard record.
struct ScoreEntry
{
    string name;
    int score;
};

// Load words from a file.
vector<string> loadWords(string filename);

// Initialize all word lists.
void initializeWordDatabase();

// Get a random word by difficulty.
string getRandomWord(int difficulty);

// Load leaderboard records.
vector<ScoreEntry> loadLeaderboard();

// Save a score to the leaderboard.
void saveLeaderboard(string name, int score);

// Show the leaderboard.
void displayLeaderboard();

// Save the current game state.
void saveGameState(const Player& p, const Floor& f);

// Load a saved game state.
bool loadGameState(Player& p, Floor& f);

// Check if a save file exists.
bool hasSaveFile();

#endif
