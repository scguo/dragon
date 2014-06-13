/* Steve Guo - Finalized on January 12, 2011 -
 * This a game called "Dragon", written in the C language. It is a remake of/inspired
 * by "Snake", a video game first released during the mid 1970s in arcades.
 *
 * In Dragon you control a long, thin creature (resembling a snake), which roams around on a bordered plane,
 * picking up food, whilst trying to avoid touching its own tail/body or the walls that surround the playing field.
 *
 * Each time the Dragon eats a piece of food, its tail grows longer by one length, making the game increasingly difficult.
 * You can control the direction the Dragon heads (up, left, down, or right) while the body/tail follows suit, but you cannot
 * stop the Dragon from moving while the game is in progress, or go in reverse.
 *
 * This program/game is intended as freeware: if you've paid money for it you've been ripped off!
 */

/* Include standard libraries */
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

/* Defined Contants */
#define LIMIT_Y 32 /* Maximum number of rows for the playing area */
#define LIMIT_X 76 /* Maximum number of columns for the playing Area */
#define OFFSET_X 2 /* Column offset from origin of console window*/
#define OFFSET_Y 2 /* Row offset from origin of console window*/
#define SPACE_CHARACTER 32 /* Default: 32 [space] */
#define FOOD_CHARACTER 1 /* White on black smiling face */
#define OBSTACLE_CHARACTER 219 /* Default: 219 [white full block] */
#define DRAGON_KEY_LAYOUTS 3 /* Number of customizable Dragon key layouts */
#define DRAGON_LENGTH_CAP 512 /* Maximum size of Dragon */
#define MAX_GAME_RECORDS 20 /* Maximum number of entries displayed in records*/
#define MAX_NAME_LENGTH 9 /* 8 Character max [add one for '\0']*/

/* Gradient characters */
#define FULL_BLOCK_CHARACTER 219 /* Full gradient block */
#define DARK_SHADE_CHARACTER 178 /* Dark shade gradient block */
#define MEDIUM_SHADE_CHARACTER 177 /* Medium shade gradient block */
#define LIGHT_SHADE_CHARACTER 176 /* Light shade gradient block */

/* Directional characters */
#define LD_UR_CHARACTER 201 /* Dragon heading: left-down, or up-right */
#define LU_DR_CHARACTER 200 /* Dragon heading: left-up, or down-right */
#define LL_RR_CHARACTER 205 /* Dragon heading: left-left, or right-right */
#define RD_UL_CHARACTER 187 /* Dragon heading: right-down, or up-left */
#define RU_DL_CHARACTER 188 /* Dragon heading: right-up, or down-left */
#define UU_DD_CHARACTER 186 /* Dragon heading: up-up, or down-down */

/* Middle of playing field */
#define SPAWNDRAGON_X (LIMIT_X / 2) /* Middle of x-plane */
#define SPAWNDRAGON_Y (LIMIT_Y / 2) /* Middle of y-plane */

/* Constants */
const char * const DRAGON_GAME_VERSION = "Dragon 3.0"; /* Current development version */
const char * const DRAGON_RECORD_FILE = "Guo Steve - Dragon Records.bin"; /* Name of game record binary file */

/* Enumerated definitions */
enum GAME_MODE
{
    RANDOM_LEVEL, /* Random field generation upon reaching level target mode */
    SPAWN_LEVEL, /* Spawn tiles upon growth mode */
    USER_LEVEL /* User loaded field mode */
};

enum TILE_TYPE
{
    BLANK_TILE,	/* User field code for BLANK_CHARACTER */
    FOOD_TILE, /* User field code for FOOD_CHARACTER */
    OBSTACLE_TILE /* User field code for OBSTACLE_CHARACTER */
};

/* Function prototypes */

/* Visuals [aids] */
void moveCursorXY(int x, int y);
void loadScreen();
void splashScreen();

/* Game configuration */
void resetKeys();
void mapKeys();
void createUserField(bool *userFieldLoaded);
void readUserField(bool *userFieldLoaded);
void clearUserField(bool *userFieldLoaded);
void saveUserField();
int characterMapping(int tileType);

/* Game runtime */
void playDragon(enum GAME_MODE gameMode);
void createField(enum GAME_MODE gameMode, int currentLevel);
void drawLimit(int col, int row);
void displayScore(enum GAME_MODE gameModem, int currentScore, int currentHighscore, int currentLevel, int numberTiles);
void resetField(enum GAME_MODE gameMode, int numberTiles);
void spawnTile(int number, enum TILE_TYPE tile);
void loadPlayingField();
void blitField();
void addNode(char lastDirectionDragon, char directionDragon);

/* Utilities */
void drawLine(int character);
void saveGameRecord(char *playerName, char gameMode, int currentScore, int lengthDragon);
void readAllGameRecords();
int compareScores(const void *record1, const void *record2);
void deleteAllGameRecords();

/* Error handling */
void abnormalExit();

/* User interaction */
int mainMenu();

/* Global structure variables */
struct DirectionalControls
{
    int KEY_UP;
    int KEY_LEFT;
    int KEY_DOWN;
    int KEY_RIGHT;
} playerControls[DRAGON_KEY_LAYOUTS];

/* Dragon Length */
struct DragonNode
{
    int row;
    int col;
    int character;
} Dragon[DRAGON_LENGTH_CAP]; /* Dragon body data*/ 

/* Structure for reading/writing a record to a binary file*/
struct GameRecord
{
    char playerName[MAX_NAME_LENGTH];
    char gameMode;
    int currentScore;
    int lengthDragon;
};

/* Global gameplay variables */
int lengthDragon = 0; /* Dragon's length currently shown on screen */
int undisplayedLengthDragon = 0; /* Buffered Dragon length */
int KeyOptionUsed = 0; /* Key layouts for control */
int playingField[LIMIT_Y][LIMIT_X]; /* Hidden calculations field */
int userField[LIMIT_Y][LIMIT_X]; /* Separately stored user loaded field */

