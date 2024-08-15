#define TYPES_H

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