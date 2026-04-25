#ifndef PLAYER_H   
#define PLAYER_H

#include <string>
#include <vector>
 
enum class ItemType {
    DICTIONARY_SCROLL,  
    SKIP_TOKEN,         
    REVEAL,             
    FREEZE,             
    BANDAGE             
};
 
enum class HintTier {
    VAGUE  = 1,   
    MEDIUM = 2,   
    CLOSE  = 3    
};
 

 
struct Item {
    ItemType type;
    std::string name;
    std::string description;
};
 

struct Player {
    std::string name;
 
    int hp;           
    int maxHp;        
    int score;        
    int currentFloor; 
    int wrongGuesses;
 
    bool freezeActive;       
    int  freezeSecondsLeft;  
 
    std::vector<Item> inventory;
};
 

Player createPlayer(const std::string& name, int difficulty);
 

void takeDamage(Player& p, int amount);
 

void healPlayer(Player& p, int amount);
 

void useHint(Player& p, Room& r, int tier);
 

void useItem(Player& p, ItemType item);
 

bool isDead(const Player& p);
 

void applyReward(Player& p, const Item& reward);
 

int calculateScore(const Player& p);
 
void onPuzzleSolved(Player& p);
 

void onWrongGuess(Player& p);
 

bool hasItem(const Player& p, ItemType item);
 
void displayStatus(const Player& p);
 
void displayInventory(const Player& p);

void useHint(Player& p, Room& r, int tier);

#endif
 

