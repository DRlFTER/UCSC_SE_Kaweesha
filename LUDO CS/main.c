#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NUM_PLAYERS 4
#define NAME_LENGTH 20
#define NUM_PIECES 4

struct pieces
{
    char color;
    int preLocation;
    int hslocation;
    int location;
    int captures;

    int direction;
    int dotPassed;
};

struct player
{
    struct pieces pieces[NUM_PIECES];
    char name[NAME_LENGTH];
    int piecesOnBoard;
};

struct gameStatus
{
    struct player players[NUM_PLAYERS];
    struct player playerOrder[NUM_PLAYERS];
    int currentPlayer;
    int diceValue;
    int tossValue; // 0 = clockwise, 1 = anti-clockwise
};

int dice()
{
    return rand() % 6 + 1;
}

int initialToss()
{
    return rand() % 2;
}

void initializePlayerNamesAndPieces(struct gameStatus *status)
{
    char *names[NUM_PLAYERS] = {"Blue", "Red", "Green", "Yellow"};
    char pieceColors[NUM_PLAYERS] = {'B', 'R', 'G', 'Y'};

    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        strncpy(status->players[i].name, names[i], NAME_LENGTH);
        status->players[i].name[NAME_LENGTH - 1] = '\0';
        status->players[i].piecesOnBoard = 0;
        for (int j = 0; j < NUM_PIECES; j++)
        {
            status->players[i].pieces[j].color = pieceColors[i];
            status->players[i].pieces[j].location = -1;
            status->players[i].pieces[j].preLocation = -1;
            status->players[i].pieces[j].hslocation = -1;
            status->players[i].pieces[j].captures = 0;
            status->players[i].pieces[j].direction = -1;
            status->players[i].pieces[j].dotPassed = 0;
        }
    }
}

void determineFirstPlayer(struct gameStatus *status)
{
    int rolls[NUM_PLAYERS];
    int maxRoll = 0;
    int firstPlayer = 0;
    int tieCount = 0;

    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        rolls[i] = dice();
        printf("%s rolls %d\n", status->players[i].name, rolls[i]);
        if (rolls[i] > maxRoll)
        {
            maxRoll = rolls[i];
            firstPlayer = i;
            tieCount = 1;
        }
        else if (rolls[i] == maxRoll)
        {
            tieCount++;
        }
    }

    if (tieCount > 1)
    {
        printf("Tie detected, roll again\n");
        determineFirstPlayer(status);
        return;
    }

    status->currentPlayer = 0;

    int index = 0;
    for (int i = firstPlayer; i < NUM_PLAYERS; i++, index++)
    {
        status->playerOrder[index] = status->players[i];
    }
    for (int i = 0; i < firstPlayer; i++, index++)
    {
        status->playerOrder[index] = status->players[i];
    }
}

// Starting points Yellow = 2, Blue = 15, Red = 28, Green = 41
void placePieceOnBoard(struct gameStatus *status, struct player *player)
{
    int startingPositions[NUM_PLAYERS] = {2, 15, 28, 41};
    char *playerNames[NUM_PLAYERS] = {"Yellow", "Blue", "Red", "Green"};

    int startPosition = -1;
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        if (strcmp(player->name, playerNames[i]) == 0)
        {
            startPosition = startingPositions[i];
            break;
        }
    }

    for (int i = 0; i < NUM_PIECES; i++)
    {
        if (player->pieces[i].location == -1)
        {
            status->tossValue = initialToss();
            player->pieces[i].location = startPosition;
            player->pieces[i].direction = status->tossValue;
            player->piecesOnBoard++;
            printf("%s player moves piece %c%d to the starting point\n", player->name, player->pieces[i].color, i + 1);
            printf("%s player tossed %s\n", player->name, status->tossValue ? "Tail" : "Head");
            return;
        }
    }
}

void updateDotPassed(struct pieces *piece, struct player *player)
{
    int dotLocations[NUM_PLAYERS] = {0, 13, 26, 39}; // Yellow, Blue, Red, Green
    int playerIndex = -1;

    if (strcmp(player->name, "Yellow") == 0)
    {
        playerIndex = 0;
    }
    else if (strcmp(player->name, "Blue") == 0)
    {
        playerIndex = 1;
    }
    else if (strcmp(player->name, "Red") == 0)
    {
        playerIndex = 2;
    }
    else if (strcmp(player->name, "Green") == 0)
    {
        playerIndex = 3;
    }

    if (playerIndex != -1)
    {
        int dotLocation = dotLocations[playerIndex];

        int prevLocation = piece->preLocation;
        int currentLocation = piece->location;

        if (piece->direction == 0)
        {
            if ((prevLocation < dotLocation && currentLocation >= dotLocation) ||
                (prevLocation > currentLocation && currentLocation >= dotLocation))
            {
                piece->dotPassed++;
            }
        }
        else if (piece->direction == 1)
        {
            if ((prevLocation > dotLocation && currentLocation <= dotLocation) ||
                (prevLocation < currentLocation && currentLocation <= dotLocation))
            {
                piece->dotPassed++;
            }
        }

        // Update the preLocation to the current location for the next move
        piece->preLocation = currentLocation;
    }
}