/* Main Program */
int main()
{
    SetConsoleTitle(DRAGON_GAME_VERSION); /* Change console window title to current version*/
    
    loadScreen(); /* Display "Dragon" game logo */

    moveCursorXY(0,0);
    drawLine(FULL_BLOCK_CHARACTER); /* prints a line of "FULL_BLOCK_CHARACTER" across the screen */
    drawLine(DARK_SHADE_CHARACTER); /* prints a line of "DARK_SHADE_CHARACTER" across the screen */
    drawLine(MEDIUM_SHADE_CHARACTER); /* prints a line of "MEDIUM_SHADE_CHARACTER" across the screen */
    drawLine(LIGHT_SHADE_CHARACTER); /* prints a line of "LIGHT_SHADE_CHARACTER" across the screen */

    moveCursorXY(31, 5);
    printf("WELCOME TO DRAGON");

    moveCursorXY(23, 8);
    printf("Programmed by Steve Guo for ICS3U");

    moveCursorXY(10, 10);
    printf("For optimal experience: use the largest resolution, maximize");

    moveCursorXY(17, 11);
    printf("the program, and remember to keep CAPS LOCK off");

    moveCursorXY(0,17);
    drawLine(LIGHT_SHADE_CHARACTER); /* prints a line of "LIGHT_SHADE_CHARACTER" across the screen */
    drawLine(MEDIUM_SHADE_CHARACTER); /* prints a line of "MEDIUM_SHADE_CHARACTER" across the screen */
    drawLine(DARK_SHADE_CHARACTER); /* prints a line of "DARK_SHADE_CHARACTER" across the screen */
    drawLine(FULL_BLOCK_CHARACTER); /* prints a line of "FULL_BLOCK_CHARACTER" across the screen */

    moveCursorXY(23, 15);
    system("pause");

    resetKeys(); /* Initialize default control settings */

    if(0 == mainMenu()) /* Start with no user field loaded*/
    {
        splashScreen(); /* Program has ended, show exit information */
        return 0;
    }
    else
    {
        abnormalExit(); /* Program has crashed */
        return 1;
    }
}

void loadScreen()
{
    printf("       ....\n");
    printf("   .xH888888Hx.\n");
    printf(" .H8888888888888:      .u    .                              u.     u.    u.\n");
    printf(" 888*\"\"\"?\"\"*88888X   .d88B :@8c       u         uL     ...ue888b  x@88k u@88c.\n");
    printf("'f     d8x.   ^%%88k =\"8888f8888r   us888u.  .ue888Nc.. 888R Y888r^\"8888\"\"8888\"\n");
    printf("'>    <88888X   '?8   4888>'88\" .@88 \"8888\"d88E`\"888E` 888R I888>  8888  888R\n");
    printf(" `:..:`888888>    8>  4888> '   9888  9888 888E  888E  888R I888>  8888  888R\n");
    printf("        `\"*88     X   4888>     9888  9888 888E  888E  888R I888>  8888  888R\n");
    printf("   .xHHhx..\"      !  .d888L .+  9888  9888 888E  888E u8888cJ888   8888  888R\n");
    printf("  X88888888hx. ..!   ^\"8888*\"   9888  9888 888& .888E  \"*888*P\"   \"*88*\" 8888\"\n");
    printf(" !   \"*888888888\"       \"Y\"     \"888*\"\"888\"*888\" 888&    'Y\"        \"\"   'Y\"\n");
    printf("        ^\"***\"`                  ^Y\"   ^Y'  `\"   \"888E\n");
    printf("                                           .dWi   `88E\n");
    printf("                                           4888~  J8%%\n");
    printf("                                            ^\"===*\"`\n");

    printf("%80s", DRAGON_GAME_VERSION);

    moveCursorXY(23, 22);
    system("pause");
    system("cls");
}

/* Position cursor */
void moveCursorXY(int x, int y)
{
    /* Variable declaration within scope of moveCursorXY() */
    COORD coordinate;
    coordinate.X = x;
    coordinate.Y = y;
    
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinate);
}

/* Prints a line of "character" across the screen */
void drawLine(int character)
{
    /* Variable declaration within scope of drawLine() */
    int i; /* Counter */

    for(i = 0; i < 80; i++)
    {
       printf ("%c", character);
    }
}

/* Resets all player controls to default */
void resetKeys()
{
    /* Player layout one */
    playerControls[0].KEY_UP = 'w';
    playerControls[0].KEY_LEFT = 'a';
    playerControls[0].KEY_DOWN = 's';
    playerControls[0].KEY_RIGHT = 'd';

    /* Player layout two */
    playerControls[1].KEY_UP = '8';
    playerControls[1].KEY_LEFT = '4';
    playerControls[1].KEY_DOWN = '5';
    playerControls[1].KEY_RIGHT = '6';

    /* Player layout three*/
    playerControls[2].KEY_UP = 'i';
    playerControls[2].KEY_LEFT = 'j';
    playerControls[2].KEY_DOWN = 'k';
    playerControls[2].KEY_RIGHT = 'l';
}

/* Main overview of program */
int mainMenu()
{
    /* Variable declarations within scope of mainMenu() */
    char gameMode;
    int returnCode = 0;
    bool userFieldLoaded = false; /* Indicates if a user loaded field is being stored */

    while(true)
    {
        system("cls");

        printf("%80s\n", "Main Menu: Dragon");

        printf(" \"1\" to play with random field generation upon reaching level target\n");
        printf(" \"2\" to play with spawn tiles upon growth\n");
        if(false == userFieldLoaded)
        {
            printf(" \"3\" to play with a user field (status: un-loaded) \n");
        }
        else
        {
            printf(" \"3\" to play with a user field (status: loaded) \n");
        }
        printf(" \"4\" to load a user field from a file\n");
        printf(" \"5\" to create a user field\n");
        printf(" \"6\" to unload the user field\n");
        printf(" \"7\" to re-map player keys\n");
        printf(" \"8\" to show game records\n");
        printf(" \"9\" to reset/clear game records\n");
        printf(" \"0\" to quit Dragon\n");

        do
        {
            fflush(stdin);
            printf("\n Option: ");
            returnCode = scanf("%c", &gameMode);
        }while(returnCode != 1 || gameMode < '0' || gameMode > '9');

        switch(gameMode)
        {
            case '1':
                playDragon(RANDOM_LEVEL);
                break;
            case '2':
                playDragon(SPAWN_LEVEL);
                break;
            case '3':
                playDragon(USER_LEVEL);
                break;
            case '4':  
                readUserField(&userFieldLoaded);
                break;
            case '5':
                createUserField(&userFieldLoaded);
                break;
            case '6':
                clearUserField(&userFieldLoaded);
                break;
            case '7':
                mapKeys();
                break;
            case '8':
                readAllGameRecords();
                break;
            case '9':
                deleteAllGameRecords();
                break;
            case '0':
                return 0; /* Exit Menu Loop successfully*/
                break;
            default:
                return 1; /* Exit Menu Loop unsuccessfully*/
                break;
        }
    }
}

