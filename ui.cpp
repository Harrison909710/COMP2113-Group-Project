#include "ui.h"
#include "database.h"   // for save/load, leaderboard, words
#include "dungeon.h"   // for puzzle room answer
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cctype>
//implement GETCH() for both windows and linux
#ifdef _WIN32
    #include <conio.h>
    #define GETCH _getch
#else
    #include <termios.h>
    #include <unistd.h>
    #include <stdio.h>
    inline int getch_linux() {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
    #define GETCH getch_linux
#endif

using namespace std;

// =================== Main Menu & Setup ===================
void displayMainMenu() {
    drawASCII("title");
    cout << "\n===== WORD DUNGEON =====\n"
         << "1. New Game\n"
         << "2. Continue\n"
         << "3. Leaderboard\n"
         << "4. Quit\n";
}

void displayDifficultySelect() {
    cout << "\nChoose difficulty:\n"
         << "1. Easy   (HP: 10)\n"
         << "2. Medium (HP: 7)\n"
         << "3. Hard   (HP: 5)\n";
}

void runGame(Player& player, int difficulty) {
    // Initialize word database (P4)
    initializeWordDatabase();

    DungeonGenerator gen(11, 11);
    Player localPlayer = createPlayer(player.name, difficulty);

    Floor tempFloor;

    // Check if we are loading a saved game
    if (difficulty == -1) { // -1 used as a flag for "Continue"
        if (!loadFullGame(localPlayer, tempFloor, gen, difficulty)) {
            cout << "No save file found. Starting a new game.\n";
            cout << "Press any key to continue...";
            GETCH();
            gameLoop(localPlayer, gen, 1); // fallback
            return;
        }
    } else {
        // New game: start from floor 1
        localPlayer.currentFloor = 1;
        gameLoop(localPlayer, gen, difficulty);
    }

    // After game over, update leaderboard and display
    saveLeaderboard(localPlayer.name, calculateScore(localPlayer));
    displayLeaderboard();
}

void displayMap(const Floor& f, int playerX, int playerY, DungeonGenerator& gen) {
    cout << "\n--- DUNGEON MAP ---\n";
    for (int y = 0; y < 11; ++y) {
        for (int x = 0; x < 11; ++x) {
            if (x == playerX && y == playerY) {
                cout << "@ "; 
            } else {
                Room* r = gen.getRoomAt(const_cast<Floor&>(f), x, y);
                if (r != nullptr) {
                    if (r->isCleared) {
                        cout << ". "; 
                    } else {
                        switch (r->type) {
                            case RoomType::TREASURE: cout << "T "; break;
                            case RoomType::BOSS:     cout << "B "; break;
                            default:                 cout << "P "; break; 
                        }
                    }
                } else {
                    if (f.layout.grid[y][x] == TileType::STAIRS) {
                        cout << "S "; 
                    } else if (f.layout.grid[y][x] == TileType::WALL) {
                        cout << "# "; 
                    } else {
                        cout << ". "; 
                    }
                }
            }
        }
        cout << "\n";
    }
    cout << "-------------------\n";
}

// =================== Main Game Loop ===================
void gameLoop(Player& p, DungeonGenerator& gen, int difficulty) {
    int pX, pY;
    Floor currentFloor = gen.generateFloor(p.currentFloor, difficulty);
    pX = currentFloor.startX;
    pY = currentFloor.startY;

    PuzzleEngine puzzleEngine;
    bool running = true;

    while (running && !isDead(p) && p.currentFloor <= 10) {
        // Display current floor
        displayFloorInfo(p.currentFloor);
        displayHPBar(p);

        displayMap(currentFloor, pX, pY, gen); 
        int dx = 0, dy = 0;
        cout << "Use WASD to move (or 'i' for inventory, 'h' for hint, ESC to quit): ";
        char input = GETCH();
        cout << input << "\n";

        if (input == 27) break; // ESC

        if (input == 'i' || input == 'I') {
            displayInventory(p);
            cout << "\nPress any key to continue...";
            GETCH(); 
            continue;
        }
        if (input == 'h' || input == 'H') {
            // Allow hint usage outside room? Only inside room. Skip.
            continue;
        }

        // Movement
        if (input == 'w' || input == 'W') { dx = 0; dy = -1; }
        else if (input == 's' || input == 'S') { dx = 0; dy = 1; }
        else if (input == 'a' || input == 'A') { dx = -1; dy = 0; }
        else if (input == 'd' || input == 'D') { dx = 1; dy = 0; }
        else continue;

        if (!gen.handleMove(currentFloor, pX, pY, dx, dy)) {
            cout << "You can't move there!\n";
            continue;
        }

        // Check for room interaction
        Room* roomPtr = gen.getRoomAt(currentFloor, pX, pY);
        if (roomPtr && !roomPtr->isCleared) {
            displayRoomDescription(*roomPtr);
    
            if (roomPtr->type == RoomType::TREASURE) {
                runTreasure(p);
                gen.updateRoomStatus(currentFloor, pX, pY); 
            }
            else if (roomPtr->type == RoomType::BOSS) {
                runBossFight(p, *roomPtr, puzzleEngine);
                if (roomPtr->isCleared) {
                    gen.updateRoomStatus(currentFloor, pX, pY);
                }
            }
            else {
                bool solved = runPuzzle(p, *roomPtr, puzzleEngine);
                
                if (cin.fail() || cin.rdbuf()->in_avail() > 0 || true) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                }
                
                if (solved) {
                    cout << "\n>>> The path is clear. <<<\n";
                    gen.updateRoomStatus(currentFloor, pX, pY);
                } else {
                    cout << "\n>>> You take damage. <<<\n";
                    gen.updateRoomStatus(currentFloor, pX, pY);
                }
                
                cout << "Press any key to continue...";
                GETCH();
            }
        }

        // Check for stairs
        if (currentFloor.layout.grid[pY][pX] == TileType::STAIRS) {
            if (p.currentFloor >= 10) {
                running = false; // win
                break;
            }
            p.currentFloor++;
            cout << "\n========================================" << endl;
            cout << "   Floor cleared!" << endl;
            cout << "   Descending to Floor " << p.currentFloor << "..." << endl;
            cout << "========================================" << endl;
            cout << "Press any key to continue...";
            GETCH(); 
            currentFloor = gen.generateFloor(p.currentFloor, difficulty);
            pX = currentFloor.startX;
            pY = currentFloor.startY;
            cout << "You climb to the next floor!\n";
        }

        // Save game optionally (press 's' to save) – can be added to input loop
        if (input == 's') {
            saveFullGame(p, currentFloor);
        }

        if (isDead(p)) {
            displayDeathScreen(p);
            running = false;
        }
    }

    if (p.currentFloor > 10) {
        displayWinScreen(p);
    }
}

