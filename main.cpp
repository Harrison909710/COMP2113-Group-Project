#include "ui.h"
#include "database.h"
#include <iostream>
using namespace std;

int main() {
    initializeWordDatabase();   // loads all word files
    srand(static_cast<unsigned>(time(0)));

    Player player;
    bool quit = false;
    while (!quit) {
        displayMainMenu();
        char choice;
        cin >> choice;
        cin.ignore(1000, '\n'); // Clean buffer for getline

        switch (choice) {
            case '1': { // New Game
                cout << "Enter your name: ";
                string name;
                getline(cin, name);
                displayDifficultySelect();
                int diff;
                cin >> diff;
                cin.ignore(1000, '\n');
                
                player = createPlayer(name, diff);
                runGame(player, diff);
                break;
            }
            case '2': { // Continue
                if (hasSaveFile()) {
                    player.name = "Adventurer";
                    runGame(player, -1); // -1 triggers load
                } else {
                    cout << "No save file found!\n";
                }
                break;
            }
            case '3':
                displayLeaderboard();
                pauseForUser();
                break;
            case '4':
                quit = true;
                cout << "Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice.\n";
        }
    }
    return 0;
}
