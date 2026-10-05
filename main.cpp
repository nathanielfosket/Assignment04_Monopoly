// JDoodle: Libraries = SDL2; enable GUI and Interactive.
#include <SDL2/SDL.h>
#undef main
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

#include "LinkedList.h"
#include "Renderer.h"

using namespace std;

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << SDL_GetError() << '\n';
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "My Monopoly Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 720, 720, 0);
    
    SDL_Renderer* r = window ? SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE) : nullptr;
    
    if (!r) {
        cerr << SDL_GetError() << '\n';
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    CircularLinkedList board;

    string propertyNames[30] = {
        "Euston rd", "OLD KENT RD", "boardwalk", "WHITECHAPEL RD", "Liverpool street", "pall mall", "whitehall", "bow street", "marvin gardens",
        "Pennsylvania Avenue", "park place", "Pacific Avenue", "Water works", "electric company", "Ventnor Avenue", "Atlantic Avenue",
        "North Carolina Ave", "Fleet street", "bond street", "Piccadilly", "reading railroad", "Trafalgar Square", "Illinois Avenue",
        "The Strand", "Vine Street", "Marlborough Street", "oxford street", "Connecticut Avenue", "Oriental Avenue", "Mayfair"
    };

    for (int i = 0; i < 30; i++) {
        int randomCost = rand() % 8 + 2;
        board.append(propertyNames[i], randomCost);
    }
    
    Node* first = board.getHead();
    Node* player[2] = {first, first};
    int turn = 0; // 0 = blue, 1 = red
    int dice = 1;
    SDL_Rect rollButton = {310, 355, 100, 100};
    string topText1 = "Click on the dice or press";
    string topText2 = "space to roll";
    int p1Bones = 30;
    int p2Bones = 30;

    bool waitingForPurchase = false; 
    SDL_Rect yesButton = {280, 104, 75, 35};
    SDL_Rect noButton = {375, 104, 60, 35};
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            
            bool roll = false;
            bool clickYes = false;
            bool clickNo = false;

            if (event.type == SDL_KEYDOWN && !event.key.repeat && event.key.keysym.sym == SDLK_SPACE) {
                if (!waitingForPurchase) roll = true; 
            }
            
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                SDL_Point mouse = {event.button.x, event.button.y};
                if (!waitingForPurchase) {
                    roll = SDL_PointInRect(&mouse, &rollButton);
                } else {
                    clickYes = SDL_PointInRect(&mouse, &yesButton);
                    clickNo = SDL_PointInRect(&mouse, &noButton);
                }
            }
            
            if (roll) {
                dice = rand() % 6 + 1;
                for (int step = 0; step < dice; step++) {
                    player[turn] = player[turn]->next;
                }
                
                topText1 = "P" + to_string(turn + 1) + " LANDED ON " + player[turn]->name + ".";
                
                if (player[turn]->owner == 0) {
                    // Property is unowned! Pause the game and ask to buy.
                    waitingForPurchase = true;
                    topText2 = "Purchase " + player[turn]->name + " for " + to_string(player[turn]->cost) + " bones?";
                } else {
                    // Property is already owned. Skip buying and change turns.
                    if (player[turn]->owner == turn + 1) {
                        topText2 = "You already own this";
                    } else {
                        int rent = player[turn]->cost;
                        topText2 = "Owned by P" + to_string(player[turn]->owner) + "! P" + to_string(turn + 1) + " owes P"+ 
                        to_string(player[turn]->owner) + " " + to_string(player[turn]->cost) + " bones!";
                        
                        if (turn == 0) {
                            p1Bones -= rent;
                            p2Bones += rent;
                        } else {
                            p2Bones -= rent;
                            p1Bones += rent;
                        }
                    }
                    turn = 1 - turn; 
                }
            }

            if (clickYes && waitingForPurchase) {
                player[turn]->owner = turn + 1; // Assign ownership (1 or 2)
                
                int cost = player[turn]->cost;
                if (turn == 0) {
                    p1Bones -= cost;
                } else {
                    p2Bones -= cost;
                }

                topText1 = "P" + to_string(turn + 1) + " BOUGHT " + player[turn]->name + "!";
                topText2 = "";
                waitingForPurchase = false;
                turn = 1 - turn; 
            }

            if (clickNo && waitingForPurchase) {
                topText1 = "P" + to_string(turn + 1) + " did not purchase " + player[turn]->name + ".";
                topText2 = "";
                waitingForPurchase = false;
                turn = 1 - turn; 
            }
        }

        SDL_SetRenderDrawColor(r, 22, 65, 56, 255);
        SDL_RenderClear(r);

        Node* current = first;
        SDL_SetRenderDrawColor(r, 110, 150, 130, 255);
        for (int i = 0; i < 30; i++) {
            SDL_Point a = position(i), b = position((i + 1) % 30);
            SDL_RenderDrawLine(r, a.x, a.y, b.x, b.y);
        }

        int index = 0;
        do {
            SDL_Point p = position(index++);
            SDL_Rect tile = {p.x - 20, p.y - 20, 40, 40};
            
            // Color the tile based on ownership
            if (current->owner == 1) {
                SDL_SetRenderDrawColor(r, 135, 185, 245, 255);
            } else if (current->owner == 2) {
                SDL_SetRenderDrawColor(r, 245, 135, 135, 255);
            } else {
                SDL_SetRenderDrawColor(r, 239, 237, 219, 255);
            }
            SDL_RenderFillRect(r, &tile);
            
            SDL_SetRenderDrawColor(r, 25, 45, 40, 255);
            text(r, to_string(current->cost), p.x - 6, p.y - 15, 4);
            
            for (int j = 0; j < 2; j++) {
                if (current != player[j]) continue;
                SDL_Rect token = {p.x - 14 + j * 18, p.y + 8, 10, 10};
                
                if (j == 0) {
                    SDL_SetRenderDrawColor(r, 35, 125, 245, 255);
                } else {
                    SDL_SetRenderDrawColor(r, 235, 65, 75, 255);
                }
                
                SDL_RenderFillRect(r, &token);
            }
            current = current->next;
        } while (current != first);

        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        SDL_RenderFillRect(r, &rollButton);
        
        text(r, "ROLL", 315, 477, 6);
        
        SDL_SetRenderDrawColor(r, 25, 45, 40, 255);
        text(r, to_string(dice), 342, 375, 12);
        
        SDL_SetRenderDrawColor(r, 35, 125, 245, 255);
        text(r, "P1", 42, 365, 6);
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        text(r, to_string(p1Bones), 44, 415, 5);
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        text(r, "bones", 44, 450, 2);
        
        SDL_SetRenderDrawColor(r, 235, 65, 75, 255);
        text(r, "P2", 640, 365, 6);
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        text(r, to_string(p2Bones), 642, 415, 5);
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        text(r, "bones", 642, 450, 2);
        
        if (turn == 0) {
            SDL_SetRenderDrawColor(r, 35, 125, 245, 255);
        } else {
            SDL_SetRenderDrawColor(r, 235, 65, 75, 255);
        }
        
        text(r, "P" + to_string(turn + 1), 332, 280, 8);

        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        text(r, topText1, 65, 30, 4);
        text(r, topText2, 65, 67, 4);
        
        if (waitingForPurchase) {
            SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
            SDL_RenderFillRect(r, &yesButton);
            SDL_RenderFillRect(r, &noButton);
            
            SDL_SetRenderDrawColor(r, 22, 65, 56, 255);
            text(r, "YES", yesButton.x + 12, yesButton.y + 8, 4);
            text(r, "NO", noButton.x + 18, noButton.y + 8, 4);
        }
        
        SDL_RenderPresent(r);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}