// =================== Puzzle Routing ===================
bool runPuzzle(Player& p, Room& dRoom, PuzzleEngine& engine) {
    cout << "\nA puzzle appears! Type: ";
    switch (dRoom.type) {
        case RoomType::ANAGRAM:      cout << "Anagram\n";
            return runAnagramPuzzle(p, dRoom.answer, engine);
        case RoomType::CAESAR:       cout << "Caesar\n";
            return runCaesarPuzzle(p, dRoom.answer, engine);
        case RoomType::SUBSTITUTION: cout << "Substitution\n";
            return runSubstitutionPuzzle(p, dRoom.answer, engine);
        case RoomType::WORD_CHAIN:   cout << "Word Chain\n";
            return runWordChainPuzzle(p, dRoom.answer, engine);
        case RoomType::HANGMAN:      cout << "Hangman\n";
            return runHangmanPuzzle(p, dRoom.answer);
        case RoomType::RIDDLE:       cout << "Riddle\n";
            return runRiddlePuzzle(p, engine);
        case RoomType::SPEED_ROUND:  cout << "Speed Round\n";
            return runSpeedRound(p, dRoom.answer, engine, dRoom.timeLimit > 0 ? dRoom.timeLimit : 30);
        default: return false;
    }
}

void handleInventoryInput(Player& p) {
    displayInventory(p); 
    if (p.inventory.empty()) {
        cout << "Press any key to return...";
        GETCH();
        return;
    }

    cout << "Select item to use (1-" << p.inventory.size() << ") or 0 to back: ";
    
    int choice;
    if (cin >> choice) {
        cin.ignore(1000, '\n'); 
        if (choice > 0 && choice <= (int)p.inventory.size()) {
            useItem(p, p.inventory[choice - 1].type);
            cout << "Press any key to continue...";
            GETCH();
        }
    } else {
        cin.clear();
        cin.ignore(1000, '\n');
    }
}

