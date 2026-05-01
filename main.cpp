#include "ui.h"
#include "database.h"
#include "dungeon.h"
#include <iostream>
using namespace std;

int main() {
    initializeWordDatabase();   // loads all word files
    srand(static_cast<unsigned>(time(0)));

    Player player;
    Floor currentFloorObj; 
    bool quit = false;
    while (!quit) {
        displayMainMenu();
        cout << "Please select an option (1-4): "; 
        char choice;
        cin >> choice;
        cin.ignore(1000, '\n'); // Clean buffer for getline

        switch (choice) {
            case '1': { // New Game
                cout << "Enter your name: ";
                string name;
                getline(cin, name);
                
                int diff;
                while (true) {
                    displayDifficultySelect();
                    cout << "Please select difficulty (1-3): ";
                    if (!(cin >> diff)) {
                        cin.clear();
                        cin.ignore(1000, '\n');
                        cout << "Invalid input. Please enter a number.\n";
                        continue;
                    }
                    cin.ignore(1000, '\n');

                    if (diff >= 1 && diff <= 3) {
                        break;
                    } else {
                        cout << "Invalid difficulty. Please choose 1, 2, or 3.\n";
                    }
                }
                
                player = createPlayer(name, diff);
                runGame(player, diff);
                break;
            }
            case '2': {
                if (hasSaveFile()) {
                    if (loadGameState(player, currentFloorObj)) {
                        pauseForUser();
                        runGame(player, -1);
                    } else {
                        cout << "Load failed." << endl;
                        pauseForUser();
                    }
                } else {
                    cout << "No save file found!" << endl;
                    pauseForUser();
                }
                break;
            }
            case '3':
                displayLeaderboard();
                pauseForUser();
                break;
            case '4':
                quit = true;
                break;
            default:
                cout << "Invalid choice.\n";
                pauseForUser();
        }
    }
    return 0;
}
