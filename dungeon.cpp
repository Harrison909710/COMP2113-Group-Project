#include "dungeon.h"
#include "database.h"
#include "puzzle_engine.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <conio.h>
using namespace std;

DungeonGenerator::DungeonGenerator(int w, int h) : defaultWidth(w), defaultHeight(h) {
    srand(static_cast<unsigned int>(time(0)));
}

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
    if (f.floorNumber < 10) {
        f.layout.grid[f.startY][f.startX] = TileType::STAIRS;
    }
}


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

bool DungeonGenerator::handleMove(Floor& f, int& pX, int& pY, int moveX, int moveY) {
    int nextX = pX + moveX;
    int nextY = pY + moveY;

    if (nextX < 0 || nextX >= f.layout.width || nextY < 0 || nextY >= f.layout.height) return false;
    if (f.layout.grid[nextY][nextX] == TileType::WALL) return false;

    pX = nextX;
    pY = nextY;

    Room* r = getRoomAt(f, pX, pY);
    if (r && !r->isCleared) {
        updateRoomStatus(f, pX, pY);
    }

    return true;
}

Room* DungeonGenerator::getRoomAt(Floor& f, int x, int y) {
    for (auto& r : f.rooms) {
        if (r.x == x && r.y == y) return &r;
    }
    return nullptr;
}
