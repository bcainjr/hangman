/*
 * TDQC5
 * Bruce Cain
 *
 * Create the game hangman for project 1.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#define FILEWORDS ".words"
#define FILEHANGMAN ".hangman"

/* Plays the game loop */
void            hangmanGame(
    char *word);

/* Determine file to use and grab word to use in game */
void            readFile(
    int const argc,
    char *argv[],
    char *fileInput);

/* Count number of lines in a file */
unsigned int    countLines(
    char const *fileName);

/* Save player stats to .hangman */
void            save(
    int *games,
    int *wins,
    int *loss,
    float *average,
    int *streak,
    int *lStreak,
    double *tTime,
    double *aTime);

/* Trys to load player stats from .hangman */
void            load(
    int *games,
    int *wins,
    int *loss,
    float *average,
    int *streak,
    int *lStreak,
    double *tTime,
    double *aTime);

/* Validate words in File */
char            wordValidation(
    char *word);

/* Validate user input */
char            inputValidation(
    char *character);

/* Print the hangman stick guy */
void            printStickGuy(
    int misses);

int
main(
    int argc,
    char *argv[])
{
    char fileInput[34] = { 0 };
    readFile(argc, argv, fileInput);
    hangmanGame(fileInput);

    return 0;
}

void
hangmanGame(
    char *word)
{
    /* 
     * hangmanGame function is used to play through the hangman game.
     *
     * char *word argument brings in the word the player is to guess.
     *
     */

    int             games = 0, wins = 0, loss = 0, playing = 1,
                    misses = 0, hits = 0, oddMarks = 0;
    int             streak = 0, longestStreak = 0;
    unsigned long   wordLength = strlen(word);
    char           *input = NULL, *mark, found, valid = 0;
    double          totalTime = 0.0, averageTime = 0.0;
    float           average = 0.0;

    /* size_t is unsigned int */
    size_t          size = 0;
    time_t          begin, end;

    load(&games, &wins, &loss, &average, &streak, &longestStreak, &totalTime,
         &averageTime);

    /* Increment games played */
    games++;

    /* Allocate enough space to mark guessed letter */
    mark = (char *) malloc(wordLength + 1);

    for (int i = 0; i < wordLength; i++)
    {
        if (ispunct(word[i]))
        {
            mark[i] = word[i];
            oddMarks++;
        }
        else
        {
            mark[i] = 0;
        }
    }

    printf("Game: %d Win(s): %d Loss(es): %d Average score: %.1f\n", games,
           wins, loss, average);
    printf
        ("Total Time Played: %.2f Average Time: %.2f Longest Win Streak: %d\n",
         totalTime, averageTime, longestStreak);

    while (playing)
    {
        begin = time(NULL);

        while (!valid)
        {
            printStickGuy(misses);

            printf("%d ", misses);

            /* print out number of slots for hidden word */
            for (int i = 0; i < wordLength; i++)
            {
                if (mark[i] == word[i])
                {
                    printf("%c", word[i]);
                }
                else
                {
                    printf("_");
                }
            }

            printf(": ");


            /* Validate user input */
            getline(&input, &size, stdin);
            input[strlen(input) - 1] = '\0';
            valid = inputValidation(input);
        }

        valid = 0;

        /* Check if guess is contained in the word */
        found = 0;
        for (int i = 0; i < wordLength; i++)
        {
            if (*input == word[i] && *input != mark[i])
            {
                mark[i] = word[i];
                hits++;
                found = 1;
            }
        }

        if (!found)
        {
            misses++;
        }

        /* Check if player lost */
        if (misses == 6)
        {
            printStickGuy(misses);
            printf("%s\nYou Lost...\n", word);
            loss++;
            playing = 0;
            streak = 0;
        }

        /* Check if player won */
        if (hits == (wordLength - oddMarks))
        {
            printf("%s\nYou... Won! You missed %d\n", word, misses);
            average += ((float) misses / (float) games);
            wins++;
            playing = 0;

            if (longestStreak < ++streak)
            {
                longestStreak = streak;
            }
        }

    }

    end = time(NULL);
    totalTime += difftime(end, begin);
    averageTime += (totalTime / (double) games);

    save(&games, &wins, &loss, &average, &streak, &longestStreak, &totalTime,
         &averageTime);

    free(mark);
    free(input);
}