/* Actual gameplay */
void playDragon(enum GAME_MODE gameMode)
{
    /* Variable declarations within scope of playDragon() */
    int inputKey;
    int i = 0, j = 0; /* Counters */
    
    bool pauseLevel = true; /* Indicates a pause upon start of each level */
    char directionDragon = playerControls[KeyOptionUsed].KEY_RIGHT; /* Where to place next node of Dragon */
    char lastDirectionDragon = playerControls[KeyOptionUsed].KEY_RIGHT; /* Where the last node of Dragon was placed */

    int currentScore = 0; /* Current game runthrough's score */
    int currentHighscore = 0; /* Current program record score */
    int numberTiles = 0; /* Number of objects to display, for both tile types */
    
    int currentLevel = 0; /* Initial starting level */
    int currentDelay = 0; /* Pause time for playability */
    int delayLength = 10; /* Changeable, but 10 is the scale for playability*/
    
    char playerName[MAX_NAME_LENGTH + 1] = " Dragon "; /* 8 Character max [add one for '\n', and one for '\0']*/

    do/* loop for restarting game */
    {
        currentLevel = 1;
        currentScore = 0;
        currentDelay = 14; /* Default: 14, minimum 5 for playability */
        
        if(RANDOM_LEVEL == gameMode)
        {
            numberTiles = 5; /* Default: 5, minimum 4 */
        }
        else
        {
            numberTiles = 1;
        }

        srand(time(0)); /* Ensure random seed initiated */

        /* Initial run once code */
        createField(gameMode, currentLevel);
        resetField(gameMode, numberTiles); /* Draw/create playing area */
        displayScore(gameMode, currentScore, currentHighscore, currentLevel, numberTiles);
        moveCursorXY(0, 0);

        /* Main game loop */
        do
        {
            /* Pause at start of every level (until any key is hit) */
            if(true == pauseLevel)
            {
                /* Print Dragon's spawn position in the playing field */
                moveCursorXY(Dragon[0].col, Dragon[0].row);
                printf("%c", Dragon[0].character);
                
                /* Arbitrarily heading right */
                directionDragon = playerControls[KeyOptionUsed].KEY_RIGHT;
                inputKey = playerControls[KeyOptionUsed].KEY_RIGHT;
                
                moveCursorXY(0, 0);
                system("pause");
                pauseLevel = false; /* Exit pause*/
                moveCursorXY(0, 0);
                printf("                               "); /* Cover up "Press any key to continue . . . ", or any other status message*/
            }

            /*Movement code*/
            addNode(lastDirectionDragon, directionDragon); /* Add a segment to the head of the Dragon */

            if(undisplayedLengthDragon > 0) /* If there is buffered content to add to Dragon, do it*/
            {
                lengthDragon++;
                undisplayedLengthDragon--;
            }
            else
            {
                /* Blank last segment of the Dragon */
                moveCursorXY(Dragon[0].col, Dragon[0].row);
                printf(" ");

                /* Compensate for the removal of the last segment, of the Dragon, from the array by shifting contents of existing*/
                for(i = 1; i <= lengthDragon; i++)
                {
                    Dragon[i - 1] = Dragon[i];
                }
            }

            /* Print whole Dragon's position on the screen */
            moveCursorXY(Dragon[lengthDragon - 1].col, Dragon[lengthDragon - 1].row); 
            printf("%c", Dragon[lengthDragon - 1].character);
            moveCursorXY(Dragon[lengthDragon - 2].col, Dragon[lengthDragon - 2].row); 
            printf("%c", Dragon[lengthDragon - 2].character);
            
            moveCursorXY(0, 0); /* Position flashing cursor in one place instead of following the Dragon */

            /* Game speed delay; for actual playability  */
            Sleep((currentDelay * delayLength) + delayLength);

            /* If key has been hit, then check if it is a direction key - if so, change direction */
            if(kbhit())
            {
                inputKey = getch();
                if(inputKey == playerControls[KeyOptionUsed].KEY_RIGHT || inputKey == playerControls[KeyOptionUsed].KEY_LEFT ||
                    inputKey == playerControls[KeyOptionUsed].KEY_UP || inputKey == playerControls[KeyOptionUsed].KEY_DOWN)
                {
                    lastDirectionDragon = directionDragon; /* Save last direction */

                    /* Prevent the Dragon from doubling back into itself and dying */
                    if(directionDragon != playerControls[KeyOptionUsed].KEY_UP && inputKey == playerControls[KeyOptionUsed].KEY_DOWN)
                    {
                        directionDragon = inputKey;
                    }
                    if(directionDragon != playerControls[KeyOptionUsed].KEY_LEFT && inputKey == playerControls[KeyOptionUsed].KEY_RIGHT)
                    {
                        directionDragon = inputKey;
                    }
                    if(directionDragon != playerControls[KeyOptionUsed].KEY_DOWN && inputKey == playerControls[KeyOptionUsed].KEY_UP)
                    {
                        directionDragon = inputKey;
                    }
                    if(directionDragon != playerControls[KeyOptionUsed].KEY_RIGHT && inputKey == playerControls[KeyOptionUsed].KEY_LEFT)
                    {
                        directionDragon = inputKey;
                    }
                }
            }
            else
            {
                lastDirectionDragon = directionDragon; /* Save last direction */
            }

            /* Collision with wall/s detection, the Dragon dies */
            if(Dragon[lengthDragon - 1].row <= OFFSET_Y - 1)
            {
                moveCursorXY(0, 0);
                printf("You collided with the top limit");
                inputKey = 'x'; /* exits game loop - Game Over */
            }
            else if(Dragon[lengthDragon - 1].col <= OFFSET_X - 1)
            {
                moveCursorXY(0, 0);
                printf("You collided with the left limit");
                inputKey = 'x'; /* exits game loop - Game Over */
            }
            else if(Dragon[lengthDragon - 1].row > LIMIT_Y + OFFSET_Y - 1)
            {
                moveCursorXY(0, 0);
                printf("You collided with the bottom limit");
                inputKey = 'x'; /* exits game loop - Game Over */
            }
            else if(Dragon[lengthDragon - 1].col > LIMIT_X + OFFSET_X - 1)
            {
                moveCursorXY(0, 0);
                printf("You collided with the right limit");
                inputKey = 'x'; /* exits game loop - Game Over */
            }
            
            /* Collision with obstacle/s detection, the Dragon dies */
            if(OBSTACLE_CHARACTER == playingField[Dragon[lengthDragon - 1].row - OFFSET_Y][Dragon[lengthDragon - 1].col - OFFSET_X])
            {
                moveCursorXY(0, 0);
                printf("You collided with an obstacle");
                inputKey = 'x'; /* exits game loop - Game Over */
            }

            /* Collision with body/tail detection => Dragon Dies */
            for(i = 0; i < lengthDragon - 1; i++)
            {
                if(Dragon[lengthDragon - 1].row == Dragon[i].row && Dragon[lengthDragon - 1].col == Dragon[i].col)
                {
                    moveCursorXY(0, 0);
                    printf("You collided with yourself");
                    inputKey = 'x'; /* exits game loop - Game Over */
                    break; /* Stop checking nodes for collision at first detection, the Dragon is already dead*/
                }
            }

            /* Collision with food detection, the Dragon grows */
            if(FOOD_CHARACTER == playingField[Dragon[lengthDragon - 1].row - OFFSET_Y][Dragon[lengthDragon - 1].col - OFFSET_X])
            {
                /* Increase currentScore and length of the Dragon */
                currentScore += (lengthDragon + undisplayedLengthDragon) * numberTiles;
                undisplayedLengthDragon++;
                addNode(lastDirectionDragon, directionDragon);
                playingField[Dragon[lengthDragon - 1].row - OFFSET_Y][Dragon[lengthDragon - 1].col - OFFSET_X] = SPACE_CHARACTER;

                /* When length of the Dragon reaches level target, continue to next level */
                if(RANDOM_LEVEL == gameMode && lengthDragon == (currentLevel + 3) * 2)
                {
                    currentScore += currentLevel * 1000;
                    numberTiles += 2; /* Add to number of tiles */
                    currentLevel++; /* Change current level */

                    /* Increase speed every even level */
                    if(0 == currentLevel % 2 && currentDelay > 5)
                    {
                        currentDelay--;
                    }

                    createField(gameMode, currentLevel);
                    pauseLevel = true;
                    resetField(gameMode, numberTiles); /* Draw/create updated playing area */
                }
                else if(SPAWN_LEVEL == gameMode) /* For growth spawn */
                {
                    resetField(gameMode, numberTiles); /* Draw/create updated playing area */
                    numberTiles++; /* Update number of obstacles */
                }
                displayScore(gameMode, currentScore, currentHighscore, currentLevel, numberTiles);
            }
            
            /* Check if Dragon array is full; game has reached it's limit */
            if(lengthDragon + undisplayedLengthDragon >= DRAGON_LENGTH_CAP)
            {
                moveCursorXY(19, 5);
                printf("Dragon's length is legendary...You've \"Won\"");
                inputKey = 'x'; /* exits game loop - Game Over */                                   
            }
            
        }while(inputKey != 'x');

        fflush(stdin);
        
        /* Game Over results/message */
        if(currentScore > currentHighscore)
        {
            currentHighscore = currentScore;
            displayScore(gameMode, currentScore, currentHighscore, currentLevel, numberTiles);
        }

        moveCursorXY(28,6);
        printf("  G A M E      O V E R  ");
        
        moveCursorXY(1, 7);
        printf("Enter your name (only 8 characters accepted): ");
        
        while(true)
        {
            moveCursorXY(strlen("Enter your name (only 8 characters accepted): ") + 1, 7);
            if(NULL != fgets(playerName, sizeof(playerName), stdin))
            {
                fflush(stdin);
                
                if(strlen(playerName) <= 1) /* Only get '\n', ignore it */
                {
                    continue;
                }
                else /* Accept the rest */
                {
                    playerName[strlen(playerName) - 1] = '\0'; /* Remove/change the '\n' character */
                    break;
                }
            }
        }
        
        saveGameRecord(playerName, gameMode, currentScore, lengthDragon + undisplayedLengthDragon); /* Auto-save */

        moveCursorXY(30,10);
        printf("Play Again? (y/n): ");

        do
        {
            inputKey = getch();
        }while(inputKey != 'y' && inputKey != 'n'); /* Filters input to 'y' or 'n' */

        pauseLevel = true; /* Reset run-once code */

    }while('y' == inputKey);
}

