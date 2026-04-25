#include "ui.h"
#include "database.h"
#include <iostream>
using namespace std;

int main() {
    initializeWordDatabase();   // loads all word files
    srand(static_cast<unsigned>(time(0)));

    bool quit = false;
    while (!quit) {
        displayMainMenu();
        char choice;
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case '1': { // New Game
                string name;
                cout << "Enter your name: ";
                getline(cin, name);
                displayDifficultySelect();
                int diff;
                cin >> diff;
                Player player = createPlayer(name, diff);
                runGame(player, diff);
                break;
            }
            case '2': { // Continue
                if (hasSaveFile()) {
                    Player player;
                    player.name = "Adventurer";
                    runGame(player, -1); // -1 triggers load
                } else {
                    cout << "No save file found.\n";
                }
                break;
            }
            case '3':
                displayLeaderboard();
                break;
            case '4':
                quit = true;
                break;
            default:
                cout << "Invalid choice.\n";
        }
    }
    return 0;
}
