# COMP2113-Group-Project
# Word Dungeon
A terminal-based dungeon crawler where your weapon is your vocabulary. Explore procedurally generated floors, solve word puzzles, collect items, manage your HP, and defeat the boss on Floor 10 to escape.

---

## Team Members

| Name | Student ID | Role |
|------|-----------|------|
| *(Member 1 — fill in)* | *(ID)* | Database / Save-Load / Word System |
| *(Member 2 — fill in)* | *(ID)* | Dungeon Generation / Movement Logic |
| *(Member 3 — fill in)* | *(ID)* | Player / Inventory / Scoring |
| *(Member 4 — fill in)* | *(ID)* | UI / Game Loop / Puzzle Routing |
| *(Member 5 — fill in)* | *(ID)* | Puzzle Engine (all puzzle types + timer) |

> **Note:** Replace the placeholder names and IDs above with your actual team information before submission.

---

## Game Description

**Word Dungeon** is a roguelike word-puzzle game played entirely in the terminal. The player navigates an 11×11 grid dungeon floor by floor, entering rooms that each present a different word challenge. Solving puzzles earns score and preserves HP; wrong guesses deal damage. Survive all 10 floors and defeat the final boss to win.

Core loop:
1. Move around the floor with WASD
2. Enter a room → solve its word puzzle
3. Clear all rooms → stairs appear → advance to the next floor
4. Reach Floor 10 → fight the Boss (3-phase puzzle gauntlet)
5. Escape → final score calculated and saved to the leaderboard

---

## Features and Coding Requirements

Below each feature, the relevant coding elements (1–5) from the course requirements are identified and explained.

---

### 1. Procedural Dungeon Generation
Rooms are randomly placed on the grid each run. Floor 10 always spawns a Boss room; earlier floors get 2–4 puzzle rooms plus a guaranteed Treasure room. Hard mode adds one extra room.

- **Coding Element 2 — Data structures (vectors, structs):** `Floor`, `Room`, and `Map` are custom structs. The grid is a `vector<vector<TileType>>` and rooms are stored in a `vector<Room>`. These allow dynamic, runtime layout construction.
- **Coding Element 4 — Dynamic memory / pointers:** `getRoomAt()` returns a `Room*` pointer into the rooms vector, allowing the game loop to modify room state (e.g., mark cleared) through that pointer without copying.
- **Coding Element 3 — File I/O:** Word lists are read from `words_easy.txt`, `words_medium.txt`, and `words_hard.txt` at startup to populate the room puzzle data.

---

### 2. WASD Movement and Collision
The player moves one tile at a time. Movement is blocked by walls and dungeon boundaries. Stepping onto a room tile triggers a puzzle; stepping onto stairs advances the floor.

- **Coding Element 1 — Functions:** `handleMove()`, `getRoomAt()`, and `updateRoomStatus()` are separate functions in `dungeon.cpp`, each with a single clear responsibility.
- **Coding Element 2 — Structs:** `Floor` holds the `Map` grid and the player's start position. `TileType` is an enum class used to distinguish `WALL`, `FLOOR`, `ROOM_TILE`, and `STAIRS`.

---

### 3. Seven Puzzle Types
Each room presents one of: Anagram, Caesar Cipher, Substitution Cipher, Word Chain, Hangman, Riddle, or Speed Round.

- **Coding Element 1 — Functions:** Every puzzle type is a standalone function (`runAnagramPuzzle`, `runCaesarPuzzle`, `runHangmanPuzzle`, etc.) in `ui.cpp`, routed through `runPuzzle()` using a `switch` on `RoomType`.
- **Coding Element 2 — Classes / structs:** `PuzzleEngine` (in `puzzle_engine.h/.cpp`) encapsulates all puzzle logic — anagram generation, Caesar cipher, substitution map, word-chain validation, riddle list, and timer — as member functions and private data.
- **Coding Element 5 — STL / algorithms:** `std::shuffle` (from `<algorithm>`) randomises the anagram and substitution map. `std::sort` is used for anagram validation. `std::find_if` searches the inventory. `std::unordered_map<char,char>` stores the substitution cipher mapping.

---

### 4. Player Stats, HP, and Inventory
The player has HP (varies by difficulty), a score, a floor counter, and an inventory of up to 5 item types. Wrong guesses deal 1 damage; correct answers award score based on remaining HP.

- **Coding Element 1 — Functions:** `takeDamage()`, `healPlayer()`, `useItem()`, `applyReward()`, `onPuzzleSolved()`, `onWrongGuess()`, `calculateScore()`, and `displayInventory()` are all separate functions in `player.cpp`.
- **Coding Element 2 — Structs / enums:** `Player` struct holds all stats and an `std::vector<Item>` inventory. `ItemType` and `HintTier` are `enum class` types for type-safe item and hint identification.
- **Coding Element 5 — STL:** `std::vector<Item>` stores inventory dynamically. `std::find_if` with a lambda locates items by type for use/removal.

---

### 5. Hint System
During a puzzle the player may spend 1 HP to receive one of three hint tiers (Vague → Medium → Close), each revealing progressively more about the answer.

- **Coding Element 1 — Functions:** `useHint(Player& p, Room& r, int tier)` in `player.cpp` handles HP deduction and hint printing. `makeHintRoom()` in `ui.cpp` pre-builds hint strings for any answer word.
- **Coding Element 2 — Structs:** The `Room` struct (in `player.h`) stores `hint1`, `hint2`, `hint3` as `std::string` fields, filled per-puzzle at room entry time.