void readUserField(bool *userFieldLoaded)
{
    /* Variable declarations within scope of readUserField() */
    int i, j; /* Field matrix counter variables */
    
    /* Allow user to type in file they want to read from */
    char strFilename[64];
    FILE *userFile;

    /* Instructions on how the file being read should have been setup */
    printf("The file should have elements that are separated by spaces)\n");
    printf("Please use main menu option \"5\" to create a user field file\n");
    printf("type \"quit\" to return to main menu\n");
    
    /* Error checking in case file is unaccessable */
    do
    {
        printf("Enter a valid file[name]:");
        fflush(stdin);
        gets(strFilename);
        
        if(!strcmp(strFilename, "quit"))
        {
            printf("\nReturning to the main menu without applying anything\n");
            system("pause");
            return; /* Prematurely quit */
        }
        
        userFile = fopen(strFilename, "r");
    }while(NULL == userFile);

    /*Acquire elements of field*/
    for(i = 0; i < LIMIT_Y; i++)
    {
        for(j = 0; j < LIMIT_X; j++)
        {
            if(1 != fscanf(userFile, "%d", &userField[i][j]))
            {
                userField[i][j] = BLANK_TILE; /* Defaults errors to nothing */
            }
        }
    }
    
    fclose(userFile);
    printf("%s has been loaded\n", strFilename);
    *userFieldLoaded = true; /* Indicates that a user field was successfully loaded*/
    system("pause");
}

