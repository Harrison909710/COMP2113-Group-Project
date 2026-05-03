#include "puzzle_engine.h"
#include <algorithm>
#include <chrono>
#include <cctype>

using namespace std;

// What it does: Constructs the PuzzleEngine by initializing the RNG and generating the substitution map.
// Input: none.
// Output: none (constructor).

PuzzleEngine::PuzzleEngine() {
    initializeRNG();
    generateSubstitutionMap();
}

// What it does: Seeds the Mersenne Twister RNG using a hardware random device.
// Input: none.
// Output: none (modifies rng_ in place).
void PuzzleEngine::initializeRNG() {
    random_device rd;
    rng_.seed(rd());
}


// What it does: Shuffles the letters of a word to produce a scrambled anagram.
// Input: word - the original word to scramble.
// Output: returns a new string with the same letters in a random order.
string PuzzleEngine::generateAnagram(const string& word) {
    string scrambled = word;
    shuffle(scrambled.begin(), scrambled.end(), rng_);
    return scrambled;
}


// What it does: Checks whether the attempt is a valid anagram of the original word.
// Input: original - the correct word; attempt - the player's guess.
// Output: returns true if both strings contain the same letters, false otherwise.
bool PuzzleEngine::validateAnagram(const string& original, const string& attempt) {
    if (original.length() != attempt.length()) return false;
    
    string sorted_original = original;
    string sorted_attempt = attempt;
    sort(sorted_original.begin(), sorted_original.end());
    sort(sorted_attempt.begin(), sorted_attempt.end());
    
    return sorted_original == sorted_attempt;
}

// What it does: Encrypts or decrypts a string using the Caesar cipher with the given shift.
// Input: text - the string to process; shift - number of positions to shift each letter;
//        encrypt - true to encrypt, false to decrypt.
// Output: returns the resulting string after applying the cipher.

string PuzzleEngine::caesarCipher(const string& text, int shift, bool encrypt) {
    if (!encrypt) shift = -shift;
    
    string result;
    for (char c : text) {
        if (isalpha(c)) {
            char base = isupper(c) ? 'A' : 'a';
            c = (c - base + shift + 26) % 26 + base;
        }
        result += c;
    }
    return result;
}

// What it does: Generates a random one-to-one substitution mapping for all 26 letters.
// Input: none.
// Output: none (populates substitution_map_ for both upper and lower case).
void PuzzleEngine::generateSubstitutionMap() {
    string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string shuffled = alphabet;
    shuffle(shuffled.begin(), shuffled.end(), rng_);
    
    for (size_t i = 0; i < alphabet.size(); ++i) {
        substitution_map_[alphabet[i]] = shuffled[i];
        substitution_map_[tolower(alphabet[i])] = tolower(shuffled[i]);
    }
}

// What it does: Applies the substitution cipher to a string, replacing each letter using substitution_map_.
// Input: text - the original string to encode.
// Output: returns the encoded string with substituted characters.
string PuzzleEngine::substitutionCipher(const string& text) {
    string result;
    for (char c : text) {
        auto it = substitution_map_.find(c);
        result += (it != substitution_map_.end()) ? it->second : c;
    }
    return result;
}

// What it does: Validates that each consecutive pair of words in the chain differs by exactly one letter.
// Input: chain - a vector of strings representing the word chain to validate.
// Output: returns true if every adjacent pair is a single-letter change, false otherwise.
bool PuzzleEngine::validateWordChain(const vector<string>& chain) {
    if (chain.empty()) return false;
    
    for (size_t i = 0; i < chain.size() - 1; ++i) {
        if (!isSingleLetterChange(chain[i], chain[i+1])) {
            return false;
        }
    }
    return true;
}

// What it does: Checks whether two words of equal length differ by exactly one character.
// Input: word1 - the first word; word2 - the second word.
// Output: returns true if the words differ by exactly one letter, false otherwise.
bool PuzzleEngine::isSingleLetterChange(const string& word1, const string& word2) {
    if (word1.length() != word2.length()) return false;
    
    int differences = 0;
    for (size_t i = 0; i < word1.length(); ++i) {
        if (word1[i] != word2[i]) {
            if (++differences > 1) return false;
        }
    }
    return differences == 1;
}

// What it does: Selects and returns a random riddle from the internal riddle bank.
// Input: none.
// Output: returns a Riddle struct containing a question and its answer.
PuzzleEngine::Riddle PuzzleEngine::getRandomRiddle() {
    uniform_int_distribution<size_t> dist(0, riddles_.size() - 1);
    return riddles_[dist(rng_)];
}
// What it does: Sets the timer end point to the current time plus the given number of seconds.
// Input: seconds - the duration of the timer in seconds.
// Output: none (sets end_time_ for use by isTimeUp).

void PuzzleEngine::startTimer(int seconds) {
    end_time_ = chrono::system_clock::now() + chrono::seconds(seconds);
}

// What it does: Checks whether the timer started by startTimer has expired.
// Input: none.
// Output: returns true if the current time has reached or passed end_time_, false otherwise.

bool PuzzleEngine::isTimeUp() const {
    return chrono::system_clock::now() >= end_time_;







    
}