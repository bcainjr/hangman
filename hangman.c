/*
* TDQC5
* Bruce Cain
*
* Create the game hangman for project 1.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define FILEWORDS ".words"
#define FILEHANGMAN ".hangman"

/* Plays the game loop */
void hangmanGame(char *word);
/* Determine file to use and grab word to use in game */
char *readFile(int const argc, char *argv[]);
/* Count number of lines in a file */
char countLines(void);

int main(int argc, char *argv[])
{
    char *fileInput;


    fileInput = readFile(argc, argv);

    /* Validate word */

    printf("%s\n", fileInput);
    if (strlen(fileInput) <= 34)
    {
        hangmanGame(fileInput);
    }

    return 0;
}

void hangmanGame(char *word)
{
    int games = 0, wins = 0, loss = 0, playing = 1, misses = 0, hits = 0;
    unsigned long  wordLength = strlen(word);
    char *input = NULL, *mark, found;
    double average = 0.0;
    /* size_t is unsigned int */
    size_t size;

    mark = (char *) malloc(wordLength);

    printf("Game: %d Wins: %d Losses: %d Average score: %.1f\n", games, wins, loss, average);

    while (playing)
    {
        printf("%d ", misses);

        /* print out number of slots for hidden word */
        for (int i = 0; i < wordLength; i++)
        {
            if (mark[i] == 1)
            {
                printf("%c", word[i]);
            }
            else
            {
                printf("_");
            }
        }

        printf(": ");

        getline(&input, &size, stdin);
        
        /* Validate user input */

        /* Check if guess is contained in the word */
        found = 0;
        for (int i = 0; i < wordLength; i++)
        {
            if (*input == word[i])
            {
                mark[i] = 1;
                hits++;
                found = 1;
            }
        }

        if (!found)
        {
            misses++;
        }

        /* Check if player lost */
        if (misses == 5)
        {
            printf("%s\nYou Lost...\n", word);
            playing = 0;
        }

        /* Check if player won */
        if (hits == wordLength)
        {
            printf("%s\nYou... Won! You missed %d\n", word, misses);
            playing = 0;
        }

    }

    free(mark);
    free(input);
}

char countLines(void)
{
    FILE *pWords;
    char lines = 0, *line = NULL;
    size_t size;

    pWords = fopen(FILEWORDS, "r");

    while (getline(&line, &size, pWords) != -1)
    {
        lines++;
    }

    fclose(pWords);

    return lines;
}

char *readFile(int const argc, char *argv[])
{
    FILE *pFile;
    char *input = NULL, opened = 0;
    size_t size;
    int randLine, lines;

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
        printf("Enter first file open try\n");
        if (pFile)
        {
            opened = 1;
        }
    }
    
    if (!opened)
    {
        pFile = fopen(FILEWORDS, "r");

        if (pFile == NULL)
        {
            printf("No file could be opened.\n");
            /* Could not open any file to read words */
            exit(2);
        }
    }

    lines = countLines();
    printf("lines %d\n", lines);
    
    randLine = (rand() % lines) + 1;
    printf("%d\n", randLine);

    if (pFile != NULL)
    {
        for (int i = 0; i < randLine; i++)
        {
            getline(&input, &size, pFile);
            input[strlen(input) - 1] = '\0';
            printf("%s\n", input);

            /* Validate word if fail quit? */
        }
    }

    fclose(pFile);
    return input;
}
