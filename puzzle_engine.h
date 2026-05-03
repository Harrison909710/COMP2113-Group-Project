#ifndef PUZZLE_ENGINE_H
#define PUZZLE_ENGINE_H
#include <vector>
#include <string>
#include <random>
#include <unordered_map>
#include <chrono> 
class PuzzleEngine 
{
public:
    explicit PuzzleEngine();
    ~PuzzleEngine() = default;

    // What it does: Shuffles the letters of a word to produce a scrambled anagram.
    // Input: word - the original word to scramble.
    // Output: returns a new string with the same letters in a random order.

    std::string generateAnagram(const std::string& word);
  
    // What it does: Checks whether the attempt is a valid anagram of the original word.
    // Input: original - the correct word; attempt - the player's guess.
    // Output: returns true if both strings contain the same letters, false otherwise.

    bool validateAnagram(const std::string& original, const std::string& attempt);

    // What it does: Encrypts or decrypts a string using the Caesar cipher with the given shift.
    // Input: text - the string to process; shift - number of positions to shift;
    //        encrypt - true to encrypt, false to decrypt.
    // Output: returns the resulting string after applying the cipher.
    std::string caesarCipher(const std::string& text, int shift, bool encrypt);
    
    void generateSubstitutionMap();

    // What it does: Applies the substitution cipher to a string using substitution_map_.
    // Input: text - the original string to encode.
    // Output: returns the encoded string with substituted characters.

    std::string substitutionCipher(const std::string& text);
   
    // What it does: Validates that each consecutive pair of words in the chain differs by exactly one letter.
    // Input: chain - a vector of strings representing the word chain.
    // Output: returns true if every adjacent pair is a single-letter change, false otherwise.

    bool validateWordChain(const std::vector<std::string>& chain);

    struct Riddle {
        std::string question;
        std::string answer;
    };

    Riddle getRandomRiddle();

    void startTimer(int seconds);
    bool isTimeUp() const;

private:
    std::unordered_map<char, char> substitution_map_;
    std::mt19937 rng_;
    std::chrono::system_clock::time_point end_time_ = std::chrono::time_point<std::chrono::system_clock>::min();
    
   const std::vector<Riddle> riddles_ = {
    {"What has keys but no locks?", "piano"},
    {"What gets wetter as it dries?", "towel"},
    {"I speak without a mouth and hear without ears.", "echo"},
    {"What has to be broken before you can use it?", "egg"},
    {"What goes up but never comes down?", "age"},
    {"I fly without wings, I cry without eyes.", "cloud"},
    {"The more you take, the more you leave behind.", "footsteps"},
    {"What can you catch but not throw?", "cold"},
    {"What 5-letter word becomes shorter when you add 2 letters?", "short"},
    {"What begins with T, ends with T, and has T in it?", "teapot"},
    {"What belongs to you but others use it more?", "name"},
    {"What can fill a room but takes up no space?", "light"},
    {"What has a head, a tail, but no body?", "coin"},
    {"What is so fragile that saying its name breaks it?", "silence"},
    {"What can run but never walks?", "river"},
    {"What has hands but can't clap?", "clock"},
    {"What has a neck but no head?", "bottle"},
    {"What gets bigger when more is taken away?", "hole"},
    {"What is full of holes but still holds water?", "sponge"},
    {"What has cities but no houses, forests but no trees?", "map"},
    {"What has words but never speaks?", "book"},
    {"What can travel around the world while staying in a corner?", "stamp"},
    {"What has a thumb and four fingers but isn't alive?", "glove"},
    {"What has many rings but no fingers?", "telephone"},
    {"What has a face and two hands but no arms or legs?", "clock"},
    {"What is always in front of you but can't be seen?", "future"},
    {"The more there is, the less you see. What is it?", "darkness"},
    {"What goes through towns and over hills but never moves?", "road"},
};

    void initializeRNG();
    // What it does: Checks whether two words of equal length differ by exactly one character.
    // Input: word1 - the first word; word2 - the second word.
    // Output: returns true if the words differ by exactly one letter, false otherwise.
    bool isSingleLetterChange(const std::string& word1, const std::string& word2);
};

#endif
