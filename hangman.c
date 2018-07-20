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
void hangmanGame(char const *word);
/* Count number of lines in a file */
char countLines(FILE *pFile);

int main(void)
{
    FILE *pWords;
    char *input = NULL;
    int lines, randLine;
    size_t size;

    srand(time(NULL));

    pWords = fopen(FILEWORDS, "r");

    lines = countLines(pWords);
    printf("lines %d\n", lines);
    
    randLine = (rand() % lines);
    printf("%d\n", randLine);

    if (pWords != NULL)
    {
        for (int i = 0;  i < randLine; i++)
        {
            printf("%d\n", i);
            getline(&input, &size, pWords);
            input[strlen(input) - 1] = '\0';
            printf("%s\n", input);
        }
    }

    fclose(pWords);

    /* Validate word */

    printf("%lu\n", strlen(input));
    if (strlen(input) <= 34)
    {
        hangmanGame(input);
    }

    free(input);
    return 0;
}

void hangmanGame(char const *word)
{
    int games = 0, wins = 0, loss = 0, playing = 1, misses = 0, hits = 0;
    unsigned long  wordLength = strlen(word);
    char *input = NULL, *mark, found;
    double average = 0.0;
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
        if (misses > 6)
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

char countLines(FILE *pFile)
{
    char lines = 0;
    char *line = NULL;
    size_t size;

    while (getline(&line, &size, pFile) != -1)
    {
        lines++;
    }

    return lines;
}
