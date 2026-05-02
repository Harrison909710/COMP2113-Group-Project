#ifndef DUNGEON_H
#define DUNGEON_H

#include "puzzle_engine.h"
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
    // What it does: Initializes the generator with map dimensions and seeds the random number generator.
    // What the inputs are: w (width), h (height) of the dungeon grid.
    // What the outputs are: None (Constructor).
    DungeonGenerator(int w = 11, int h = 11);

    // What it does: Generates an entire floor, including puzzle rooms and treasure rooms.
    // What the inputs are: floorNumber (current depth), chosenDifficulty (game difficulty level).
    // What the outputs are: A Floor object containing the complete floor data.
    Floor generateFloor(int floorNumber, int chosenDifficulty);   

    // What it does: Creates an individual room with specific data like puzzle type and mystery words.
    // What the inputs are: floorNumber (current floor), roomID (unique ID), type (RoomType enum), chosenDifficulty (difficulty level).
    // What the outputs are: A Room struct populated with game data.
    Room generateRoom(int floorNumber, int roomID, RoomType type, int chosenDifficulty); 

    // What it does: Randomly selects a room type based on the current floor number.
    // What the inputs are: floorNumber (current depth), index (room sequence index).
    // What the outputs are: A RoomType enum value representing the selected room type.
    RoomType pickRoomType(int floorNumber, int index);

    // What it does: Checks if the current floor number is a designated boss floor.
    // What the inputs are: floorNumber (integer).
    // What the outputs are: Boolean value (true if it is a boss floor).
    bool isBossFloor(int floorNumber);

    // Logic functions for movement and rules

    // What it does: Validates player movement and updates coordinates if no collision occurs.
    // What the inputs are: f (Floor reference), pX/pY (Player coordinates by reference), moveX/moveY (Movement vector).
    // What the outputs are: Boolean value (true if the movement was successful).
    bool handleMove(Floor& f, int& pX, int& pY, int moveX, int moveY);

    // What it does: Searches for a room object at specific grid coordinates.
    // What the inputs are: f (Floor reference), x/y (Grid coordinates).
    // What the outputs are: A pointer to the Room object found, or nullptr if none exists.
    Room* getRoomAt(Floor& f, int x, int y);

    // What it does: Marks a room as cleared and checks if all rooms are cleared to spawn stairs.
    // What the inputs are: f (Floor reference), x/y (Coordinates of the cleared room).
    // What the outputs are: None (Updates the Floor object state).
    void updateRoomStatus(Floor& f, int x, int y);

    // What it does: Places the exit stairs on the map grid to allow progression.
    // What the inputs are: f (Floor reference).
    // What the outputs are: None (Modifies the Map grid tiles).
    void spawnStairs(Floor& f);

private:
    int defaultWidth;
    int defaultHeight;

    // What it does: Creates the initial grid filled with floor tiles and boundary walls.
    // What the inputs are: m (Map reference).
    // What the outputs are: None (Initializes the grid vector).
    void initializeMap(Map& m);

    // What it does: Randomly assigns coordinates to rooms on the map without overlapping.
    // What the inputs are: f (Floor reference).
    // What the outputs are: None (Assigns x/y to each room and updates the grid).
    void placeElements(Floor& f);
};

#endif
