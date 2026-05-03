#include "puzzle_engine.h"
#include <algorithm>
#include <chrono>
#include <cctype>

using namespace std;

PuzzleEngine::PuzzleEngine() {
    initializeRNG();
    generateSubstitutionMap();
}

// initialize rng
void PuzzleEngine::initializeRNG() {
    random_device rd;
    rng_.seed(rd());
}


//Random Number Generator
string PuzzleEngine::generateAnagram(const string& word) {
    string scrambled = word;
    shuffle(scrambled.begin(), scrambled.end(), rng_);
    return scrambled;
}


//validate anagram
bool PuzzleEngine::validateAnagram(const string& original, const string& attempt) {
    if (original.length() != attempt.length()) return false;
    
    string sorted_original = original;
    string sorted_attempt = attempt;
    sort(sorted_original.begin(), sorted_original.end());
    sort(sorted_attempt.begin(), sorted_attempt.end());
    
    return sorted_original == sorted_attempt;
}


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

//  alphabet Substitution
void PuzzleEngine::generateSubstitutionMap() {
    string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string shuffled = alphabet;
    shuffle(shuffled.begin(), shuffled.end(), rng_);
    
    for (size_t i = 0; i < alphabet.size(); ++i) {
        substitution_map_[alphabet[i]] = shuffled[i];
        substitution_map_[tolower(alphabet[i])] = tolower(shuffled[i]);
    }
}

// Substitution cipher generator
string PuzzleEngine::substitutionCipher(const string& text) {
    string result;
    for (char c : text) {
        auto it = substitution_map_.find(c);
        result += (it != substitution_map_.end()) ? it->second : c;
    }
    return result;
}

//validate word chain
bool PuzzleEngine::validateWordChain(const vector<string>& chain) {
    if (chain.empty()) return false;
    
    for (size_t i = 0; i < chain.size() - 1; ++i) {
        if (!isSingleLetterChange(chain[i], chain[i+1])) {
            return false;
        }
    }
    return true;
}

// only one word can be changed each turn
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

// get random riddle 
PuzzleEngine::Riddle PuzzleEngine::getRandomRiddle() {
    uniform_int_distribution<size_t> dist(0, riddles_.size() - 1);
    return riddles_[dist(rng_)];
}

void PuzzleEngine::startTimer(int seconds) {
    end_time_ = chrono::system_clock::now() + chrono::seconds(seconds);
}
// tracking the time for answering question. 

bool PuzzleEngine::isTimeUp() const {
    return chrono::system_clock::now() >= end_time_;







    
}