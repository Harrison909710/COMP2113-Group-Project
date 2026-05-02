#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>

using namespace std;

struct Player;
struct Floor;

// Store one leaderboard record.
// Input: player name and score.
// Output: one score entry object.
struct ScoreEntry
{
    string name;
    int score;
};

// Load words from a file.
// Input: filename of the word list.
// Output: returns a vector of words.
vector<string> loadWords(string filename);

// Initialize all word lists.
// Input: none.
// Output: loads word data into memory.
void initializeWordDatabase();

// Get a random word by difficulty.
// Input: difficulty level.
// Output: returns one random word.
string getRandomWord(int difficulty);

// Load leaderboard records.
// Input: none.
// Output: returns a vector of score entries.
vector<ScoreEntry> loadLeaderboard();

// Save a score to the leaderboard.
// Input: player name and score.
// Output: updates the leaderboard file.
void saveLeaderboard(string name, int score);

// Display the leaderboard.
// Input: none.
// Output: prints leaderboard data.
void displayLeaderboard();

// Save the current game state.
// Input: player data and floor data.
// Output: writes save data to file.
void saveGameState(const Player& p, const Floor& f);

// Load a saved game state.
// Input: player and floor passed by reference.
// Output: returns true if loading succeeds.
bool loadGameState(Player& p, Floor& f);

// Check if a save file exists.
// Input: none.
// Output: returns true if save.txt exists.
bool hasSaveFile();

#endif