// =================== Individual Puzzles ===================
bool runAnagramPuzzle(Player& p, const string& answer, PuzzleEngine& engine) {
    string scrambled = engine.generateAnagram(answer);
    while (true) {
        cout << "Unscramble: " << scrambled << "\n";
        cout << "Actions: [A] Answer  [I] Inventory  [H] Hint\n";
        cout << "Choice: ";
        char choice = GETCH();
        cout << choice << "\n";

        if (choice == 'i' || choice == 'I') {
            handleInventoryInput(p);
            continue;
        }
        if (choice == 'h' || choice == 'H') {
            Room tempRoom = makeHintRoom(answer);
            useHint(p, tempRoom, 1);   
            continue;
        }
        if (choice == 'a' || choice == 'A') {
            string guess;
            cout << "Your guess: ";
            cin >> guess;
            cin.ignore(1000, '\n');
            if (engine.validateAnagram(answer, guess) && guess == answer) {
                onPuzzleSolved(p);
                return true;
            } else {
                onWrongGuess(p);
                return false;
            }
        }
    }
}

bool runCaesarPuzzle(Player& p, const string& answer, PuzzleEngine& engine) {
    int shift = 3; // fixed shift
    string cipher = engine.caesarCipher(answer, shift, true);
    while (true) {
        cout << "Cipher (shift " << shift << "): " << cipher << "\n";
        cout << "Actions: [A] Answer  [I] Inventory  [H] Hint\n";
        cout << "Choice: ";
        char choice = GETCH();
        cout << choice << "\n";

        if (choice == 'i' || choice == 'I') {
            handleInventoryInput(p);
            continue;
        }
        if (choice == 'h' || choice == 'H') {
            Room tempRoom = makeHintRoom(answer);
            useHint(p, tempRoom, 1);   
            continue;
        }
        if (choice == 'a' || choice == 'A') {
            string guess;
            cout << "Your guess: ";
            cin >> guess;
            cin.ignore(1000, '\n');
            if (guess == answer) {
                onPuzzleSolved(p);
                return true;
            } else {
                onWrongGuess(p);
                return false;
            }
        }
    }
}

bool runSubstitutionPuzzle(Player& p, const string& answer, PuzzleEngine& engine) {
    string cipher = engine.substitutionCipher(answer);
    while (true) {
        cout << "Substitution cipher: " << cipher << "\n";
        cout << "Actions: [A] Answer  [I] Inventory  [H] Hint\n";
        cout << "Choice: ";
        char choice = GETCH();
        cout << choice << "\n";

        if (choice == 'i' || choice == 'I') {
            handleInventoryInput(p);
            continue;
        }
        if (choice == 'h' || choice == 'H') {
            Room tempRoom = makeHintRoom(answer);
            useHint(p, tempRoom, 1);   
            continue;
        }
        if (choice == 'a' || choice == 'A') {
            string guess;
            cout << "Your guess: ";
            cin >> guess;
            cin.ignore(1000, '\n');
            if (guess == answer) {
                onPuzzleSolved(p);
                return true;
            } else {
                onWrongGuess(p);
                return false;
            }
        }
    }
}

bool runHangmanPuzzle(Player& p, const string& answer) {
    const int maxWrong = 6;
    int wrong = 0;
    string guessedWord(answer.length(), '_');
    string usedLetters;
    while (true) {
        cout << "Hangman! Word length: " << answer.length() << "\n";
        cout << "Word: " << guessedWord << "   Wrong: " << wrong << "/" << maxWrong << "\n";
        cout << "Actions: [G] Guess Letter  [I] Inventory  [H] Hint\n";
        cout << "Choice: ";
        char choice = GETCH();
        cout << choice << "\n";

        if (choice == 'i' || choice == 'I') {
            handleInventoryInput(p);
            continue;
        }
        if (choice == 'h' || choice == 'H') {
            Room tempRoom = makeHintRoom(answer);
            useHint(p, tempRoom, 1);   
            continue;
        }
        if (choice == 'g' || choice == 'G') {
            cout << "Enter letter: ";
            char c;
            cin >> c;
            cin.ignore(1000, '\n');
            c = tolower(c);

            if (usedLetters.find(c) != string::npos) {
                cout << "Already used.\n";
                continue;
            }
            usedLetters += c;

            bool found = false;
            for (size_t i = 0; i < answer.length(); ++i) {
                if (tolower(answer[i]) == c) {
                    guessedWord[i] = answer[i];
                    found = true;
                }
            }

            if (!found) {
                wrong++;
            }

            if (guessedWord == answer) {
                onPuzzleSolved(p);
                return true;
            }

            if (wrong >= maxWrong) {
                cout << "Word was: " << answer << "\n";
                onWrongGuess(p);
                return false;
            }
        }
    }
}

