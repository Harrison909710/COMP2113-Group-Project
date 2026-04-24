#include "database.h"
#include "player.h"
#include "dungeon.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

static vector<string> easyWords;
static vector<string> mediumWords;
static vector<string> hardWords;

vector<string> loadWords(string filename)
{
    vector<string> words;
    ifstream fin(filename);
    string word;

    if (!fin.is_open())
    {
        cout << "Error: cannot open file " << filename << endl;
        return words;
    }

    while (fin >> word)
    {
        if (word != "")
        {
            words.push_back(word);
        }
    }

    fin.close();
    return words;
}

void initializeWordDatabase()
{
    easyWords = loadWords("words_easy.txt");
    mediumWords = loadWords("words_medium.txt");
    hardWords = loadWords("words_hard.txt");

    srand(time(0));
}

string getRandomWord(int difficulty)
{
    vector<string>* selectedList = nullptr;

    if (difficulty == 1)
    {
        selectedList = &easyWords;
    }
    else if (difficulty == 2)
    {
        selectedList = &mediumWords;
    }
    else
    {
        selectedList = &hardWords;
    }

    if (selectedList->empty())
    {
        return "";
    }

    int index = rand() % selectedList->size();
    return (*selectedList)[index];
}

vector<ScoreEntry> loadLeaderboard()
{
    vector<ScoreEntry> leaderboard;
    ifstream fin("leaderboard.txt");

    if (!fin.is_open())
    {
        return leaderboard;
    }

    ScoreEntry entry;
    while (fin >> entry.name >> entry.score)
    {
        leaderboard.push_back(entry);
    }

    fin.close();
    return leaderboard;
}

void saveLeaderboard(string name, int score)
{
    vector<ScoreEntry> leaderboard = loadLeaderboard();

    ScoreEntry newEntry;
    newEntry.name = name;
    newEntry.score = score;
    leaderboard.push_back(newEntry);

    sort(leaderboard.begin(), leaderboard.end(),
         [](const ScoreEntry& a, const ScoreEntry& b)
         {
             return a.score > b.score;
         });

    if (leaderboard.size() > 10)
    {
        leaderboard.resize(10);
    }

    ofstream fout("leaderboard.txt");

    if (!fout.is_open())
    {
        cout << "Error: cannot write leaderboard.txt" << endl;
        return;
    }

    for (int i = 0; i < (int)leaderboard.size(); i++)
    {
        fout << leaderboard[i].name << " " << leaderboard[i].score << endl;
    }

    fout.close();
}

void displayLeaderboard()
{
    vector<ScoreEntry> leaderboard = loadLeaderboard();

    cout << "===== LEADERBOARD =====" << endl;

    if (leaderboard.empty())
    {
        cout << "No scores yet." << endl;
        return;
    }

    for (int i = 0; i < (int)leaderboard.size(); i++)
    {
        cout << i + 1 << ". "
             << leaderboard[i].name
             << " - "
             << leaderboard[i].score
             << endl;
    }
}

void saveGameState(const Player& p, const Floor& f)
{
    ofstream fout("save.txt");

    if (!fout.is_open())
    {
        cout << "Error: cannot write save.txt" << endl;
        return;
    }

    fout << p.name << endl;
    fout << p.hp << " "
         << p.maxHp << " "
         << p.score << " "
         << p.currentFloor << " "
         << p.wrongGuesses << " "
         << p.freezeActive << " "
         << p.freezeSecondsLeft << endl;

    fout << f.floorNumber << " "
         << f.startX << " "
         << f.startY << " "
         << f.hasTreasure << endl;

    fout.close();
    cout << "Game saved successfully." << endl;
}

bool loadGameState(Player& p, Floor& f)
{
    ifstream fin("save.txt");

    if (!fin.is_open())
    {
        cout << "Error: cannot open save.txt" << endl;
        return false;
    }

    getline(fin, p.name);

    fin >> p.hp
        >> p.maxHp
        >> p.score
        >> p.currentFloor
        >> p.wrongGuesses
        >> p.freezeActive
        >> p.freezeSecondsLeft;

    fin >> f.floorNumber
        >> f.startX
        >> f.startY
        >> f.hasTreasure;

    fin.close();
    cout << "Game loaded successfully." << endl;
    return true;
}

bool hasSaveFile()
{
    ifstream fin("save.txt");
    bool exists = fin.is_open();
    fin.close();
    return exists;
}
