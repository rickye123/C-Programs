/* Wireworld - Week 4 Assessment */
/* This program will simulate wireworld - a cellular automation 
formed from a 2D grid of cells. Each cell has one of four states. 
Empty cells are denoted by a ' ' character, electron heads by 'H', 
electron tails by 't' and conductors by 'c'. 

In Linux terminals, this program will simulate wireworld 
in an animation, using colours to represent the letters 'H', 
't', 'c' and ' '. */ 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "neillncurses.h"

#define MAX_COLS 40
#define MAX_ROWS 40
#define EXIT_SUCCESSFUL 0
#define EXIT_FAILURE 1
#define CORRECT_NUM 1
#define INVALID 0
#define VALID 1
#define ELECTRON_HEAD 'H'
#define CONDUCTOR 'c'
#define ELECTRON_TAIL 't'
#define EMPTY ' '
#define INCREMENT_NUM_HEADS (*pNum)++
#define PRINT_NEWLINE puts("")
#define VALID_CHARACTERS (c == 'H' || c == 't' || c == 'c' || c == ' ' || c == '\n' || c == EOF)
#define END_ITERATION 500
#define CELL_ROW_LIMIT 2
#define CELL_COL_LIMIT 2
#define OUT_OF_BOUNDS 0
#define IN_RANGE 1

typedef char state; 

void invalid_argument(int argc, char argv[]);
void read_file(FILE *fp, state arr[][MAX_COLS]);
int check_characters(char c);
void set_colors(NCURS_Simplewin *sw);
void thousand_generations(state arr[][MAX_COLS], state new_arr[][MAX_COLS]);
void printArray(state arr[][MAX_COLS]);
void add_rules(state arr[][MAX_COLS], state new_arr[][MAX_COLS]);
void copy_element(state arr[][MAX_COLS], state new_arr[][MAX_COLS], int row, int col);
void apply_rules(state arr[][MAX_COLS], state new_arr[][MAX_COLS], int row, int col);
int access_array_oob(state a[][MAX_COLS], int row, int col);
int num_electron_heads(state a[][MAX_COLS], int row, int col);
void check_neighbouring_cells(state a[][MAX_COLS], int row, int col, int* pNum);
int check_cell_oob(int row, int col);
void copy_array(state new_arr[][MAX_COLS], state arr[][MAX_COLS]); 

