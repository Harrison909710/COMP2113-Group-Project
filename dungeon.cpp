#include "dungeon.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <conio.h> // For _getch()
using namespace std;

DungeonGenerator::DungeonGenerator(int w, int h) : defaultWidth(w), defaultHeight(h) {
    srand(static_cast<unsigned int>(time(0)));
}

Floor DungeonGenerator::generateFloor(int floorNumber, int chosenDifficulty) {
    Floor f;
    f.floorNumber = floorNumber;
    f.layout.width = defaultWidth;
    f.layout.height = defaultHeight;

    initializeMap(f.layout);
    f.startX = defaultWidth / 2;
    f.startY = defaultHeight / 2;   // start in middle grid

    if (floorNumber == 10) {
        f.rooms.push_back(generateRoom(floorNumber, 1, RoomType::BOSS, chosenDifficulty));
    } else {
        // 1. Guaranteed Treasure Room
        f.rooms.push_back(generateRoom(floorNumber, 99, RoomType::TREASURE, chosenDifficulty));

        // 2. Base Puzzle Rooms (e.g., Simple: 2, Normal: 3, Hard: 3)
        int puzzleCount = (chosenDifficulty == 0) ? 2 : 3;
        for (int i = 1; i <= puzzleCount; i++) {
            f.rooms.push_back(generateRoom(floorNumber, i, pickRoomType(floorNumber, i), chosenDifficulty));
        }

        // 3. Hard Mode Extra Room: 80% Puzzle, 20% Treasure
        if (chosenDifficulty == 2) {
            RoomType extraType = (rand() % 100 < 20) ? RoomType::TREASURE : pickRoomType(floorNumber, 100);
                f.rooms.push_back(generateRoom(floorNumber, 100, extraType, chosenDifficulty));
        }
    }

    placeElements(f);
    return f;
}

Room DungeonGenerator::generateRoom(int floorNumber, int roomID, RoomType type, int chosenDifficulty) {
    Room r;
    r.roomID = roomID;
    r.type = type;
    r.isCleared = false;
    r.difficulty = chosenDifficulty;

    if (type == RoomType::TREASURE) {
        r.category = "Treasure";
        // Directly call Person 4's database to get an item name
        // r.puzzleData = Database::getRandomItem(chosenDifficulty);
        r.puzzleData = "A mysterious item";
        r.answer = "";
    }
    else if (type == RoomType::BOSS) {
        r.category = "Boss";
        r.puzzleData = "ULTIMATE PUZZLE";
        r.answer = "CHAMPION";
    }
    else {
        r.category = "Puzzle";
        // 1. Get raw word from Person 4
        // string rawWord = Database::getRandomWord(chosenDifficulty);

        // 2. Set answer as the raw word
        // r.answer = rawWord;

        // 3. Call Person 1's functions based on type to fill puzzleData
        /*
        switch(type) {
            case RoomType::ANAGRAM: r.puzzleData = PuzzleEngine::scrambleWord(rawWord); break;
            case RoomType::CAESAR:  r.puzzleData = PuzzleEngine::applyCaesarCipher(rawWord, 3); break;
            // ... and so on for all 6 types
        }
        */
        r.puzzleData = "SCRAMBLED_WORD_HERE"; // PLACEHOLDER
        r.answer = "WORD";                     // PLACEHOLDER
    }
    return r;
}

void DungeonGenerator::initializeMap(Map& m) {
    m.grid.assign(m.height, std::vector<TileType>(m.width, TileType::FLOOR));
    for (int y = 0; y < m.height; y++) {
        for (int x = 0; x < m.width; x++) {
            if (y == 0 || y == m.height - 1 || x == 0 || x == m.width - 1) {
                m.grid[y][x] = TileType::WALL;
            } else {
                m.grid[y][x] = TileType::FLOOR;
            }
        }
    }
}

void DungeonGenerator::placeElements(Floor& f) {
    for (auto& room : f.rooms) {
        bool placed = false;
        while (!placed) {
            // Randomly select coordinates within the walls
            int rx = 1 + rand() % (f.layout.width - 2);
            int ry = 1 + rand() % (f.layout.height - 2);

            // Constraint: Room cannot be at the center (player start)
            if (rx == f.startX && ry == f.startY) continue;

            // Constraint: Room cannot overlap with another ROOM_TILE
            if (f.layout.grid[ry][rx] == TileType::ROOM_TILE) continue;

            // Place the room
            room.x = rx;
            room.y = ry;
            f.layout.grid[ry][rx] = TileType::ROOM_TILE;
            placed = true;
        }
    }
}