void createUserField(bool *userFieldLoaded)
{
    /* Variable declarations within scope of createUserField() */
    
    int i, j, row, col; /* Field matrix counter variables */
    int inputKey;
    
    *userFieldLoaded = true; /* Indicates that a user field was successfully loaded*/
    
    /* Draw current user field */
    drawLimit(OFFSET_X, OFFSET_Y - 1); /* Upper limit of playing field */

    /* Draw vertical limit */
    for(row = 0; row < LIMIT_Y; row++)
    {
        moveCursorXY(OFFSET_X - 1, OFFSET_Y + row);
        printf("%c", OBSTACLE_CHARACTER); /* left limit of field*/
        /* print contents, of line, inside boundaries */
        for(col = 0; col < LIMIT_X; col++)
        {
            printf("%c", characterMapping(userField[row][col]));
        }
        printf("%c", OBSTACLE_CHARACTER); /* right limit of field*/
    }

    drawLimit(OFFSET_X, LIMIT_Y + OFFSET_Y); /* Bottom limit of playing field */

    moveCursorXY(OFFSET_X, LIMIT_Y + 4);
    printf("\"0\" for a blank tile, \"1\" for a food tile, \"2\" for an obstacle tile");

    moveCursorXY(OFFSET_X, LIMIT_Y + 5);
    printf("\"9\" to return to main menu");
    
    moveCursorXY(0, 0);
    printf("Enter elements the elements of the %d by %d field\n", LIMIT_Y, LIMIT_X);

    fflush(stdin);

    /*Acquire elements of room*/
    for(i = 0; i < LIMIT_Y; i++)
    {
        for(j = 0; j < LIMIT_X; j++)
        {
            moveCursorXY(OFFSET_X + 50, LIMIT_Y + 5);
            printf("Cursor location: [%2d][%2d]", i, j);

            moveCursorXY(j + OFFSET_X, i + OFFSET_Y); /* Position cursor in relative place on playing field */
            while(true)
            {
                Sleep(100); /* Game speed delay; to prevent over processor usage  */
                /* If key has been hit */
                if(kbhit())
                {
                    inputKey = getch();
                   
                    if(inputKey == '9')
                    {
                        moveCursorXY(j + OFFSET_X, i + OFFSET_Y);
                        printf("%c", characterMapping(userField[i][j]));
                        saveUserField();
                        return; /* Prematurely quit */
                    }
                    else if(inputKey < '0' || inputKey > '2')
                    {
                        continue; /* Defaults errors to nothing */
                    }

                    userField[i][j] = inputKey - '0'; /* Convert character to integer */
                    break;
                }
            }
            printf("%c", characterMapping(userField[i][j])); /* Print over user entered tile type */
        }
    }
    
    moveCursorXY(OFFSET_X, LIMIT_Y + 7);
    printf("Your user field has been created");
    saveUserField();
}

int characterMapping(int tileType)
{
    /* Converts given numerical tile representation into the printable visual character */
    if(FOOD_TILE == tileType)
    {
        return FOOD_CHARACTER;
    }
    else if(OBSTACLE_TILE == tileType)
    {
        return OBSTACLE_CHARACTER;
    }
    else
    {
        return SPACE_CHARACTER; /* Assuming BLANK_TILE == tileType by default*/
    }
}

/* Allow user to select file they want to read from */
void saveUserField()
{
    /* Variable declarations within scope of saveUserField() */
    int i, j; /* Field matrix counter variables */
    char strFilename[64];
    FILE *userFile;

    /* Instructions on how the file being read should have been setup */
    moveCursorXY(OFFSET_X, LIMIT_Y + 8);
    printf("Saving user field to file (type \"quit\" to not save)\n");
    
    /* Error checking in case file is unaccessable */
    do
    {
        printf("Enter a valid file[name]:");
        fflush(stdin);
        gets(strFilename);

        if(!strcmp(strFilename, "quit"))
        {
            printf("\nReturning to the main menu without saving a file, but has been loaded\n");
            system("pause");
            return; /* Prematurely quit */
        }

        userFile = fopen(strFilename, "w");
    }while(NULL == userFile);

    /* Save elements of field */
    for(i = 0; i < LIMIT_Y; i++)
    {
        for(j = 0; j < LIMIT_X; j++)
        {
            fprintf(userFile, "%d ", userField[i][j]);
        }
    }
    fclose(userFile);

    printf("Your current user field has been saved to: %s\n", strFilename);
    system("pause");
}

void clearUserField(bool *userFieldLoaded)
{
    /* Variable declarations within scope of clearUserField() */
    int i, j; /* Field matrix counter variables */
    
    /* Delete elements of field */
    printf("Field of %d by %d has been cleared\n", LIMIT_Y, LIMIT_X);
    
    for(i = 0; i < LIMIT_Y; i++)
    {
        for(j = 0; j < LIMIT_X; j++)
        {
            userField[i][j] = BLANK_TILE;
        }
    }
    *userFieldLoaded = false;
    system("pause");
}

/* Edit key layouts for control */
void mapKeys()
{
    /* Variable declarations within scope of mapkeys() */
    int playerIndex; /* Key layout distinguisher */
    int returnCode = 0;
    int mapKey_input = 0;

    do
    {
        system("cls");

        for(playerIndex = 0; playerIndex < DRAGON_KEY_LAYOUTS; playerIndex++)
        {
            printf("Layout %d\n", playerIndex + 1);
            printf("%c :UP\n", playerControls[playerIndex].KEY_UP);
            printf("%c :LEFT\n", playerControls[playerIndex].KEY_LEFT);
            printf("%c :DOWN\n", playerControls[playerIndex].KEY_DOWN);
            printf("%c :RIGHT\n\n", playerControls[playerIndex].KEY_RIGHT);
        }

        printf(" \"1\" to re-map layout 1's keys\n");
        printf(" \"2\" to re-map layout 2's keys\n");
        printf(" \"3\" to re-map layout 3's keys\n");
        printf(" \"4\" to reset all layout keys to default settings\n");
        printf(" \"5\" to select key layout to use\n");
        printf(" \"0\" to return to main menu\n");

        do
        {
            fflush(stdin);
            printf("\nOption: ");
            returnCode = scanf("%d", &mapKey_input);
        }while(returnCode != 1 || mapKey_input < 0 || mapKey_input > 5);

        switch(mapKey_input)
        {
            case 0:
                break; /* Returning to main menu, do nothing */
            case 1:
            case 2:
            case 3:
                /* Making sure no key is selected for two or more directions */
                do
                {
                    printf("\nLayout %d\n", mapKey_input);

                    printf("UP: ");
                    fflush(stdin);
                    scanf("%c", &playerControls[mapKey_input - 1].KEY_UP);

                    printf("LEFT: ");
                    fflush(stdin);
                    scanf("%c", &playerControls[mapKey_input - 1].KEY_LEFT);

                    printf("DOWN: ");
                    fflush(stdin);
                    scanf("%c", &playerControls[mapKey_input - 1].KEY_DOWN);

                    printf("RIGHT: ");
                    fflush(stdin);
                    scanf("%c", &playerControls[mapKey_input - 1].KEY_RIGHT);
                }while(playerControls[mapKey_input - 1].KEY_UP == playerControls[mapKey_input - 1].KEY_LEFT ||
                         playerControls[mapKey_input - 1].KEY_UP == playerControls[mapKey_input - 1].KEY_DOWN ||
                         playerControls[mapKey_input - 1].KEY_UP == playerControls[mapKey_input - 1].KEY_RIGHT ||
                         playerControls[mapKey_input - 1].KEY_LEFT == playerControls[mapKey_input - 1].KEY_DOWN ||
                         playerControls[mapKey_input - 1].KEY_LEFT == playerControls[mapKey_input - 1].KEY_RIGHT ||
                         playerControls[mapKey_input - 1].KEY_DOWN == playerControls[mapKey_input - 1].KEY_RIGHT);
                printf("Key layout %d changed\n", mapKey_input);
                
                system("pause");
                break;
            case 4:
                resetKeys();
                printf("Key layouts reset to defaults\n");
                
                system("pause");
                break;
            case 5:
                do
                {
                    fflush(stdin);
                    printf("Select key layout 1, 2, or 3: ");
                    returnCode = scanf("%d", &mapKey_input);
                }while(returnCode != 1 || mapKey_input < 1 || mapKey_input > 3);

                KeyOptionUsed = mapKey_input;
                mapKey_input = 0; /* Exit loop */
                
                printf("Key layout %d selected, returning to main menu\n", KeyOptionUsed);
                --KeyOptionUsed; /* Map to index */
                
                system("pause");
                break;
            default:
                abnormalExit(); /* Error case */
                break;
        }
    }while(mapKey_input != 0);
}

