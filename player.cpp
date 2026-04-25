#include "player.h"
#include <iostream>
#include <algorithm>
#include <string>

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

void takeDamage(Player& p, int amount) {
    p.hp -= amount;
    if (p.hp < 0) p.hp = 0;
}

void healPlayer(Player& p, int amount) {
    p.hp += amount;
    if (p.hp > p.maxHp) p.hp = p.maxHp;
}
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

bool hasItem(const Player& p, ItemType item) {
    for (const auto& i : p.inventory) {
        if (i.type == item) return true;
    }
    return false;
}

void applyReward(Player& p, const Item& reward) {
    p.inventory.push_back(reward);
    std::cout << "  [REWARD] You found: " << reward.name
              << " — " << reward.description << "\n";
}

bool isDead(const Player& p) {
    return p.hp <= 0;
}

void onPuzzleSolved(Player& p) {
    int bonus = 100 + (p.hp * 10);
    p.score += bonus;
    p.wrongGuesses = 0;
    std::cout << "  [CORRECT!] +" << bonus << " points. "
              << "Score: " << p.score << "\n";
}


void onWrongGuess(Player& p) {
    p.wrongGuesses++;
    takeDamage(p, 1);
    std::cout << "  [WRONG] -1 HP. HP remaining: "
              << p.hp << "/" << p.maxHp << "\n";
}

int calculateScore(const Player& p) {
    return p.score
         + (p.currentFloor * 50)
         + (p.hp * 20);
}

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