bool runRiddlePuzzle(Player& p, PuzzleEngine& engine) {
    auto riddle = engine.getRandomRiddle();
    while (true) {
        cout << "Riddle: " << riddle.question << "\n";
        cout << "Actions: [A] Answer  [I] Inventory  [H] Hint\n";
        cout << "Choice: ";
        char choice = GETCH();
        cout << choice << "\n";

        if (choice == 'i' || choice == 'I') {
            handleInventoryInput(p);
            continue;
        }
        if (choice == 'h' || choice == 'H') {
            Room tempRoom = makeHintRoom(riddle.answer);
            useHint(p, tempRoom, 1);   
            continue;
        }
        if (choice == 'a' || choice == 'A') {
            string guess;
            cout << "Answer: ";
            cin >> guess;
            cin.ignore(1000, '\n');
            if (guess == riddle.answer) {
                onPuzzleSolved(p);
                return true;
            } else {
                onWrongGuess(p);
                return false;
            }
        }
    }
}

bool runWordChainPuzzle(Player& p, const string& answer, PuzzleEngine& engine) {
    while (true) {
        cout << "Word Chain: " << answer << " -> ??? -> " << answer << "\n";
        cout << "Actions: [A] Answer  [I] Inventory  [H] Hint\n";
        cout << "Choice: ";
        char choice = GETCH();
        cout << choice << "\n";

        if (choice == 'i' || choice == 'I') {
            handleInventoryInput(p);
            continue;
        }
        if (choice == 'h' || choice == 'H') {
            Room tempRoom = makeHintRoom(answer);
            useHint(p, tempRoom, 1);   
            continue;
        }
        if (choice == 'a' || choice == 'A') {
            string middle;
            cout << "Enter the middle word (must differ by one letter): ";
            cin >> middle;
            cin.ignore(1000, '\n');
            vector<string> chain = { answer, middle, answer };
            if (engine.validateWordChain(chain)) {
                onPuzzleSolved(p);
                return true;
            } else {
                onWrongGuess(p);
                return false;
            }
        }
    }
}

bool runSpeedRound(Player& p, const string& answer, PuzzleEngine& engine, int timeLimit) {
    string scrambled = engine.generateAnagram(answer);
    auto start = chrono::steady_clock::now();

    while (true) {
        auto current = chrono::steady_clock::now();
        int secondsPassed = chrono::duration_cast<chrono::seconds>(current - start).count();
        int timeLeft = timeLimit - secondsPassed;

        if (timeLeft <= 0) {
            cout << "Time's up!\n";
            onWrongGuess(p);
            return false;
        }

        cout << "Speed Round! You have " << timeLeft << " seconds.\n";
        cout << "Unscramble this word: " << scrambled << "\n";
        cout << "Actions: [A] Answer  [I] Inventory  [H] Hint\n";
        cout << "Choice: ";

        char choice = GETCH();
        cout << choice << "\n";

        if (choice == 'i' || choice == 'I') {
            handleInventoryInput(p);
            continue;
        }
        if (choice == 'h' || choice == 'H') {
            Room tempRoom = makeHintRoom(answer);
            useHint(p, tempRoom, 1);
            continue;
        }
        if (choice == 'a' || choice == 'A') {
            string guess;
            cout << "Your guess: ";
            cin >> guess;
            cin.ignore(1000, '\n');

            auto end = chrono::steady_clock::now();
            int totalTime = chrono::duration_cast<chrono::seconds>(end - start).count();

            if (totalTime <= timeLimit && guess == answer) {
                onPuzzleSolved(p);
                return true;
            } else {
                if (totalTime > timeLimit) cout << "Too slow! ";
                onWrongGuess(p);
                return false;
            }
        }
    }
}

// =================== Treasure & Boss ===================
void runTreasure(Player& p) {
    int idx = rand() % 5;
    ItemType type = static_cast<ItemType>(idx);
    Item reward;
    reward.type = type;
    
    if (type == ItemType::DICTIONARY_SCROLL) { reward.name = "Dictionary Scroll"; reward.description = "Reveals word category."; }
    else if (type == ItemType::SKIP_TOKEN) { reward.name = "Skip Token"; reward.description = "Skip current room."; }
    else if (type == ItemType::REVEAL) { reward.name = "Reveal"; reward.description = "Shows 2 letters."; }
    else if (type == ItemType::FREEZE) { reward.name = "Freeze"; reward.description = "Freezes timer."; }
    else { reward.name = "Bandage"; reward.description = "Restore 2 HP."; }

    applyReward(p, reward);

    cout << "\n[TREASURE] You opened a chest!" << endl;
    cout << "You found: " << reward.name << endl;
    cout << "Effect: " << reward.description << endl;
    cout << "\nPress any key to continue...";
    GETCH();
}