/* End of program splash screen */
void splashScreen()
{
    system("cls");

    /* End of program splash screen */
    moveCursorXY(0,0);
    drawLine(254); /* prints a line of "254" across the screen */
    printf("\n");
    printf("       _/_/_/   _/                                      _/_/_/\n");
    printf("    _/       _/_/_/_/   _/_/   _/      _/   _/_/     _/       _/    _/   _/_/\n");
    printf("     _/_/     _/     _/_/_/_/ _/      _/ _/_/_/_/   _/  _/_/ _/    _/ _/    _/\n");
    printf("        _/   _/     _/         _/  _/   _/         _/    _/ _/    _/ _/    _/\n");
    printf(" _/_/_/       _/_/   _/_/_/     _/       _/_/_/     _/_/_/   _/_/_/   _/_/\n");
    drawLine(254); /* prints a line of "254" across the screen */

    moveCursorXY(0, 10);
    drawLine(223); /* prints a line of "223" across the screen */

    moveCursorXY(11, 11);
    printf("Dragon");

    moveCursorXY(13, 13);
    printf("This program/game is intended as freeware:");

    moveCursorXY(15, 15);
    printf("if you've paid money for it you've been ripped off!");

    moveCursorXY(0, 18);
    drawLine(220); /* prints a line of "220" across the screen */

    moveCursorXY(17, 17);
    system("pause"); 
}

/* Point of no return, user has somehow crashed the program */
void abnormalExit()
{
    system("cls");
    
    moveCursorXY(0,0);
    drawLine('!'); /* prints a line of "!" across the screen */

    moveCursorXY(13, 2);
    printf("Something abnormal has occured, you should not be here");

    moveCursorXY(29, 4);
    printf("\"Dragon\" will now exit");

    moveCursorXY(0, 10);
    printf("\n\n\n  88888888ba,\n");
    printf("  88      `\"8b\n");
    printf("  88        `8b\n");
    printf("  88         88  8b,dPPYba,  ,adPPYYba,   ,adPPYb,d8   ,adPPYba,   8b,dPPYba,\n");
    printf("  88         88  88P'   \"Y8  \"\"     `Y8  a8\"    `Y88  a8\"     \"8a  88P'   `\"8a\n");
    printf("  88         8P  88          ,adPPPPP88  8b       88  8b       d8  88       88\n");
    printf("  88      .a8P   88          88,    ,88  \"8a,   ,d88  \"8a,   ,a8\"  88       88\n");
    printf("  88888888Y\"'    88          `\"8bbdP\"Y8   `\"YbbdP\"Y8   `\"YbbdP\"'   88       88\n");
    printf("                                          aa,    ,88\n");
    printf("                                           \"Y8bbdP\"\n");

    moveCursorXY(23, 7);
    system("pause"); 
}

/* Setup user interface for gameplay */
void createField(enum GAME_MODE gameMode, int currentLevel)
{
    /* Variable declarations within scope of createField() */
    int i, row, col; /* Field matrix counter variables */
    
    /* Set up global variables for new level */
    /* Dragon has a starting total length of 5 */
    lengthDragon = 1;
    undisplayedLengthDragon = currentLevel + 3;

    /* Fill grid with blanks */
    for(row  = 0; row < LIMIT_Y; row++)
    {
        for(col = 0; col < LIMIT_X; col++)
        {
            playingField[row][col] = SPACE_CHARACTER; 
        }
    }
    
    /* Delete existing Dragon */
    for(i = 0; i < DRAGON_LENGTH_CAP; i ++)
    {
        Dragon[i].row = 0;
        Dragon[i].col = 0;
        Dragon[i].character = SPACE_CHARACTER;
    }
    
    /* Rereate the Dragon array */
    Dragon[0].row = SPAWNDRAGON_Y + OFFSET_Y;
    Dragon[0].col = SPAWNDRAGON_X + OFFSET_X;
    Dragon[0].character = LL_RR_CHARACTER; /* left, or right character */

    /* Print game name in top right corner */
    moveCursorXY(0, 0);
    printf("%80s", DRAGON_GAME_VERSION);

    /* Draw playing field */
    drawLimit(OFFSET_X, OFFSET_Y - 1); /* Upper limit of playing field */

    /* Draw vertical limit */
    for(row = 0; row < LIMIT_Y; row++)
    {
        moveCursorXY(OFFSET_X - 1, OFFSET_Y + row);
        printf("%c", OBSTACLE_CHARACTER); /* Left limit of field*/
        /* Print line of the contents of playing field, between boundaries */
        for(col = 0; col < LIMIT_X; col++)
        {
            printf("%c", playingField[row][col]);
        }
        printf("%c", OBSTACLE_CHARACTER); /* Right limit of field*/
    }

    drawLimit(OFFSET_X, LIMIT_Y + OFFSET_Y); /* Bottom limit of playing field */

    moveCursorXY(11, LIMIT_Y + OFFSET_Y + 5);
    printf("Key layout: %d {Up: %c, Left: %c, Down: %c, Right: %c} Exit: x.", (KeyOptionUsed + 1), playerControls[KeyOptionUsed].KEY_UP, playerControls[KeyOptionUsed].KEY_LEFT, playerControls[KeyOptionUsed].KEY_DOWN, playerControls[KeyOptionUsed].KEY_RIGHT);
}

/* Draw horizontal limit */
void drawLimit(int col, int row)
{
    /* Variable declaration within scope of drawLimit() */
    int i; /* Counter */
    
    moveCursorXY(col, row);
    for(i = 0; i < LIMIT_X; i++)
    {
        printf("%c", OBSTACLE_CHARACTER);
    }
}