RoomType DungeonGenerator::pickRoomType(int floorNumber, int index) {
    // Speed Round appears after floor 5 with 15% chance
    if (floorNumber > 5 && (rand() % 100 < 15)) {
        return RoomType::SPEED_ROUND;
    }

    // Remaining 85% (or 100% before floor 5) split equally among first 6 types
    return static_cast<RoomType>(rand() % 6);
}

bool DungeonGenerator::isBossFloor(int floorNumber) {
    return (floorNumber > 0 && floorNumber % 10 == 0);
}

void DungeonGenerator::spawnStairs(Floor& f) {
    if (f.floorNumber == 10) {
        // Person 5 will detect this and trigger Win Screen / Score Calculation
        std::cout << "DUNGEON CLEAR! Preparing final score..." << std::endl;
        return;
    }
    // Normal behavior: spawn stairs at center
    f.layout.grid[f.startY][f.startX] = TileType::STAIRS;
}


void DungeonGenerator::updateRoomStatus(Floor& f, int x, int y) {
    Room* r = getRoomAt(f, x, y);
    if (r) {
        r->isCleared = true; // This makes the room "disappear" from the map
    }

    // Existing logic to check all rooms
    bool allCleared = true;
    for (const auto& rm : f.rooms) {
        if (!rm.isCleared) {
            allCleared = false;
            break;
        }
    }

    if (allCleared) {
        spawnStairs(f); // Automatically spawn stairs at (5,5)
    }
}

// Returns true if the move was successful (not a wall)
bool DungeonGenerator::handleMove(Floor& f, int& pX, int& pY, int moveX, int moveY) {
    int nextX = pX + moveX;
    int nextY = pY + moveY;

    // 1. Boundary and Wall check
    if (nextX < 0 || nextX >= f.layout.width || nextY < 0 || nextY >= f.layout.height) return false;
    if (f.layout.grid[nextY][nextX] == TileType::WALL) return false;

    // 2. If it's not a wall, update player position
    pX = nextX;
    pY = nextY;

    // 3. Check for Room Interaction
    if (f.layout.grid[pY][pX] == TileType::ROOM_TILE) {
        Room* r = getRoomAt(f, pY, pX);
        if (r && !r->isCleared) {
            // Here the logic returns to Main/UI to start the puzzle
            // The UI person will see pX, pY is on a ROOM_TILE and trigger the screen
            std::cout << "Entering " << r->category << " ID: " << r->roomID << std::endl;
        }
    }

    // 4. Check for Stairs Interaction
    if (f.layout.grid[pY][pX] == TileType::STAIRS) {
        std::cout << "Climbing to next floor..." << std::endl;
    }

    return true;
}

Room* DungeonGenerator::getRoomAt(Floor& f, int x, int y) {
    for (auto& r : f.rooms) {
        if (r.x == x && r.y == y) return &r;
    }
    return nullptr;
}

int main() {
    DungeonGenerator generator(11, 11);
    int pX, pY, floorNum = 1, difficulty = 1;

    // Initialize Floor
    Floor currentFloor = generator.generateFloor(floorNum, difficulty);
    pX = currentFloor.startX;
    pY = currentFloor.startY;

    bool isRunning = true;
    while (isRunning) {
        // 1. New WASD Input (Replaced the static dx=1)
        int dx = 0, dy = 0;
        char input = _getch(); // Catch keyboard input

        if (input == 'w' || input == 'W') dy = -1;
        else if (input == 's' || input == 'S') dy = 1;
        else if (input == 'a' || input == 'A') dx = -1;
        else if (input == 'd' || input == 'D') dx = 1;
        else if (input == 27) break; // ESC key to quit

        // 2. Original handleMove logic (STAYED THE SAME)
        if (generator.handleMove(currentFloor, pX, pY, dx, dy)) {

            // 3. Original Room Interaction (STAYED THE SAME)
            Room* r = generator.getRoomAt(currentFloor, pX, pY);
            if (r && !r->isCleared) {
                generator.updateRoomStatus(currentFloor, pX, pY);
            }

            // 4. Original Stairs Check (STAYED THE SAME)
            if (currentFloor.layout.grid[pY][pX] == TileType::STAIRS) {
                if (floorNum >= 10) isRunning = false;
                else {
                    floorNum++;
                    currentFloor = generator.generateFloor(floorNum, difficulty);
                    pX = currentFloor.startX;
                    pY = currentFloor.startY;
                }
            }
        }

        // 5. Original Failsafe (STAYED THE SAME)
        if (floorNum > 10) break;
    }
    return 0;
}
