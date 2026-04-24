#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>

using namespace std;

struct Player;
struct Floor;

struct ScoreEntry
{
    string name;
    int score;
};

vector<string> loadWords(string filename);
void initializeWordDatabase();
string getRandomWord(int difficulty);

void saveLeaderboard(string name, int score);
vector<ScoreEntry> loadLeaderboard();
void displayLeaderboard();

void saveGameState(Player p, Floor f);
Player loadGameState();

bool hasSaveFile();

#endif
