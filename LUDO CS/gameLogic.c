#include "types.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define NUM_PLAYERS 4

struct pieces {
    char color;
    int location;
    int captures;
};

struct player {
    struct pieces piece[4];
    char color;
};

struct gameStatus {
    struct player players[4];
    int currentPlayer;
    int diceValue;
};

int dice() {
    srand(time(NULL));
    int diceRoll;
    diceRoll = rand() % 6 + 1;
    return diceRoll;
}

// Function to determine who plays first
void determineFirstPlayer(struct gameStatus *status) {
    int rolls[NUM_PLAYERS];
    int maxRoll = 0;
    int firstPlayer = 0;

    // Roll the dice for each player
    for (int i = 0; i < NUM_PLAYERS; i++) {
        rolls[i] = dice();
        printf("Player %d rolled a %d\n", i, rolls[i]);
        
        if (rolls[i] > maxRoll) {
            maxRoll = rolls[i];
            firstPlayer = i;
        }
    }

    // Set the starting player
    status->currentPlayer = firstPlayer;
    printf("Player %d goes first\n", firstPlayer);
}