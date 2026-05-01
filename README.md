# COMP2113-Group-Project
# Word Dungeon
A terminal-based dungeon crawler where your weapon is your vocabulary. Explore dungeon floors, solve word puzzles, collect items, manage your HP, and defeat the boss on Floor 10 to escape.

---

## Team Members

| Name | Student ID | Role |
|---|---|---|
| (Member 1 — fill in) | (ID) | Database / Save-Load / Word System |
| (Member 2 — fill in) | (ID) | Dungeon Generation / Movement Logic |
| (Member 3 — fill in) | (ID) | Player / Inventory / Scoring |
| (Member 4 — fill in) | (ID) | UI / Game Loop / Puzzle Routing |
| (Member 5 — Liu Liyang) | 3035552529 | Puzzle Engine (all puzzle types + timer) |

> Note: Replace the placeholder names and IDs above with your actual team information before submission.

---

## Game Description

Word Dungeon is a roguelike word-puzzle game played entirely in the terminal. The player navigates an 11×11 grid dungeon floor by floor, enters rooms that present different word challenges, earns score by solving puzzles, and loses HP after wrong guesses. The player must clear the rooms on each floor, use the spawned stairs to advance, and survive until Floor 10 to defeat the final boss.

### Core Loop
1. Move around the floor with WASD
2. Enter a room and solve its word puzzle
3. Clear puzzle rooms so that stairs can appear
4. Advance to the next floor
5. Reach Floor 10 and clear the Boss room
6. Finish the run and save the score to the leaderboard

---

## Features and Coding Requirements

Below, each major game feature is explained using the official project requirements: generation of random events, data structures for storing data, dynamic memory management, file input/output, program codes in multiple files, and multiple difficulty levels.

---

### 1. Procedural Dungeon Generation

Rooms are generated for each floor at runtime. The dungeon uses an 11×11 map, stores rooms in a room list, and places special content such as puzzle rooms, treasure rooms, and the boss room according to floor number and difficulty.

- **Generation of random events:** room types and room content are generated during play, so each run can have a different room arrangement and puzzle order.
- **Data structures for storing data:** the game uses `Map`, `Room`, and `Floor` structs, as well as `vector<vector<TileType>>` for the grid and `vector<Room>` for the room list.
- **Dynamic memory management:** the dungeon layout and room collections are stored in `std::vector`, which uses dynamic storage at runtime.
- **Program codes in multiple files:** dungeon generation and movement logic are separated into `dungeon.h` and `dungeon.cpp`.

---

### 2. WASD Movement and Collision

The player moves one tile at a time. Movement is blocked by walls and map boundaries. Stepping onto a room tile starts a puzzle, while stepping onto stairs advances to the next floor.

- **Data structures for storing data:** tile types are represented by the `TileType` enum class, and the map is stored in the `Map` struct.
- **Program codes in multiple files:** movement and room lookup are handled in `dungeon.cpp`, while player state is handled separately in `player.cpp`.

---

### 3. Seven Puzzle Types

The game includes multiple puzzle types: Anagram, Caesar Cipher, Substitution Cipher, Word Chain, Hangman, Riddle, and Speed Round.

- **Generation of random events:** puzzle type and puzzle word selection vary between rooms and floors.
- **Data structures for storing data:** puzzle-related information is stored in `RoomType`, `Room`, and the `PuzzleEngine` class.
- **Dynamic memory management:** word lists and puzzle data are stored in containers such as `std::vector` and `std::unordered_map`, which grow dynamically at runtime.
- **Program codes in multiple files:** puzzle logic is separated into `puzzle_engine.h`, `puzzle_engine.cpp`, `ui.h`, and `ui.cpp`.

---

### 4. Player Stats, HP, and Inventory

The player has HP, maximum HP, score, floor progress, wrong-guess tracking, and an inventory that stores items collected from treasure rooms.

- **Data structures for storing data:** the game uses `Player`, `Item`, `ItemType`, and `HintTier` to store player state and item data.
- **Dynamic memory management:** the inventory is stored in `std::vector<Item>`, which expands dynamically when items are added.
- **Program codes in multiple files:** player logic is implemented in `player.h` and `player.cpp`.

---

### 5. Hint System

During puzzles, the player can request hints. Hint tiers such as Vague, Medium, and Close provide increasing levels of help.

- **Data structures for storing data:** hint information is represented through `HintTier`, and room or puzzle state is stored in related structures.
- **Program codes in multiple files:** hint handling is split between the player module and the UI / puzzle flow.

---

### 6. Items and Treasure Rooms

Treasure rooms reward the player with useful items such as Bandage, Freeze, Reveal, Skip Token, and Dictionary Scroll.

- **Generation of random events:** treasure rewards are part of the game's randomized room content.
- **Data structures for storing data:** item data is represented using the `Item` struct and `ItemType` enum class.
- **Dynamic memory management:** collected items are stored in the player's inventory vector.
- **Program codes in multiple files:** item effects are implemented in the player module, while treasure room generation belongs to the dungeon and UI logic.

