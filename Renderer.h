#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <string>

using namespace std;

// Tiny font drawing function
inline void text(SDL_Renderer* r, string words, int x, int y, int size) {
    string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!?.-"; 
    string font[] = {
        "111101111101101", "110101110101110", "111100100100111", 
        "110101101101110", "111100111100111", "111100110100100",
        "111100101101111", "101101111101101", "111010010010111",
        "001001001101111", "101110100110101", "100100100100111", 
        "101111101101101", "111101101101101", "111101101101111", 
        "111101111100100", "111101101111001", "111101110101101", 
        "111100111001111", "111010010010010", "101101101101111", 
        "101101101101010", "101101101111101", "101101010101101", 
        "101101111010010", "111001010100111", // A-Z

        "111101101101111", "010110010010111", "111001111100111", 
        "111001111001111", "101101111001001", "111100111001111", 
        "111100111101111", "111001001001001", "111101111101111", 
        "111101111001111", // 1-9

        "010010010000010", // !
        "111001010000010", // ?
        "000000000000010", // .
        "000000111000000", // -
    };
    
    for (char c : words) {
        c = toupper(c);
        auto letter = letters.find(c);
        if (letter != string::npos) {
            for (int i = 0; i < 15; i++) {
                if (font[letter][i] == '1') {
                    SDL_Rect pixel = {x + i % 3 * size, y + i / 3 * size, size, size};
                    SDL_RenderFillRect(r, &pixel);
                }
            }
        }
        x += 4 * size;
    }
}

// Board square geometry mapping function
inline SDL_Point position(int index) {
    int cx = 360; // Center X 
    int cy = 405; // Center Y 
    int r = 210;  // Radius 
    
    if (index <= 8) {
        // Top
        return {cx - r + (index * 2 * r / 8), cy - r};
    } else if (index <= 15) {
        // Right side
        return {cx + r, cy - r + ((index - 8) * 2 * r / 7)};
    } else if (index <= 23) {
        // Bottom
        return {cx + r - ((index - 15) * 2 * r / 8), cy + r};
    } else {
        // Left side
        return {cx - r, cy + r - ((index - 23) * 2 * r / 7)};
    }
}

#endif