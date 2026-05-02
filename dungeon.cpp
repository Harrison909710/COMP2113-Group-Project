#include "dungeon.h"
#include "database.h" // for randomized words
#include "puzzle_engine.h" // for puzzle data
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>

using namespace std;

// What it does: Initializes the generator with map dimensions and seeds the random number generator.
// What the inputs are: w (width), h (height) of the dungeon grid.
// What the outputs are: None (Constructor).
DungeonGenerator::DungeonGenerator(int w, int h) : defaultWidth(w), defaultHeight(h) {
    srand(static_cast<unsigned int>(time(0)));
}

// What it does: Generates an entire floor, including puzzle rooms and treasure rooms.
// What the inputs are: floorNumber (current depth), chosenDifficulty (game difficulty level).
// What the outputs are: A Floor object containing the complete floor data.
Floor DungeonGenerator::generateFloor(int floorNumber, int chosenDifficulty) {
    Floor f;
    f.floorNumber = floorNumber;
    f.layout.width = defaultWidth;
    f.layout.height = defaultHeight;

    f.hasTreasure = (floorNumber < 10);

    initializeMap(f.layout);
    f.startX = defaultWidth / 2;
    f.startY = defaultHeight / 2;   // start in middle grid

    if (floorNumber == 10) {
        f.rooms.push_back(generateRoom(floorNumber, 1, RoomType::BOSS, chosenDifficulty));
    } else {
        // 1. Guaranteed Treasure Room
        f.rooms.push_back(generateRoom(floorNumber, 99, RoomType::TREASURE, chosenDifficulty));

        // 2. Base Puzzle Rooms (e.g., Easy: 2, Normal: 3, Hard: 3)
        int puzzleCount = (chosenDifficulty == 1) ? 2 : 3;
        for (int i = 1; i <= puzzleCount; i++) {
            f.rooms.push_back(generateRoom(floorNumber, i, pickRoomType(floorNumber, i), chosenDifficulty));
        }

        // 3. Hard Mode Extra Room: 80% Puzzle, 20% Treasure
        if (chosenDifficulty == 3) {
            RoomType extraType = (rand() % 100 < 20) ? RoomType::TREASURE : pickRoomType(floorNumber, 100);
                f.rooms.push_back(generateRoom(floorNumber, 100, extraType, chosenDifficulty));
        }
    }

    placeElements(f);
    return f;
}

// What it does: Creates an individual room with specific data like puzzle type and mystery words.
// What the inputs are: floorNumber (current floor), roomID (unique ID), type (RoomType enum), chosenDifficulty (difficulty level).
// What the outputs are: A Room struct populated with game data.
Room DungeonGenerator::generateRoom(int floorNumber, int roomID, RoomType type, int chosenDifficulty) {
    Room r;
    r.roomID = roomID;
    r.type = type;
    r.isCleared = false;
    r.difficulty = chosenDifficulty;

    if (type == RoomType::BOSS) {
        r.category = "Boss";
        int bossLevel = (chosenDifficulty < 3) ? (chosenDifficulty + 1) : 3; // give one boss word, difficulty = chosen + 1 if hard still = 3
        r.answer = getRandomWord(bossLevel);
        r.puzzleData = "BOSS_FIGHT";
        return r;
    }

    if (type == RoomType::TREASURE) {
        r.category = "Treasure";
        r.answer = "";
        r.puzzleData = "TREASURE";
        return r;
    }

    if (type == RoomType::RIDDLE) {
        r.category = "Riddle";
        r.answer = "";
        r.puzzleData = "RIDDLE";
    } else {
        r.category = "Puzzle";
        r.answer = getRandomWord(chosenDifficulty);
        r.puzzleData = "PUZZLE";
    }

    if (type == RoomType::SPEED_ROUND) {
        if (chosenDifficulty == 1) r.timeLimit = 120;
        else if (chosenDifficulty == 2) r.timeLimit = 90;
        else r.timeLimit = 60;
    }

    return r;
}

// What it does: Creates the initial grid filled with floor tiles and boundary walls.
// What the inputs are: m (Map reference).
// What the outputs are: None (Initializes the grid vector).
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

// What it does: Randomly assigns coordinates to rooms on the map without overlapping.
// What the inputs are: f (Floor reference).
// What the outputs are: None (Assigns x/y to each room and updates the grid).
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

// What it does: Randomly selects a room type based on the current floor number.
// What the inputs are: floorNumber (current depth), index (room sequence index).
// What the outputs are: A RoomType enum value representing the selected room type.
RoomType DungeonGenerator::pickRoomType(int floorNumber, int index) {
    // Speed Round appears after floor 5 with 15% chance
    if (floorNumber > 5 && (rand() % 100 < 15)) {
        return RoomType::SPEED_ROUND;
    }

    // Remaining 85% (or 100% before floor 5) split equally among first 6 types
    return static_cast<RoomType>(rand() % 6);
}

// What it does: Checks if the current floor number is a designated boss floor.
// What the inputs are: floorNumber (integer).
// What the outputs are: Boolean value (true if it is a boss floor).
bool DungeonGenerator::isBossFloor(int floorNumber) {
    return (floorNumber > 0 && floorNumber % 10 == 0);
}

// What it does: Places the exit stairs on the map grid to allow progression.
// What the inputs are: f (Floor reference).
// What the outputs are: None (Modifies the Map grid tiles).
void DungeonGenerator::spawnStairs(Floor& f) {
    if (f.floorNumber < 10) {
        f.layout.grid[f.startY][f.startX] = TileType::STAIRS;
    }
}

// What it does: Marks a room as cleared and checks if all rooms are cleared to spawn stairs.
// What the inputs are: f (Floor reference), x/y (Coordinates of the cleared room).
// What the outputs are: None (Updates the Floor object state).
void DungeonGenerator::updateRoomStatus(Floor& f, int x, int y) {
    Room* r = getRoomAt(f, x, y);
    if (r) {
        r->isCleared = true;
        f.layout.grid[y][x] = TileType::FLOOR;
    }

    bool allCleared = true;
    for (const auto& rm : f.rooms) {
        if (rm.type != RoomType::TREASURE && !rm.isCleared) {
            allCleared = false;
            break;
        }
    }

    if (allCleared) {
        spawnStairs(f);
    }
}

// What it does: Validates player movement and updates coordinates if no collision occurs.
// What the inputs are: f (Floor reference), pX/pY (Player coordinates by reference), moveX/moveY (Movement vector).
// What the outputs are: Boolean value (true if the movement was successful).
bool DungeonGenerator::handleMove(Floor& f, int& pX, int& pY, int moveX, int moveY) {
    int nextX = pX + moveX;
    int nextY = pY + moveY;

    if (nextX < 0 || nextX >= f.layout.width || nextY < 0 || nextY >= f.layout.height) return false;
    if (f.layout.grid[nextY][nextX] == TileType::WALL) return false;

    pX = nextX;
    pY = nextY;

    return true;
}

// What it does: Searches for a room object at specific grid coordinates.
// What the inputs are: f (Floor reference), x/y (Grid coordinates).
// What the outputs are: A pointer to the Room object found, or nullptr if none exists.
Room* DungeonGenerator::getRoomAt(Floor& f, int x, int y) {
    for (auto& r : f.rooms) {
        if (r.x == x && r.y == y) return &r;
    }
    return nullptr;
}