void enterHomeStraight(struct pieces *piece, struct player *player, struct gameStatus *status)
{
    int homeStraightThresholds[NUM_PLAYERS] = {0, 13, 26, 39}; // Yellow, Blue, Red, Green

    int playerIndex = -1;
    if (strcmp(player->name, "Yellow") == 0)
    {
        playerIndex = 0;
    }
    else if (strcmp(player->name, "Blue") == 0)
    {
        playerIndex = 1;
    }
    else if (strcmp(player->name, "Red") == 0)
    {
        playerIndex = 2;
    }
    else if (strcmp(player->name, "Green") == 0)
    {
        playerIndex = 3;
    }

    if (playerIndex != -1)
    {
        int dotLocation = homeStraightThresholds[playerIndex];

        if (piece->location == 100)
        {
            return;
        }

        if (piece->direction == 1)
        {
            if (piece->dotPassed >= 2 && piece->location < dotLocation)
            {
                piece->hslocation = dotLocation - piece->location;
                piece->location = 100;
                printf("%s player move the piece to the Home straight cell %d\n", player->name, piece->hslocation);
            }
        }
        else if (piece->direction == 0)
        {
            if (piece->dotPassed == 1 && piece->location >= dotLocation)
            {
                piece->hslocation = piece->location - dotLocation;
                piece->location = 100;
                printf("%s player move the piece to the Home straight cell %d\n", player->name, piece->hslocation);
            }
        }
    }
}

void winGame(struct gameStatus *status, struct player *player)
{
    int allPiecesHome = 1;

    for (int i = 0; i < NUM_PIECES; i++)
    {
        if (player->pieces[i].hslocation != 0)
        {
            allPiecesHome = 0;
            break;
        }
    }
    if (allPiecesHome)
    {
        printf("%s wins the game\n", player->name);
        exit(0);
    }
}

void movingInHomeStraight(struct gameStatus *status, struct player *player)
{
    for (int i = 0; i < NUM_PIECES; i++)
    {
        if (player->pieces[i].hslocation > 0)
        {
            if (status->diceValue == 6 - player->pieces[i].hslocation)
            {
                player->pieces[i].hslocation = 0;
                printf("%s player's %c%d has reached Home\n", player->name, player->pieces[i].color, i + 1);
                winGame(status, player);
                return;
            }
        }
    }
}

void capturingPieces(struct gameStatus *status, struct player *player)
{

    for (int i; i < NUM_PIECES; i++)
    {
        if (player->pieces[i].location == player->pieces[i].preLocation)
        {
            player->pieces[i].captures++;
        }
    }
}

void moveSinglePiece(struct gameStatus *status, struct player *player)
{
    static int consecutiveSixes = 0;

    if (status->diceValue == 6)
    {
        consecutiveSixes++;
        if (consecutiveSixes == 3)
        {
            printf("%s has rolled 3 sixes consecutively. Chance cancelled\n", player->name);
            status->currentPlayer = (status->currentPlayer + 1) % NUM_PLAYERS;
            consecutiveSixes = 0;
            return;
        }
    }
    else
    {
        consecutiveSixes = 0;
    }

    for (int i = 0; i < NUM_PIECES; i++)
    {
        if (player->pieces[i].location != -1 && player->pieces[i].location != 100)
        {
            int prevLocation = player->pieces[i].location;

            if (player->pieces[i].direction == 1)
            {
                player->pieces[i].location -= status->diceValue;
                if (player->pieces[i].location < 1)
                {
                    player->pieces[i].location += 52;
                }
                printf("%s moves piece %c%d from location L%d to L%d by %d units in Anti-Clockwise direction\n", player->name, player->pieces[i].color, i + 1, prevLocation, player->pieces[i].location, status->diceValue);
            }
            else
            {
                player->pieces[i].location += status->diceValue;
                if (player->pieces[i].location > 52)
                {
                    player->pieces[i].location -= 52;
                }
                printf("%s moves piece %c%d from location L%d to L%d by %d units in Clockwise direction\n", player->name, player->pieces[i].color, i + 1, prevLocation, player->pieces[i].location, status->diceValue);
            }

            updateDotPassed(&player->pieces[i], player);
            enterHomeStraight(&player->pieces[i], player, status);
            return;
        }
        else if (player->pieces[i].location == 100)
        {
            movingInHomeStraight(status, player);
        }
    }
}

void determinePieceStatus(struct gameStatus *status, struct player *player)
{
    status->diceValue = dice();
    printf("%s rolls %d\n", player->name, status->diceValue);

    if (status->diceValue == 6)
    {
        int piecePlaced = 0;
        for (int i = 0; i < NUM_PIECES; i++)
        {
            if (player->pieces[i].location == -1)
            {
                placePieceOnBoard(status, player);
                piecePlaced = 1;
                break; // Exit after placing one piece
            }
        }
        if (piecePlaced == 0)
        {
            moveSinglePiece(status, player);
        }
        if (status->diceValue == 6)
        {
            determinePieceStatus(status, player);
        }
    }
    else
    {
        moveSinglePiece(status, player);
        status->currentPlayer = (status->currentPlayer + 1) % NUM_PLAYERS;
    }
}

void initializeGameStatus(struct gameStatus *status)
{
    initializePlayerNamesAndPieces(status);
    status->currentPlayer = 0;
    status->diceValue = 0;
}

int main()
{
    struct gameStatus status;
    srand(time(NULL));

    initializeGameStatus(&status);
    determineFirstPlayer(&status);

    printf("%s player has the highest roll and will begin the game\n", status.playerOrder[0].name);
    printf("The order of a single round is %s, %s, %s, and %s\n", status.playerOrder[0].name, status.playerOrder[1].name, status.playerOrder[2].name, status.playerOrder[3].name);

    while (1)
    {
        determinePieceStatus(&status, &status.playerOrder[status.currentPlayer]);
    }
    return 0;
}
