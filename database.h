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

vector<ScoreEntry> loadLeaderboard();
void saveLeaderboard(string name, int score);
void displayLeaderboard();

void saveGameState(const Player& p, const Floor& f);
bool loadGameState(Player& p, Floor& f);

bool hasSaveFile();

#endif
