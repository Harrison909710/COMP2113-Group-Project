#include "player.h"
#include "dungeon.h"
#include <iostream>
#include <algorithm>
#include <string>
// What it does: Returns the display name string for a given ItemType enum value.
// Input: t - an ItemType enum value
// Output: a std::string with the human-readable item name
static std::string itemName(ItemType t) {
    switch (t) {
        case ItemType::DICTIONARY_SCROLL: return "Dictionary Scroll";
        case ItemType::SKIP_TOKEN:        return "Skip Token";
        case ItemType::REVEAL:            return "Reveal";
        case ItemType::FREEZE:            return "Freeze";
        case ItemType::BANDAGE:           return "Bandage";
        default:                          return "Unknown";
    }
}
// What it does: Creates and initializes a new Player struct with the given name and difficulty.
//               Sets HP based on difficulty: Easy=10, Normal=7, Hard=5.
// Input: name - the player's chosen name; difficulty - 1 (Easy), 2 (Normal), 3 (Hard)
// Output: returns a fully initialized Player struct
Player createPlayer(const std::string& name, int difficulty) {
    Player p;
    p.name         = name;
    p.score        = 0;
    p.currentFloor = 1;
    p.wrongGuesses = 0;
    p.freezeActive      = false;
    p.freezeSecondsLeft = 0;

    switch (difficulty) {
        case 1:  p.maxHp = 10; break;  
        case 2:  p.maxHp = 7;  break;  
        case 3:  p.maxHp = 5;  break;  
        default: p.maxHp = 7;  break;
    }
    p.hp = p.maxHp;
    return p;
}
// What it does: Reduces the player's HP by the given amount, clamping to a minimum of 0.
// Input: p - reference to the Player to damage; amount - number of HP points to subtract
// Output: none (modifies p.hp in place)
void takeDamage(Player& p, int amount) {
    p.hp -= amount;
    if (p.hp < 0) p.hp = 0;
}
// What it does: Restores the player's HP by the given amount, clamping to the player's maxHp.
// Input: p - reference to the Player to heal; amount - number of HP points to restore
// Output: none (modifies p.hp in place)
void healPlayer(Player& p, int amount) {
    p.hp += amount;
    if (p.hp > p.maxHp) p.hp = p.maxHp;
}
// What it does: Reveals a hint for the current room at the given tier, costing 1 HP.
//               Does nothing if the player has 1 or fewer HP (to prevent death by hint).
// Input: p - reference to the Player using the hint; r - reference to the current Room;
//         tier - hint level to reveal (1=vague, 2=medium, 3=close)
// Output: none (prints hint to console and modifies p.hp)
void useHint(Player& p, Room& r, int tier) {
    if (p.hp <= 1) {
        std::cout << "  [!] Not enough HP to use a hint!\n";
        return;
    }

    takeDamage(p, 1);
    std::cout << "  [HINT -1 HP] ";

    switch (tier) {
        case 1: std::cout << r.hint1 << "\n"; break;
        case 2: std::cout << r.hint2 << "\n"; break;
        case 3: std::cout << r.hint3 << "\n"; break;
        default: std::cout << "No hint available for that tier.\n";
    }
}
// What it does: Uses an item from the player's inventory, applying its effect and removing it.
//               Prints an error if the player does not have the specified item.
// Input: p - reference to the Player using the item; item - the ItemType to use
// Output: none (modifies player state and inventory, prints result to console)
void useItem(Player& p, ItemType item) {

    auto it = std::find_if(
        p.inventory.begin(), p.inventory.end(),
        [&](const Item& i) { return i.type == item; }
    );

    if (it == p.inventory.end()) {
        std::cout << "  [!] You don't have a " << itemName(item) << ".\n";
        return;
    }

    
    switch (item) {
        case ItemType::BANDAGE:
            healPlayer(p, 2);
            std::cout << "  [BANDAGE] Restored 2 HP. HP: "
                      << p.hp << "/" << p.maxHp << "\n";
            break;

        case ItemType::FREEZE:
            p.freezeActive      = true;
            p.freezeSecondsLeft = 15;
            std::cout << "  [FREEZE] Timer frozen for 15 seconds!\n";
            break;

        case ItemType::REVEAL:
            std::cout << "  [REVEAL] 2 letters will be revealed!\n";
            break;

        case ItemType::SKIP_TOKEN:
            std::cout << "  [SKIP] Puzzle skipped! No score for this room.\n";
            break;

        case ItemType::DICTIONARY_SCROLL:
            std::cout << "  [SCROLL] The word is a: ";
            std::cout << "(category revealed by puzzle engine)\n";
            break;
    }

    p.inventory.erase(it);
}
// What it does: Checks whether the player currently holds at least one of the given item type.
// Input: p - const reference to the Player; item - the ItemType to search for
// Output: returns true if the item is found in inventory, false otherwise
bool hasItem(const Player& p, ItemType item) {
    for (const auto& i : p.inventory) {
        if (i.type == item) return true;
    }
    return false;
}
// What it does: Adds a reward item to the player's inventory.
// Input: p - reference to the Player receiving the reward; reward - the Item to add
// Output: none (appends item to p.inventory)
void applyReward(Player& p, const Item& reward) {
    p.inventory.push_back(reward);
}
// What it does: Checks whether the player is dead (HP has reached 0).
// Input: p - const reference to the Player
// Output: returns true if p.hp is 0 or below, false otherwise
bool isDead(const Player& p) {
    return p.hp <= 0;
}
// What it does: Handles post-puzzle scoring when the player solves a puzzle correctly.
//               Awards 100 base points plus a bonus based on remaining HP, then resets wrong guess counter.
// Input: p - reference to the Player who solved the puzzle
// Output: none (modifies p.score and p.wrongGuesses, prints result to console)
void onPuzzleSolved(Player& p) {
    int bonus = 100 + (p.hp * 10);
    p.score += bonus;
    p.wrongGuesses = 0;
    std::cout << "  [CORRECT!] +" << bonus << " points. "
              << "Score: " << p.score << "\n";
}

