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
#include <ctype.h>

#define FILEWORDS ".words"
#define FILEHANGMAN ".hangman"

/* Plays the game loop */
void hangmanGame(char *word);
/* Determine file to use and grab word to use in game */
void readFile(int const argc, char *argv[], char *fileInput);
/* Count number of lines in a file */
unsigned int countLines(char const *fileName);
/* Save player stats to .hangman */
void save(int *games, int *wins, int *loss, float *average);
/* Trys to load player stats from .hangman */
void load(int *games, int *wins, int *loss, float *average);
/* Validate words in File */
char wordValidation(char *word);
/* Validate user input */
char inputValidation(char *character);

int main(int argc, char *argv[])
{
    char fileInput[34] = { 0 };

    readFile(argc, argv, fileInput);

    printf("%s\n", fileInput);

    hangmanGame(fileInput);

    return 0;
}

void hangmanGame(char *word)
{
    int games = 0, wins = 0, loss = 0, playing = 1, misses = 0, hits = 0, oddMarks = 0;
    unsigned long  wordLength = strlen(word);
    char *input = NULL, *mark, found, valid = 0;
    float average = 0.0;
    /* size_t is unsigned int */
    size_t size = 0;

    load(&games, &wins, &loss, &average);

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

    printf("Game: %d Win(s): %d Loss(es): %d Average score: %.1f\n", games, wins, loss, average);

    while (playing)
    {
        while (!valid)
        {
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
            printf("%s\nYou Lost...\n", word);
            loss++;
            playing = 0;
        }

        /* Check if player won */
        if (hits == (wordLength - oddMarks))
        {
            printf("%s\nYou... Won! You missed %d\n", word, misses);
            average += ((float) misses / (float) games);
            wins++;
            playing = 0;
        }

    }

    save(&games, &wins, &loss, &average);

    free(mark);
    free(input);
}

unsigned int countLines(char const *fileName)
{
    FILE *pWords;
    char *line = NULL;
    size_t size = 0;
    unsigned int lines = 0;

    pWords = fopen(fileName, "r");

    while (getline(&line, &size, pWords) != -1)
    {
        lines++;
    }

    fclose(pWords);
    free(line);

    return lines;
}

void readFile(int const argc, char *argv[], char *fileInput)
{
    FILE *pFile;
    char *input = NULL, opened = 0, *fileName, valid = 0, try = 0;
    size_t size = 0;
    int randLine = 0;
    unsigned int lines;

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
        strcpy(fileName , FILEWORDS);
    }

    /* Continue to check for valid word to use */
    while (!valid)
    {
        lines = countLines(fileName);
        randLine = (rand() % (lines - randLine)) + 1;
        printf("rand num = %d lines = %d\n", randLine, lines);
        /* track and test of total trys of invalid guesses */
        if (try == 2)
        {
            printf("Too many invalid words, correct word file.\n");
            printf("Words must only contain letters and no longer than 34 characters\n");
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

void save(int *games, int *wins, int *loss, float *average)
{
    FILE *pFile;
    
    pFile = fopen(FILEHANGMAN, "w");

    fprintf(pFile, "%d\n", *games);
    fprintf(pFile, "%d\n", *wins);
    fprintf(pFile, "%d\n", *loss);
    fprintf(pFile, "%f\n", *average);

    fclose(pFile);
}

void load(int *games, int *wins, int *loss, float *average)
{
    FILE *pFile;

    pFile = fopen(FILEHANGMAN, "r");

    if(pFile)
    {
        fscanf(pFile, "%d", games);
        fscanf(pFile, "%d", wins);
        fscanf(pFile, "%d", loss);
        fscanf(pFile, "%f", average);

        fclose(pFile);
    }

}

char wordValidation(char *input)
{
    char valid = 0;

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

char inputValidation(char *character)
{
    char valid;

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