---

### 7. Boss Fight (Floor 10)

Floor 10 contains a special Boss room that acts as the final challenge of the game.

- **Generation of random events:** earlier floors are generated dynamically, while Floor 10 changes the normal flow by forcing a boss encounter.
- **Data structures for storing data:** the boss room is represented through the same room and room-type structures used by the dungeon system.
- **Program codes in multiple files:** the boss sequence is coordinated through the UI and puzzle systems.

---

### 8. Save and Load System

The game can save progress and load it later. The current implementation stores player data and basic floor metadata in `save.txt`.

- **File input/output:** `database.cpp` reads and writes game state using `ifstream` and `ofstream`.
- **Program codes in multiple files:** save/load logic is placed in `database.h` and `database.cpp`, separate from UI and player logic.

---

### 9. Leaderboard

After a run, the player's score is stored in `leaderboard.txt`. The leaderboard can be loaded, displayed, sorted, and trimmed to keep the highest scores.

- **File input/output:** leaderboard data is read from and written to `leaderboard.txt`.
- **Dynamic memory management:** leaderboard entries are stored in `vector<ScoreEntry>`.
- **Program codes in multiple files:** leaderboard logic is implemented in the database module.

---

### 10. Word Database by Difficulty

The game uses three word-list files: `words_easy.txt`, `words_medium.txt`, and `words_hard.txt`. These are loaded when the program starts and are used to select puzzle words by difficulty.

- **File input/output:** words are loaded from text files at runtime.
- **Multiple difficulty levels:** the code supports word selection by difficulty level through `getRandomWord(int difficulty)`.
- **Dynamic memory management:** the loaded word lists are stored in `vector<string>`.
- **Program codes in multiple files:** word database logic is implemented in `database.h` and `database.cpp`.

---

## Difficulty Levels

The project includes multiple difficulty levels, which is one of the official project requirements.

- **Easy:** easier word list and more forgiving gameplay
- **Medium:** balanced difficulty
- **Hard:** harder word list and more challenging gameplay

Difficulty affects dungeon generation and word selection through the current code structure.

---

## Non-Standard Libraries

| Library Header | Usage in Game |
|---|---|
| `conio.h` | `_getch()` is used to read single keystrokes for movement and menu input without requiring Enter. |

> All other libraries used are standard C++ libraries, such as `<iostream>`, `<fstream>`, `<vector>`, `<string>`, `<algorithm>`, `<random>`, `<unordered_map>`, `<chrono>`, `<thread>`, and `<cctype>`.
>
The project is designed to be cross-platform, supporting both Windows and Linux.
* **Windows**: Uses <conio.h> for direct keyboard input via _getch().
* **Linux**: Implements a custom getch_linux() using <termios.h> and <unistd.h> to simulate the same behavior (reading keystrokes without pressing Enter).
> The code automatically detects the operating system at compile time using the _WIN32 macro and maps the appropriate function to GETCH().

---

## Compilation and Execution (Quick Start)

### Prerequisites

- OS: Windows/Linux
- Compiler: g++ with C++17 support
- Make: GNU Make

### Step 1 — Clone / download the repository

```bash
git clone <your-repo-url>
cd COMP2113-Group-Project
```

### Step 2 — Ensure required text files are present

The following files should remain in the project directory:

```text
words_easy.txt
words_medium.txt
words_hard.txt
leaderboard.txt
```

### Step 3 — Compile

Using Make:

```bash
make
```

Using g++ directly:

```bash
g++ -std=c++17 -Wall -o word_dungeon.exe main.cpp ui.cpp dungeon.cpp player.cpp puzzle_engine.cpp database.cpp
```

### Step 4 — Run

```bash
./word_dungeon.exe
```

### Step 5 — Play

Example main menu:

```text
===== WORD DUNGEON =====
1. New Game
2. Continue
3. Leaderboard
4. Quit
```

- Choose **New Game** to start a run
- Use **W A S D** to move around the dungeon
- Enter puzzle rooms and solve the challenges
- Use items and hints carefully to survive
- Save your progress when needed

### Clean build artifacts

```bash
make clean
```

---

## File List

| File | Description |
|---|---|
| `main.cpp` | Entry point and top-level program start |
| `ui.h` / `ui.cpp` | Game loop, display functions, puzzle routing |
| `dungeon.h` / `dungeon.cpp` | Floor generation, map logic, movement, room lookup |
| `player.h` / `player.cpp` | Player stats, HP, inventory, scoring, item logic |
| `puzzle_engine.h` / `puzzle_engine.cpp` | Puzzle generation, puzzle validation, timer logic |
| `database.h` / `database.cpp` | Word loading, leaderboard handling, save/load support |
| `words_easy.txt` | Easy word list |
| `words_medium.txt` | Medium word list |
| `words_hard.txt` | Hard word list |
| `leaderboard.txt` | Leaderboard data file |
| `Makefile` | Build instructions |
