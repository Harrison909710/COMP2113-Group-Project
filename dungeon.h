#ifndef DUNGEON_H
#define DUNGEON_H

#include <vector>
#include <string>

//definitions

enum class TileType {
    WALL,
    FLOOR,
    ROOM_TILE,
    STAIRS
};

enum class RoomType {
    ANAGRAM,
    CAESAR,
    SUBSTITUTION,
    WORD_CHAIN,
    HANGMAN,
    RIDDLE,
    SPEED_ROUND,
    TREASURE,
    BOSS
};

struct Map {
    int width;
    int height;
    std::vector<std::vector<TileType>> grid;
};

struct Room {
    int roomID;
    int x, y;
    RoomType type;
    std::string puzzleData;
    std::string answer;
    std::string category;
    int difficulty;
    int timeLimit = 0;
    bool isCleared = false;
    std::string hint1;
    std::string hint2;
    std::string hint3;
    bool completed = false;

};

struct Floor {
    int floorNumber;
    Map layout;
    std::vector<Room> rooms;
    int startX, startY;
    bool hasTreasure;
};

//function implemented
class DungeonGenerator {
public:
    DungeonGenerator(int w = 11, int h = 11);
    Floor generateFloor(int floorNumber, int chosenDifficulty);   //Generates a complete floor with 3 rooms + optional treasure
    Room generateRoom(int floorNumber, int roomID, RoomType type, int chosenDifficulty);     //Creates one random room for the current floor
    RoomType pickRoomType(int floorNumber, int index);     //Picks room type with weighted probability based on floor depth
    bool isBossFloor(int floorNumber);      //Checks if this floor is a boss floor

    // Logic functions for movement and rules
    bool handleMove(Floor& f, int& pX, int& pY, int moveX, int moveY);
    Room* getRoomAt(Floor& f, int x, int y);
    void updateRoomStatus(Floor& f, int x, int y);
    void spawnStairs(Floor& f);

private:
    int defaultWidth;
    int defaultHeight;

    void initializeMap(Map& m);
    void placeElements(Floor& f);
};

#endif