unsigned int
countLines(
    char const *fileName)
{
    /*
     * countLines function will count the lines in the supplied file.
     *
     * char const *fileName is the provide file that contains the words
     * to be used in hangmanGame function.
     *
     */

    FILE           *pWords;
    char           *line = NULL;
    size_t          size = 0;
    unsigned int    lines = 0;

    pWords = fopen(fileName, "r");

    while (getline(&line, &size, pWords) != -1)
    {
        lines++;
    }

    fclose(pWords);
    free(line);

    return lines;
}

void
readFile(
    int const argc,
    char *argv[],
    char *fileInput)
{
    /*
     * readFile function will read either the default file for a word
     * or accept a filename on the command line as a agrument.
     *
     * int const argc is the number of command line arguments.
     *
     * char *argv[] is the character arrays of the command line arguments
     * delimited by a space.
     *
     * char *fileInput is a pointer that will hold the word that the
     * player guesses.
     *
     */

    FILE           *pFile;
    char           *input = NULL, opened = 0, *fileName, valid = 0, try = 0;
    size_t          size = 0;
    int             randLine = 0;
    unsigned int    lines;

    srand(time(NULL));

    if (argc > 2)
    {
        printf("Too many Command Line arguments.\n");
        printf("Enter file name to word list only.\n");
        /* Exit with 1 for too many CLI arguments */
        exit(1);
    }

    if (argc == 2)
    {
        pFile = fopen(argv[1], "r");
        if (pFile)
        {
            fileName = (char *) malloc(strlen(argv[1]));
            strcpy(fileName, argv[1]);
            opened = 1;
        }
    }

    if (!opened)
    {
        pFile = fopen(FILEWORDS, "r");

        if (!pFile)
        {
            printf("No file could be opened.\n");
            printf("Make sure .words is available or pass a file name.\n");
            /* Could not open any file to read words */
            exit(2);
        }

        fileName = (char *) malloc(strlen(FILEWORDS) + 1);
        strcpy(fileName, FILEWORDS);
    }

    /* Continue to check for valid word to use */
    while (!valid)
    {
        lines = countLines(fileName);
        randLine = (rand() % (lines - randLine)) + 1;

        /* track and test of total trys of invalid guesses */
        if (try == 2)
        {
            printf("Too many invalid words, correct word file.\n");
            printf
                ("Words must only contain letters and no longer than 34 characters\n");
            /* Exit with 4 for to many invalid words */
            exit(4);
        }
        try++;

        for (int i = 0; i < randLine; i++)
        {
            getline(&input, &size, pFile);
            input[strlen(input) - 1] = '\0';
        }

        /* Validate word in file */
        valid = wordValidation(input);
    }

    strcpy(fileInput, input);

    for (int i = 0; i < strlen(fileInput); i++)
    {
        if (isalpha(fileInput[i]))
        {
            fileInput[i] = tolower(fileInput[i]);
        }
    }

    free(fileName);
    free(input);
    fclose(pFile);
}

void
save(
    int *games,
    int *wins,
    int *loss,
    float *average,
    int *streak,
    int *lStreak,
    double *tTime,
    double *aTime)
{
    /*
     * save function will save the player states to the .hangman file.
     * 
     * int *games is the number of games played.
     *
     * int *wins is the number of games won.
     *
     * int *loss is the number of games lost.
     *
     * float *average is the average of misses / game played.
     *
     * int *streak is the current win streak.
     *
     * int *lStreak is the longest win streak.
     *
     * double *tTime is the total time played.
     *
     * double *aTime is the average time / game played.
     *
     */

    FILE           *pFile;

    pFile = fopen(FILEHANGMAN, "w");

    fprintf(pFile, "%d\n", *games);
    fprintf(pFile, "%d\n", *wins);
    fprintf(pFile, "%d\n", *loss);
    fprintf(pFile, "%f\n", *average);
    fprintf(pFile, "%d\n", *streak);
    fprintf(pFile, "%d\n", *lStreak);
    fprintf(pFile, "%lf\n", *tTime);
    fprintf(pFile, "%lf\n", *aTime);

    fclose(pFile);
}