int main(int argc, char **argv)
{

    FILE *fp; /* pointer to filename */
    /* 2D arrays of size 40 by 40 */
    state wire_world[MAX_ROWS][MAX_COLS]; 
    state new_wire_world[MAX_ROWS][MAX_COLS];
    NCURS_Simplewin sw; /* initialise mouse / keyboard events */

    /* if number of arguments passed to terminal is not 2 */
    invalid_argument(argc, argv[0]); 

    /* open file whose title is the second argument entered by the user */
    if ((fp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Error: Cannot open file\n");
        exit (EXIT_FAILURE); 
    }

    /* read file and exit if any invalid characters present */    
    read_file(fp, wire_world);

    fclose(fp);

    Neill_NCURS_Init(&sw); 

    /* set colors for states in wire_world */
    set_colors(&sw); 

    /*thousand_generations(wire_world, new_wire_world); - part 1 only*/

    /* continues looping until mouse click or ESC key pressed */
    do {
        /* print array, apply rules to new_wire_world, and copy 
        back into original array */
        Neill_NCURS_PrintArray(&wire_world[0][0], MAX_COLS, MAX_ROWS, &sw);
        add_rules(wire_world, new_wire_world); 
        copy_array(new_wire_world, wire_world); 
        Neill_NCURS_Delay(1000); /* 1 second delay */
        Neill_NCURS_Events(&sw); /* Wait for mouse click, or ESC key event */
    } while (!sw.finished); 

    /* Call this function if we exit() anywhere in the code */
    atexit(Neill_NCURS_Done);

    exit(EXIT_SUCCESSFUL); 

} /* end main */

void invalid_argument(int argc, char argv[])
{

    if (argc != 2) {
        fprintf(stderr, "Error: Incorrect usage, try e.g. %s wirefile.txt\n", argv);
        exit(EXIT_FAILURE);
    }

} /* end invalid_argument */

void set_colors(NCURS_Simplewin *sw)
{

    /* Function  sets the background color based on specific characters */
    Neill_NCURS_CharStyle(sw, "t", COLOR_RED, COLOR_RED, A_NORMAL);
    Neill_NCURS_CharStyle(sw, "c", COLOR_YELLOW, COLOR_YELLOW, A_NORMAL);
    Neill_NCURS_CharStyle(sw, "H", COLOR_BLUE, COLOR_BLUE, A_NORMAL);
    Neill_NCURS_CharStyle(sw, " ", COLOR_BLACK, COLOR_BLACK, A_NORMAL);
    Neill_NCURS_CharStyle(sw, "\n", COLOR_BLACK, COLOR_BLACK, A_NORMAL); 

} /* end set_colors */

void read_file(FILE *fp, state arr[][MAX_COLS])
{

    int i, j; 
    char c; /* characters read in by file */
    i = 0;

    while (i < MAX_ROWS) {
        j = 0;
        /* add characters to arr while not EOF.
        Go to second column of array when j is 40 */
        while (((c = getc(fp)) != EOF) && (j < MAX_COLS)) {
            arr[i][j] = c; 
            /* exit program if invalid characters found in file */
            if (check_characters(c) == INVALID) {
                fprintf(stderr, "Error: Invalid character in file\n");
                exit(EXIT_FAILURE); 
            }
            j++; 
        }
        i++;
    }

} /* end read_file */

/* checks to see if any characters bar ' ', 't', 'H'
'c', '\n' or EOF are present in the file */
int check_characters(char c)
{

    if (VALID_CHARACTERS) {
        return VALID; 
    }
    else {
        return INVALID; 
    }

} /* end check_characters */

void thousand_generations(state arr[][MAX_COLS], state new_arr[][MAX_COLS])
{

    int i; 

    /* runs through 500 times to give 1000 wireworld boards */
    for (i = 0; i < END_ITERATION; i++) {
        printArray(arr); 

        /* add rules and put contents in new array*/
        add_rules(arr, new_arr);
        /* copy contents back into original array */
        copy_array(new_arr, arr); 

    }

} /* end thousand_generations */

void printArray(state arr[][MAX_COLS])
{

    int row, col;  
    
    for (row = 0; row < MAX_ROWS; row++) {
        for (col = 0; col < MAX_COLS; col++) {
            printf("%c", arr[row][col]); 
        }
        PRINT_NEWLINE; 
    }

} /* end printArray */

void add_rules(state arr[][MAX_COLS], state new_arr[][MAX_COLS])
{

    int row, col;

    /* copies entire board over to a new array, then 
    changes the  characters in array based on rules set */
    for (row = 0; row < MAX_ROWS; row++) {
        for (col = 0; col < MAX_COLS; col++) {
            copy_element(arr, new_arr, row, col);
            apply_rules(arr, new_arr, row, col); 
        }
    }

} /* end add_rules */

void copy_element(state arr[][MAX_COLS], state new_arr[][MAX_COLS], int row, int col)
{

    new_arr[row][col] = arr[row][col]; 

} /* end copy_array */

void apply_rules(state arr[][MAX_COLS], state new_arr[][MAX_COLS], int row, int col)
{

    /* access_array_oob returns arr[row][col] if elements are 
    within the array */
    if (access_array_oob(arr, row, col) == ELECTRON_HEAD) {
        new_arr[row][col] = ELECTRON_TAIL; 
    }
    else if (access_array_oob(arr, row, col) == ELECTRON_TAIL) {
        new_arr[row][col] = CONDUCTOR;
    }
    else if (access_array_oob(arr, row, col) == EMPTY) {
        new_arr[row][col] = EMPTY;
    }
    else if (access_array_oob(arr, row, col) == CONDUCTOR) {
        /* If number electron heads is 1 or 2, 'c' becomes 'H' */
        if (num_electron_heads(arr, row, col) == CORRECT_NUM) {
            new_arr[row][col] = ELECTRON_HEAD; 
        }
        else { /* no change */
            new_arr[row][col] = CONDUCTOR;
        }
    }
    else {
        new_arr[row][col] = arr[row][col]; 
    }

} /* end apply_rules */

/* Will exit the program if, for some reason, the array was 
accessed out of bounds, i.e. < 0 or > 40 */
int access_array_oob(state a[][MAX_COLS], int row, int col)
{

    /* exits program if array goes out of bounds, otherwise returns
    the element of the 2D arary */
    if ((col < 0) ||  (row < 0) || (col >= MAX_COLS) || (row >= MAX_ROWS)) {
        fprintf(stderr, "Error: accessed an array out of bounds\n");
        exit (EXIT_FAILURE); 
    }
    else {
        return a[row][col]; 
    }

} /* end access_array_oob */

int num_electron_heads(state a[][MAX_COLS], int row, int col)
{

    int num_heads; /* number of electron heads */

    /* check the 8 neighbouring cells against the current cell */
    check_neighbouring_cells(a, row, col, &num_heads);

    if (num_heads == 1 || num_heads == 2) {
        return 1; /* changes 'c' to 'H' */
    }
    else {
        return 0; /* no change */
    }

} /* end num_electron_heads */

/* This function will check the 8 neighbouring cells of a[r][c].
If the cells being checked are out of bounds, or is the cell 
itself, it will be ignored */
void check_neighbouring_cells(state a[][MAX_COLS], int row, int col, int* pNum)
{

    int i, j; 

    i = -1;
    *pNum = 0; /* pointer to number of electron heads */

    /* check cells from a[i - 1][j - 1] to a[i + 1][j + 1]*/
    while (i < CELL_ROW_LIMIT) {
        j = -1; /* start from column j - 1*/
        while (j < CELL_COL_LIMIT) {
            /* skip cell if checking out of bounds, e.g. a[-1][-1] */
            if ((check_cell_oob(row + i, col + j)) == OUT_OF_BOUNDS) {
                j++;              
            }
            /* skip the middle cell (the one being compared against) */
            else if (i == 0 && j == 0) {
                j++;                 
            }
            else {
                /* if cell contains 'H' add to total number of heads */
                if (a[row + i][col + j] == ELECTRON_HEAD) {
                    INCREMENT_NUM_HEADS; 
                }
            j++; 
            } 
        } 
        i++; 
    } 

} /* end check_neighbouring_cells */

/* This function determines whether a cell has been accessed which
is out of bounds of the array, i.e. < 0 or > 40 */
int check_cell_oob(int row, int col)
{

    if ((col < 0) ||  (row < 0) || (col >= MAX_COLS) || (row >= MAX_ROWS)) {
        return OUT_OF_BOUNDS;  
    }
    else {
        return IN_RANGE;  
    }

} /* end check_cell_oob */

void copy_array(state new_arr[][MAX_COLS], state arr[][MAX_COLS])
{

    int row, col;

    for (row = 0; row < MAX_ROWS; row++) {
        for (col = 0; col < MAX_COLS; col++) {
            arr[row][col] = new_arr[row][col]; 
        } 
    }

} /* end copy_array */