/* User interface creation/update */
void displayScore(enum GAME_MODE gameMode, int currentScore, int currentHighscore, int currentLevel, int numberTiles)
{
    if(RANDOM_LEVEL == gameMode) /* Only applicable when playing in this mode */
    {
        moveCursorXY(OFFSET_X, LIMIT_Y + OFFSET_Y + 1);
        printf("Level: %8d", currentLevel);
    
        moveCursorXY(OFFSET_X + 28, LIMIT_Y + OFFSET_Y + 3);
        printf("Level Target: %5d", (currentLevel + 3) * 2 + 1);
    }
    
    if(USER_LEVEL != gameMode) /* Only applicable if not playing in this mode */
    {
        moveCursorXY(OFFSET_X + 53, LIMIT_Y + OFFSET_Y + 3);
        printf("Obstacles: %5d", numberTiles);
    }
    
    moveCursorXY(OFFSET_X + 28, LIMIT_Y + OFFSET_Y + 1);
    printf("Score: %8u", currentScore);

    moveCursorXY(OFFSET_X + 53, LIMIT_Y + OFFSET_Y + 1);
    printf("High Score: %8u", currentHighscore);

    moveCursorXY(OFFSET_X, LIMIT_Y + OFFSET_Y + 3);
    printf("Dragon Length: %5d", lengthDragon + undisplayedLengthDragon);
}

/* Playing field creation/update */
void resetField(enum GAME_MODE gameMode, int numberTiles)
{
    switch(gameMode)
    {
        case SPAWN_LEVEL:
            /* Randomly add a piece of food, and an obstacle */
            spawnTile(1, FOOD_TILE);
            spawnTile(1, OBSTACLE_TILE);
            break;
        case USER_LEVEL:
			/* Load user defined field to play*/
            loadPlayingField();
            break;
		default: 
			/* Defaults to random field generation upon reaching level target mode*/
            /* Randomly add food and obstacles */
            spawnTile(numberTiles, FOOD_TILE);
            spawnTile(numberTiles, OBSTACLE_TILE);
            break;
    }
	playingField[SPAWNDRAGON_Y][SPAWNDRAGON_X] = SPACE_CHARACTER; /* Compensate for overwrite, where Dragon spawns*/
    blitField(); /* Update playing field */
}

/* Spawn given number of given tiles */
void spawnTile(int number, enum TILE_TYPE tile)
{
    /* Variable declarations within scope of spawnTile() */
    int i; /* Counter */
    int row, col; 

    for(i = 0; i < number; i++)
    {
        /* Randomly add a "OBSTACLE_CHARACTER" (an obstacle), or a "FOOD_CHARACTER" (food) */
        row = rand() % LIMIT_Y;
        col = rand() % LIMIT_X;
        
        if(SPACE_CHARACTER != playingField[row][col])/* Checks if playingField[row][col] is already occupied */
        {
            i--; /* Otherwise no update, re-do */
        }
        else
        {
            playingField[row][col] = characterMapping(tile);
        }
    }
}

void loadPlayingField()
{
    /* Variable declarations within scope of loadPlayingField() */
    int row, col; /* Field matrix counter variables */

    /* Randomly add "OBSTACLE_CHARACTER"s, an obstacle, and "FOOD_CHARACTER"s, food */
    for(row = 0; row < LIMIT_Y; row++)
    {
        for(col = 0; col < LIMIT_X; col++)
        {       
            playingField[row][col] = characterMapping(userField[row][col]);
        }
    }
}

/* Blit current status of field */
void blitField()
{
    /* Variable declarations within scope of blitField() */
    int row, col; /* Field matrix counter variables */

    /* Blit */
    for(row = 0; row < LIMIT_Y; row++)
    {
        moveCursorXY(OFFSET_X, OFFSET_Y + row);
        for(col = 0; col < LIMIT_X; col++)
        {
            /* Filtered blitting for efficiency */
            if(FOOD_CHARACTER == playingField[row][col] || OBSTACLE_CHARACTER == playingField[row][col])
            {
                moveCursorXY(col + OFFSET_X, row + OFFSET_Y);
                printf("%c", playingField[row][col]);
            }
        }
    }
}

/* Movement Growth of the Dragon */
void addNode(char lastDirectionDragon, char directionDragon)
{   
    if(directionDragon == playerControls[KeyOptionUsed].KEY_RIGHT)
    {
        Dragon[lengthDragon].row = Dragon[lengthDragon - 1].row;
        Dragon[lengthDragon].col = Dragon[lengthDragon - 1].col + 1;

        if(lastDirectionDragon == playerControls[KeyOptionUsed].KEY_UP)
        {
            Dragon[lengthDragon - 1].character = LD_UR_CHARACTER; /* left-down, or up-right */
        }
        else if(lastDirectionDragon == playerControls[KeyOptionUsed].KEY_DOWN)
        {
            Dragon[lengthDragon - 1].character = LU_DR_CHARACTER; /* left-up, or down-right */
        }

        Dragon[lengthDragon].character = LL_RR_CHARACTER; /* left, or right character */
    }
    else if(directionDragon == playerControls[KeyOptionUsed].KEY_LEFT)
    {
        Dragon[lengthDragon].row = Dragon[lengthDragon - 1].row;
        Dragon[lengthDragon].col = Dragon[lengthDragon - 1].col - 1;
        
        if(lastDirectionDragon == playerControls[KeyOptionUsed].KEY_UP)
        {
            Dragon[lengthDragon - 1].character = RD_UL_CHARACTER; /* right-down, or up-left */
        }
        else if(lastDirectionDragon == playerControls[KeyOptionUsed].KEY_DOWN)
        {
            Dragon[lengthDragon - 1].character = RU_DL_CHARACTER; /* right-up, or down-left */
        }

        Dragon[lengthDragon].character = LL_RR_CHARACTER; /* left, or right */
    }
    else if(directionDragon == playerControls[KeyOptionUsed].KEY_UP)
    {
        Dragon[lengthDragon].row = Dragon[lengthDragon - 1].row - 1;
        Dragon[lengthDragon].col = Dragon[lengthDragon - 1].col;
        
        if(lastDirectionDragon == playerControls[KeyOptionUsed].KEY_LEFT)
        {
            Dragon[lengthDragon - 1].character = LU_DR_CHARACTER; /* left-up, or down-right */
        }
        else if(lastDirectionDragon == playerControls[KeyOptionUsed].KEY_RIGHT)
        {
            Dragon[lengthDragon - 1].character = RU_DL_CHARACTER; /* right-up, or down-left */
        }

        Dragon[lengthDragon].character = UU_DD_CHARACTER; /* up, or down */
    }
    else if(directionDragon == playerControls[KeyOptionUsed].KEY_DOWN)
    {
        Dragon[lengthDragon].row = Dragon[lengthDragon - 1].row + 1;
        Dragon[lengthDragon].col = Dragon[lengthDragon - 1].col;
        
        if(lastDirectionDragon == playerControls[KeyOptionUsed].KEY_LEFT)
        {
            Dragon[lengthDragon - 1].character = LD_UR_CHARACTER; /* left-down, or up-right */
        }
        else if(lastDirectionDragon == playerControls[KeyOptionUsed].KEY_RIGHT)
        {
            Dragon[lengthDragon - 1].character = RD_UL_CHARACTER; /* right-down, or up-left */
        }

        Dragon[lengthDragon].character = UU_DD_CHARACTER; /* up, or down */
    }
}