void
load(
    int *games,
    int *wins,
    int *loss,
    float *average,
    int *streak,
    int *lStreak,
    double *tTime,
    double *aTime)
{
    /*
     * load function will load all the player stats from the .hangman file.
     * 
     * int *games is the number of games played.
     *
     * int *wins is the number of games won.
     *
     * int *loss is the number of games lost.
     *
     * float *average is the average of misses / game played.
     *
     * int *streak is the current win streak.
     *
     * int *lStreak is the longest win streak.
     *
     * double *tTime is the total time played.
     *
     * double *aTime is the average time / game played.
     *
     */


    FILE           *pFile;

    pFile = fopen(FILEHANGMAN, "r");

    if (pFile)
    {
        fscanf(pFile, "%d", games);
        fscanf(pFile, "%d", wins);
        fscanf(pFile, "%d", loss);
        fscanf(pFile, "%f", average);
        fscanf(pFile, "%d", streak);
        fscanf(pFile, "%d", lStreak);
        fscanf(pFile, "%lf", tTime);
        fscanf(pFile, "%lf", aTime);

        fclose(pFile);
    }

}

char
wordValidation(
    char *input)
{
    /*
     * wordValidation validates the word chosen from either a given file or
     * the default file.
     *
     * char *input is the word to be validated.
     *
     */

    char            valid = 0;

    if (strlen(input) > 34)
    {
        printf("No word may be longer then 34 characters.\n");
        /* Exit with 3 for word to long */
        exit(3);
    }

    for (int i = 0; i < strlen(input); i++)
    {
        if (isalpha(input[i]) || ispunct(input[i]))
        {
            valid = 1;
        }
        else
        {
            valid = 0;
            break;
        }
    }

    return valid;
}

char
inputValidation(
    char *character)
{
    /*
     * inputValidation validates the user's input.
     *
     * char *character is the input captured from the stdin that the
     * user themselve entered.
     *
     */

    char            valid;

    if (strlen(character) == 1 && isalpha(*character))
    {
        *character = tolower(*character);
        valid = 1;
    }
    else
    {
        printf("That is not valid input, enter 1 character.\n");
        valid = 0;
    }

    return valid;
}

void
printStickGuy(
    int misses)
{
    /*
     * printStickGuy will print a stick figure man depending on how many
     * misses are counted.
     *
     * int misses is how many times the user incorrectly guessed a 
     * character.
     *
     */

    switch (misses)
    {
    case (0):
    {
        printf("______\n|    |\n");
        printf("|\n");
        printf("|\n");
        printf("|\n");
        printf("|\n");
        printf("|\n");
        printf("|________\n");
        break;
    }
    case (1):
    {
        printf("______\n");
        printf("|    |\n");
        printf("|    O\n");
        printf("|\n");
        printf("|\n");
        printf("|________\n");

        break;
    }
    case (2):
    {
        printf("______\n");
        printf("|    |\n");
        printf("|    O\n");
        printf("|    |\n");
        printf("|    |\n");
        printf("|\n");
        printf("|________\n");

        break;
    }
    case (3):
    {
        printf("______\n");
        printf("|    |\n");
        printf("|    O\n");
        printf("|   /|\n");
        printf("|    |\n");
        printf("|\n");
        printf("|________\n");

        break;
    }
    case (4):
    {
        printf("______\n");
        printf("|    |\n");
        printf("|    O\n");
        printf("|   /|\\\n");
        printf("|    |\n");
        printf("|\n");
        printf("|________\n");

        break;
    }
    case (5):
    {
        printf("______\n");
        printf("|    |\n");
        printf("|    O\n");
        printf("|   /|\\\n");
        printf("|    |\n");
        printf("|   /\n");
        printf("|________\n");

        break;
    }
    case (6):
    {
        printf("______\n");
        printf("|    |\n");
        printf("|    O\n");
        printf("|   /|\\\n");
        printf("|    |\n");
        printf("|   / \\\n");
        printf("|________\n");

        break;
    }
    }
}
