#ifndef PLAYER_H   
#define PLAYER_H

#include <string>
#include <vector>
 // Forward declaration of Room to avoid circular includes between player.h and dungeon.h

class Room;
// Enum representing the types of items a player can hold in their inventory

enum class ItemType {
    DICTIONARY_SCROLL,  
    SKIP_TOKEN,         
    REVEAL,             
    FREEZE,             
    BANDAGE             
};
 // Enum representing the three tiers of hints available during a puzzle

enum class HintTier {
    VAGUE  = 1,   
    MEDIUM = 2,   
    CLOSE  = 3    
};
 

 // Struct representing a single item in the player's inventory

struct Item {
    ItemType type;
    std::string name;
    std::string description;
};
 
// Struct representing the full state of the player during a run

struct Player {
    std::string name;
    int level;
    int hp;           
    int maxHp;        
    int score;        
    int currentFloor; 
    int wrongGuesses;
 
    bool freezeActive;       
    int  freezeSecondsLeft;  
 
    std::vector<Item> inventory;
};
 
// What it does: Creates and initializes a new Player with the given name and difficulty.
//               Sets maxHp based on difficulty: Easy=10, Normal=7, Hard=5.
// Input: name - the player's chosen name; difficulty - 1 (Easy), 2 (Normal), 3 (Hard)
// Output: returns a fully initialized Player struct
Player createPlayer(const std::string& name, int difficulty);
 
// What it does: Reduces the player's HP by the specified amount, clamping to a minimum of 0.
// Input: p - reference to the Player to damage; amount - HP points to subtract
// Output: none (modifies p.hp in place)

void takeDamage(Player& p, int amount);
 
// What it does: Restores the player's HP by the specified amount, clamping to p.maxHp.
// Input: p - reference to the Player to heal; amount - HP points to restore
// Output: none (modifies p.hp in place)
void healPlayer(Player& p, int amount);

// Reveals room hint at specified tier for 1 HP. Aborts if player HP <= 1.
// Input: p (Player), r (Room), tier (1=vague, 2=medium, 3=close).
// Output: Prints hint and reduces p.hp by 1.
void useHint(Player& p, Room& r, int tier);

// Uses and removes inventory item, applying its effect. Errors if item missing.
// Input: p (Player), item (ItemType), r (Room).
// Output: Modifies player/inventory; prints result.
void useItem(Player& p, ItemType item, Room& r);

// What it does: Checks whether the player's HP has reached 0 or below.
// Input: p - const reference to the Player to check
// Output: returns true if the player is dead (hp <= 0), false otherwise
bool isDead(const Player& p);

// What it does: Adds a reward item to the player's inventory.
// Input: p - reference to the Player receiving the reward; reward - the Item to add
// Output: none (appends the item to p.inventory)
void applyReward(Player& p, const Item& reward);

// What it does: Calculates the player's total final score including floor and HP bonuses.
//               Formula: score + (currentFloor * 50) + (hp * 20)
// Input: p - const reference to the Player
// Output: returns an int representing the total calculated score
int calculateScore(const Player& p);

// What it does: Awards points when a puzzle is solved correctly and resets the wrong guess counter.
//               Bonus formula: 100 + (hp * 10)
// Input: p - reference to the Player who solved the puzzle
// Output: none (modifies p.score and p.wrongGuesses, prints result to console)
void onPuzzleSolved(Player& p);

// What it does: Handles a wrong puzzle guess by incrementing the wrong guess counter
//               and applying 1 point of damage to the player.
// Input: p - reference to the Player who guessed incorrectly
// Output: none (modifies p.wrongGuesses and p.hp, prints result to console)
void onWrongGuess(Player& p);

// What it does: Checks whether the player currently holds at least one of the specified item type.
// Input: p - const reference to the Player; item - the ItemType to search for
// Output: returns true if the item exists in the inventory, false otherwise
bool hasItem(const Player& p, ItemType item);

// What it does: Displays the player's HP bar, current floor, and score to the console.
// Input: p - const reference to the Player
// Output: none (prints formatted status line to console)
void displayStatus(const Player& p);

// What it does: Displays all items in the player's inventory with their names and descriptions.
//               Prints "(empty)" if the inventory contains no items.
// Input: p - const reference to the Player
// Output: none (prints inventory list to console)
void displayInventory(const Player& p);

#endif
 