// What it does: Handles the result of a wrong puzzle guess — increments wrong guess counter
//               and applies 1 point of damage to the player.
// Input: p - reference to the Player who guessed wrong
// Output: none (modifies p.wrongGuesses and p.hp, prints result to console)
void onWrongGuess(Player& p) {
    p.wrongGuesses++;
    takeDamage(p, 1);
    std::cout << "  [WRONG] -1 HP. HP remaining: "
              << p.hp << "/" << p.maxHp << "\n";
}
// What it does: Calculates the player's total final score, including floor progress and HP bonuses.
// Input: p - const reference to the Player
// Output: returns an int representing the total calculated score
int calculateScore(const Player& p) {
    return p.score
         + (p.currentFloor * 50)
         + (p.hp * 20);
}
// What it does: Displays the player's current HP bar, floor number, and score to the console.
// Input: p - const reference to the Player
// Output: none (prints formatted status line to console)
void displayStatus(const Player& p) {
    std::cout << "\n  [HP: ";

    // filled blocks
    for (int i = 0; i < p.hp; i++)     std::cout << "\u2588";
    // empty blocks
    for (int i = p.hp; i < p.maxHp; i++) std::cout << "\u2591";

    std::cout << " " << p.hp << "/" << p.maxHp << "]"
              << "  Floor: " << p.currentFloor
              << "  Score: " << p.score
              << "\n";
}
// What it does: Displays all items currently in the player's inventory with their names and descriptions.
//               Prints a message if the inventory is empty.
// Input: p - const reference to the Player
// Output: none (prints inventory list to console)
void displayInventory(const Player& p) {
    std::cout << "\n  === Inventory ===\n";
    if (p.inventory.empty()) {
        std::cout << "  (empty)\n";
        return;
    }
    for (int i = 0; i < (int)p.inventory.size(); i++) {
        std::cout << "  " << (i + 1) << ". "
                  << p.inventory[i].name << " — "
                  << p.inventory[i].description << "\n";
    }
    std::cout << "\n";
}