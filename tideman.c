#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

/*
[   cada elemento "i" representa a cada candidato, y los elementos "j" representan cuántos votaron que alice es mejor que el otro, y van en orden de A B C igual
       A  B  C
    A [/, 0, 0],
    B [0, /, 0],
    C [0, 0, /],
    i
]



*/

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
    int dif;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }


    pair_count = 0;
    int voter_count = get_int("Number of voters: ");


    // Clear array of preferences
    for (int i=0; i<candidate_count; i++)
    {
        for (int j=0; j<candidate_count; j++)
        {
            preferences[i][j] = 0;
        }
    }


    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    /*
    for (int i=0; i<candidate_count; i++)
    {
        printf("[");
        for (int j=0; j<candidate_count; j++)
        {
            printf("%i ", preferences[i][j]);
        }
        printf("]\n");
    }
    */


    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i=0; i<candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // printf("Ranks: %i, %i, %i\n", ranks[0], ranks[1], ranks[2]);
    for (int i=0; i<candidate_count-1; i++)
    {
        for (int j=i+1; j<candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
            //printf("Preferences[%i][%i] = %i\n", ranks[i], ranks[j], preferences[ranks[i]][ranks[j]]);
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    pair pair;
    for (int i=0; i<candidate_count; i++)
    {
        for (int j=0; j<candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pair.winner = i;
                pair.loser = j;
                pair.dif = preferences[i][j] - preferences[j][i];
                pairs[pair_count] = pair;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
// Va a sortear las parejas en orden de qué tan fuerte estuvo la ganada
void sort_pairs(void)
{

    // Sorting
    pair change;
    int i = 0;
    int exit = 0;
    while (true)
    {
        if (pairs[i].dif < pairs[i+1].dif)
        {
            change = pairs[i];
            pairs[i] = pairs[i+1];
            pairs[i+1] = change;
            exit = 0;
        }
        else
        {
            exit ++;
        }

        i++;
        if (i>8) i = 0;

        if (exit > 9) break;
    }

    return;
}


// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i=0; i<pair_count; i++)
    {
        //printf("Winner #%i: %i\n", i+1, pairs[i].winner);
        //printf("Loser #%i: %i\n", i+1, pairs[i].loser);
        locked[pairs[i].winner][pairs[i].loser] = true;
    }

    return;
}

// Print the winner of the election
// Imprime a la source del graph, a el que nadie le está apuntando
void print_winner(void)
{
    for (int i=0; i<pair_count; i++)
    {
        //printf("[");
        int pointing = 0;
        for (int j=0; j<pair_count; j++)
        {
            //printf("%d ", locked[i][j]);
            if (i==j) continue;
            else if (locked[i][j] == false)
            {
                pointing++;
            }
        }
        if (pointing < 1)
        {
            printf("%s\n", candidates[i]);
        }
        //printf("]\n");
    }


    return;
}