---

### 6. Items and Treasure Rooms
Treasure rooms award a random item. Items include Bandage (heal), Freeze (pause timer), Reveal (show 2 letters), Skip Token (skip puzzle), and Dictionary Scroll (reveal word category).

- **Coding Element 1 — Functions:** `runTreasure()` and `useItem()` handle reward assignment and consumption respectively.
- **Coding Element 2 — Structs / enums:** `Item` struct holds `ItemType`, name, and description. `ItemType` enum class covers all five item variants.
- **Coding Element 5 — STL:** Inventory is a `std::vector<Item>`; items are removed with `vector::erase` after use.

---

### 7. Boss Fight (Floor 10)
Floor 10 replaces all rooms with a single Boss room that runs a three-phase puzzle gauntlet (Riddle → Caesar Cipher → Anagram). All three phases must be passed to win.

- **Coding Element 1 — Functions:** `runBossFight()` in `ui.cpp` chains three puzzle-function calls sequentially. Failure at any phase triggers `onWrongGuess()` and ends the fight.
- **Coding Element 2 — Structs:** `RoomType::BOSS` is detected in `generateFloor()` to override normal room generation for Floor 10.

---

### 8. Save and Load System
The game can be saved mid-run (press S). On next launch, selecting "Continue" restores player stats, floor, and inventory from disk.

- **Coding Element 3 — File I/O:** `saveGameState()` writes player fields and floor metadata to `save.txt` using `ofstream`. `loadGameState()` reads them back with `ifstream`. Inventory is separately persisted in `inventory.txt`.
- **Coding Element 1 — Functions:** `saveFullGame()` and `loadFullGame()` in `ui.cpp` coordinate the two files and call the database-layer functions.

---

### 9. Leaderboard
After each run the player's score is appended to `leaderboard.txt`, which is kept sorted and trimmed to the top 10 entries.

- **Coding Element 3 — File I/O:** `saveLeaderboard()` reads, modifies, and rewrites `leaderboard.txt` using `ifstream`/`ofstream`.
- **Coding Element 5 — STL / algorithms:** `std::sort` with a lambda sorts `vector<ScoreEntry>` by score descending. `vector::resize` trims to 10 entries.

---

### 10. Speed Round Timer
In Speed Round rooms the player must answer within a time limit. The timer is tracked using the C++ `<chrono>` library.

- **Coding Element 1 — Functions:** `startTimer(int seconds)` and `isTimeUp()` in `PuzzleEngine` encapsulate timer logic.
- **Coding Element 2 — Classes:** Timer state (`end_time_`) is a private member of `PuzzleEngine`, keeping time management self-contained.

---

## Non-Standard Libraries

| Library | Header | Usage in Game |
|---------|--------|---------------|
| `conio.h` | `<conio.h>` | `_getch()` — reads single keystrokes for WASD movement without requiring the player to press Enter. Used in `dungeon.cpp` and `ui.cpp`. |

> **All other libraries used are part of the C++17 standard:** `<iostream>`, `<fstream>`, `<vector>`, `<string>`, `<algorithm>`, `<random>`, `<unordered_map>`, `<chrono>`, `<thread>`, `<cctype>`.

> **Platform note:** `<conio.h>` is Windows-only. The project is designed to compile and run on Windows (MSVC / MinGW g++).

---

## Compilation and Execution (Quick Start)

### Prerequisites
- **OS:** Windows
- **Compiler:** g++ with C++17 support (MinGW recommended)
- **Make:** GNU Make (included with MinGW)

### Step 1 — Clone / download the repository
```
git clone <your-repo-url>
cd Word-Dungeon
```

### Step 2 — Ensure word list files are present
The following files must be in the same directory as the executable:
```
words_easy.txt
words_medium.txt
words_hard.txt
```
These are included in the repository.

### Step 3 — Compile

**Using Make (recommended):**
```bash
make
```

**Using g++ directly:**
```bash
g++ -std=c++17 -Wall -o word_dungeon.exe main.cpp ui.cpp dungeon.cpp player.cpp puzzle_engine.cpp database.cpp
```

### Step 4 — Run
```bash
word_dungeon.exe
```

### Step 5 — Play
```
===== WORD DUNGEON =====
1. New Game
2. Continue
3. Leaderboard
4. Quit
```
- Choose **1** for a new game, enter your name, pick a difficulty (1 Easy / 2 Medium / 3 Hard)
- Use **W A S D** to move around the dungeon
- Walk into a room tile to start a puzzle
- Press **I** to view your inventory
- Press **H** inside a puzzle room to use a hint (costs 1 HP)
- Press **S** to save your game
- Press **ESC** to quit

### Clean build artifacts
```bash
make clean
```

---

## File List

| File | Description |
|------|-------------|
| `main.cpp` | Entry point — main menu loop |
| `ui.h` / `ui.cpp` | Game loop, puzzle routing, all display functions |
| `dungeon.h` / `dungeon.cpp` | Floor/room generation, WASD movement, tile logic |
| `player.h` / `player.cpp` | Player stats, inventory, scoring, hint system |
| `puzzle_engine.h` / `puzzle_engine.cpp` | All puzzle algorithms and timer |
| `database.h` / `database.cpp` | Word loading, leaderboard, save/load |
| `words_easy.txt` | Easy word list |
| `words_medium.txt` | Medium word list |
| `words_hard.txt` | Hard word list |
| `Makefile` | Build instructions |