/* Save a game record to the binary file */
void saveGameRecord(char *playerName, char gameMode, int currentScore, int lengthDragon)
{
    /* Variable declarations within scope of saveGameRecord() */
    struct GameRecord gameRecord;
    FILE *stream = fopen(DRAGON_RECORD_FILE, "a+b ");

    if(NULL == stream) /* Open file for appending a game record in binary */
    {
        moveCursorXY(OFFSET_X, 8);
        printf("Error accessing to \"%s\"\n", DRAGON_RECORD_FILE);
        moveCursorXY(OFFSET_X, 9);
        system("pause");
        return;
    }

    /* Fill in the record of the playthrough */
    strcpy(gameRecord.playerName, playerName);
    gameRecord.gameMode = gameMode + 1; /* Game mode selected, accounting for index */
    gameRecord.currentScore = currentScore;
    gameRecord.lengthDragon = lengthDragon;

    /* Write game record to file */
    if(1 != fwrite(&gameRecord, sizeof(GameRecord), 1, stream))
    {
        /* Writing error has occured */
        moveCursorXY(OFFSET_X, 8);
        printf("Error writing to file \"%s\"\n", DRAGON_RECORD_FILE);
        moveCursorXY(OFFSET_X, 9);
        system("pause");
    }
    fclose(stream);
}

/* Clean all game records in the binary file */
void deleteAllGameRecords()
{
    /* Variable declaration within scope of deleteAllGameRecords() */
    FILE *stream = fopen(DRAGON_RECORD_FILE, "w+b");
    
    if(NULL == stream)
    {
        printf("Cannot reset/clear \"%s\"\n", DRAGON_RECORD_FILE);
    }
    else
    {
        printf("\"%s\" reset/cleared\n", DRAGON_RECORD_FILE);
        fclose(stream);
    }

    system("pause");
}

/* Function utilized by qsort to perform a Quick Sort of game record */
int compareScores(const void *record1, const void *record2)
{
   /* Compare scores in two records and return the difference */
   return ((GameRecord *) record2)->currentScore - ((GameRecord *) record1)->currentScore;
}

/* Read in all game records from the binary file, showing at most top 20 records */
void readAllGameRecords()
{
    /* Variable declaration within scope of readAllGameRecords() */
    struct GameRecord topGameRecords[MAX_GAME_RECORDS];
    struct GameRecord oneGameRecord;
    int i; /* Counter */
    int numberOfRecordsRead = 0;
    int numberOfRecordsIgnored = 0;
    int readingStatus; 
    FILE *stream = fopen(DRAGON_RECORD_FILE, "rb ");
    
    if(NULL == stream) /* Open file for appending, binary */
    {
        printf("Error reading \"%s\"\n", DRAGON_RECORD_FILE);
        system("pause");
        return;
    }

    rewind(stream);

    /* First read at most MAX_GAME_RECORDS records */
    for(i = 0; i < MAX_GAME_RECORDS; i++)
    {
        readingStatus = fread(&topGameRecords[i], sizeof(GameRecord), 1, stream);
        
        if(1 == readingStatus)
        {
            ++numberOfRecordsRead;
        }
        else
        {
            break; /* Exit for loop */
        }
    }
    
    if(0 == numberOfRecordsRead) /* Accounting for blank record file */
    {
        printf("No entries in game record\n");
        system("pause");
        fclose(stream);
        return;
    }
    
    /* Sort records according to score using Quicksort algorithm */
    qsort((void *)&topGameRecords[0], (size_t)numberOfRecordsRead, sizeof(GameRecord), compareScores);
    
    /* Checks if more than MAX_GAME_RECORDS, no EOF has been reached, and no errors in reading */
    if(MAX_GAME_RECORDS == numberOfRecordsRead && !feof(stream) && !ferror(stream))
    {
        /* More game records exist; continue to read them, but only printing the top MAX_GAME_RECORDS records */
         while(1 == fread(&oneGameRecord, sizeof(GameRecord), 1, stream))
         {   /* because topGameRecords[MAX_GAME_RECORDS] already sorted in decreasing score order */
            if(oneGameRecord.currentScore > topGameRecords[MAX_GAME_RECORDS - 1].currentScore)
            { /* Ignore the last record which has the lowest score */
                for(i = MAX_GAME_RECORDS - 2; i >= 0; i--) /* Backwards comparing */
                {
                    if(oneGameRecord.currentScore > topGameRecords[i].currentScore)
                    {
                        topGameRecords[i+1] = topGameRecords[i]; /* Shift position of previous entry */

                        if(0 == i) /* Reached the top slot */
                        {
                            /* Record just read has the highest score */
                            topGameRecords[0] = oneGameRecord;
                            break;
                        }
                    }
                    else
                    {
                        /* Store the record just read such that topGameRecords[] still sorted in decreasing order */
                        topGameRecords[i+1] = oneGameRecord; 
                        break; 
                    }
                }
            }
            ++numberOfRecordsIgnored;
        }
    }

    fclose(stream); 
    
    /* Alerts user that there are undisplayed entries */
    if(numberOfRecordsIgnored > 0)
    {
        printf("Top %d Game records are shown; (Undisplayed Entries: %d)\n",  MAX_GAME_RECORDS, numberOfRecordsIgnored);
    }
    
    printf("%-9s %9s %6s %8s\n", "Name:", "Score:", "Mode:", "Length:"); /* Display column titles for entries */

    /* Display entries */
    for(i = 0; i < numberOfRecordsRead; i++)
    {
        printf("%-9s %9d %6d %8d\n", topGameRecords[i].playerName, topGameRecords[i].currentScore, topGameRecords[i].gameMode, topGameRecords[i].lengthDragon);
    }

    printf("\n");
    system("pause");
}