void runBossFight(Player& p, Room& bossRoom, PuzzleEngine& engine) {
    drawASCII("boss");
    cout << "BOSS FIGHT! Defeat the champion in three phases.\n";
    // Phase 1: Riddle
    cout << "Phase 1: Riddle\n";
    if (!runRiddlePuzzle(p, engine)) { onWrongGuess(p); return; }
    // Phase 2: Caesar cipher
    cout << "Phase 2: Caesar Cipher\n";
    if (!runCaesarPuzzle(p, bossRoom.answer , engine)) { onWrongGuess(p); return; }
    // Phase 3: Anagram
    cout << "Phase 3: Anagram\n";
    if (!runAnagramPuzzle(p, bossRoom.answer , engine)) { onWrongGuess(p); return; }
    // Victory
    onPuzzleSolved(p);
    bossRoom.isCleared = true;
}

// =================== Hint Generation ===================
Room makeHintRoom(const string& answer) {
    Room hr;
    hr.answer = answer;
    hr.hint1 = "Vague: The word starts with '" + string(1, answer[0]) + "'";
    hr.hint2 = "Medium: The word has " + to_string(answer.length()) + " letters.";
    hr.hint3 = "Close: The first two letters are '" + answer.substr(0,2) + "'";
    return hr;
}

// =================== Display Helpers ===================
void displayHPBar(const Player& p) {
    cout << "\n[HP: ";
    for (int i = 0; i < p.hp; i++) cout << "\u2588";
    for (int i = p.hp; i < p.maxHp; i++) cout << "\u2591";
    cout << " " << p.hp << "/" << p.maxHp << "]  Floor: " << p.currentFloor
         << "  Score: " << p.score << "\n";
}

void displayRoomDescription(const Room& r) {
    cout << "\nYou enter a " << r.category << " room.\n";
    switch (r.type) {
        case RoomType::ANAGRAM: cout << "Letters are scrambled.\n"; break;
        case RoomType::CAESAR: cout << "A cipher hides the word.\n"; break;
        case RoomType::HANGMAN: cout << "Fill in the blanks.\n"; break;
        case RoomType::RIDDLE: cout << "Answer the riddle.\n"; break;
        case RoomType::SPEED_ROUND: cout << "Hurry! Time is ticking.\n"; break;
        case RoomType::TREASURE: cout << "A glowing chest awaits.\n"; break;
        case RoomType::BOSS: cout << "The air crackles with power.\n"; break;
        default: break;
    }
}

void drawASCII(const string& type) {
    if (type == "title") {
        cout << R"(
 __          __           _    ____
 \ \        / /          | |  |  _ \
  \ \  /\  / /__  _ __ __| |  | | | |
   \ \/  \/ / _ \| '__/ _` |  | | | |
    \  /\  / (_) | | | (_| |  | |_| |
     \/  \/ \___/|_|  \__,_|  |____/
)" << endl;
    } else if (type == "boss") {
        cout << R"(
   /| ________________
O|===|* >________________>
   \|
)" << endl;
    }
}

void displayDeathScreen(const Player& p) {
    cout << "You have perished in the dungeon.\n";
    cout << "Final Score: " << calculateScore(p) << "\n";
}

void displayWinScreen(const Player& p) {
    cout << "Congratulations! You escaped the Word Dungeon!\n";
    cout << "Final Score: " << calculateScore(p) << "\n";
}

void displayFloorInfo(int floor) {
    cout << "\n--- Floor " << floor << " ---\n";
}

// =================== Save/Load Extension ===================
void saveFullGame(const Player& p, const Floor& f) {
    // First save base state using P4's function (already done in main)
    saveGameState(p, f);
    // Now save inventory to a separate file or append. Let's use inventory.txt
    ofstream finv("inventory.txt");
    for (const auto& item : p.inventory) {
        finv << static_cast<int>(item.type) << " " << item.name << " " << item.description << "\n";
    }
    finv.close();
    cout << "Full game saved.\n";
}

bool loadFullGame(Player& p, Floor& f, DungeonGenerator& gen, int difficulty) {
    if (!loadGameState(p, f)) return false;
    // Regenerate the floor layout and rooms (since they weren't saved)
    f = gen.generateFloor(p.currentFloor, difficulty);
    // Load inventory
    ifstream inv("inventory.txt");
    if (inv.is_open()) {
        p.inventory.clear();
        int type;
        string name, desc;
        while (inv >> type) {
            inv.ignore();
            getline(inv, name, ' ');
            getline(inv, desc);
            Item itm;
            itm.type = static_cast<ItemType>(type);
            itm.name = name;
            itm.description = desc;
            p.inventory.push_back(itm);
        }
        inv.close();
    }
    return true;
}

void pauseForUser() {
    cout << "\nPress any key to continue...";
    GETCH();
} // call this whenever need pause