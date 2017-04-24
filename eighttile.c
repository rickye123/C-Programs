/* This program solves the chinese 8-tile puzzle 
problem. A file containg a puzzle is inserted via the 
command line by the user and the  program attempts to solve it. 

If the puzzle is solved, the solution (including the
number of moves) will be printed on the terminal. 

This version uses a dynamic linked list to solve
the problem. Pointers of type struct tileboard 
are used, however, as if the data structure was
a queue. This made it easy to add elements to the 
end of the queue (as the front of the queue was 
never changed throughout).

In Linux terminals, this program will simulate the 
solution in an animation. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "neillncurses.h"

#define SIZE 3
#define NUM_OF_VALID_ARGUMENTS 2
#define PRINT_NEWLINE puts("")
#define SPACE ' '
#define VALID 0
#define INVALID 1
#define VALID_CHARS (c == '\r') || (c == ' ') || (c == '\n') || (c == EOF) || ((c <= '8') && (c >= '1'))
#define SPACE_CHARACTERS isspace(first_board[i][j]) && (first_board[i][j] != '\r') && (first_board[i][j] != '\n')
#define MAX_NUM_OF_SPACES 1
#define GOING_LEFT j - 1
#define GOING_RIGHT j + 1
#define GOING_UP i - 1
#define GOING_DOWN i + 1
#define DOES_NOT_ALREADY_EXIST 0
#define ALREADY_DOES_EXIST 1
#define POSITION_FOUND 1
#define POSITION_NOT_FOUND 0
#define LOWER_BOUND 0
#define UPPER_BOUND 2
#define PROBLEM_SOLVED 1
#define PROBLEM_NOT_SOLVED 0
#define MAX_NUM_OF_POSSIBLE_BOARDS 100000
#define GOAL_BOARD 1
#define SOLUTION_NOT_YET_FOUND 0
#define FIRST_GRID_NUMBER 0
#define EXIT_SUCCESSFUL 0
#define EXIT_FAILURE 1
#define SKIP_COMPARISON continue
#define SAME_BOARDS 0
#define PUZZLE_SIZE 21
#define BORDER_UPPER 13
#define BORDER_LOWER 7
#define PAD_LOWER 9
#define PAD_UPPER 11 
#define LEFT_BORDER ((i > BORDER_LOWER && i < BORDER_UPPER) && (j >= BORDER_LOWER && j < PAD_LOWER))
#define RIGHT_BORDER ((i > BORDER_LOWER && i < BORDER_UPPER) && (j > PAD_UPPER && j <= BORDER_UPPER))
#define TOP_BORDER ((i > BORDER_LOWER && i < PAD_LOWER) && (j >= PAD_LOWER && j <= PAD_UPPER))
#define BOTTOM_BORDER (( i >= PAD_UPPER && i < BORDER_UPPER) && (j >= PAD_LOWER && j <= PAD_UPPER))
#define PADDING '-'
#define BORDER '_'
#define MOVES_REQUIRED moves - 1
#define ONE_SECOND_DELAY 1000
#define THREE_SECOND_DELAY 3000

struct tileboard {
    char grid[SIZE][SIZE]; /* 2D array of size 3 by 3 */
    int parent_cell; /* used to keep track of grid's parent grid */
    struct tileboard *next;
};

enum status {SOLVED, UNSOLVED};

typedef enum status Status;
typedef struct tileboard Board;
typedef char tile; 

void check_invalid_argument(int argc, char *argv[]);
void initialise_list(Board **front, Board **back, Board **current);
void open_file (FILE **fp, char *argv[]);
void read_file(FILE *fp, tile first_board[][SIZE]);
int check_char_valid(char c);
void check_spaces(tile first_board[][SIZE]);
void check_if_number_already_present(tile first_board[][SIZE]);
Board * allocate_space(tile current_board[][SIZE], int *parent);
void set_attributes(NCURS_Simplewin *sw); 
int find_solution(Board **front, Board **back, Board **temp, Board **current, int *cnt, int *parent);
int find_position(Board **temp, int *i, int *j);
int move_directions(Board **front, Board **back, Board **temp, Board **current, int *cnt, int i, int j, int *parent);
int move_left(Board **front, Board **back, Board **temp, Board **current, int *cnt, int i, int j, int *parent);
int move_right(Board **front, Board **back, Board **temp, Board **current, int *cnt, int i, int j, int *parent);
int move_up(Board **front, Board **back, Board **temp, Board **current, int *cnt, int i, int j, int *parent);
int move_down(Board **front, Board **back, Board **temp, Board **current, int *cnt, int i, int j, int *parent);
void swap(char *element1, char *element2);
int change_board (Board **front, Board **back, Board **current, int *cnt);
int already_exists(Board **front, Board **back, Board *tile);
int is_goal_reached(tile current_board[][SIZE]);
void print_solution(Board **back, Board **front, int *cnt, NCURS_Simplewin *sw);
void print_correct_order(Board **front, int store[], int *i, NCURS_Simplewin *sw);
void print_original_order(Board **front, Board *temp, int *i, int *parent, int *cell, int *store);
void print_grid(tile grid[][SIZE]);
void print_list(Board *head);
void create_border_array(Board **current, char tempArray[][PUZZLE_SIZE]);

int main(int argc, char *argv[])
{

    FILE *fp; /* pointer to filename */
    /* 2D array of size 3 by 3, which is the first board read in by the file  */
    tile first_board[SIZE][SIZE]; 
    int parent, cnt; /* parent used to keep track of the parent_cell and cnt used
    to keep track of number of possible boards */
    NCURS_Simplewin sw; /* initialise mouse / keyboard events */
    /* Board pointers necessary for list implementation */
    Board *front, *back, *current, *temp;
    
    /* if number of arguments passed to terminal is not 2 exit */
    check_invalid_argument(argc, &argv[0]);

    initialise_list(&front, &back, &current); /* sets list pointers to NULL */

    /* open file whose title is the second argument entered by the user */
    open_file(&fp, argv);

    /* read file and exit if any invalid characters, extra spaces, or repeat 
    numbers are present */
    read_file(fp, first_board);

    fclose(fp);

    Neill_NCURS_Init(&sw); 
    /* set color attributes for graphical display */
    set_attributes(&sw); 

    /* set cnt and parent to 0 before first board is made */
    parent = cnt = 0;

    /* create first board - read in from file */
    front = temp = current = back = allocate_space(first_board, &parent);

    /* find solution - exit if can't find solution after 100,000 boards are printed */
    if(find_solution(&front, &back, &temp, &current, &cnt, &parent) == PROBLEM_NOT_SOLVED) {
        fprintf(stdout, "There is no solution to this puzzle\n");
        exit(EXIT_FAILURE);
    }
    
    /* Call this function if we exit() anywhere in the code */
    atexit(Neill_NCURS_Done);

    /* This function will use ncurses and display the solution graphically */
    print_solution(&back, &front, &cnt, &sw);

    exit(EXIT_SUCCESSFUL);

} /* end main */

void check_invalid_argument(int argc, char *argv[])
{

    if (argc != NUM_OF_VALID_ARGUMENTS) {
        fprintf(stderr, "Error: Incorrect usage, try e.g. %s 1.8tile\n", argv[0]);
        exit(EXIT_FAILURE);
    }

} /* end check_invalid_argument */

void initialise_list(Board **front, Board **back, Board **current)
{

    /* initialise the Board pointers to NULL */
    *front = *back = *current = NULL;

} /* end initialise_list */

void open_file (FILE **fp, char *argv[])
{

    /* open file whose title is the second argument entered by the user */
    if ((*fp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Error: Cannot open file\n");
        exit(EXIT_FAILURE);
    }

} /* end open_file */

void read_file(FILE *fp, char first_board[][SIZE])
{

    int i, j, row, col;
    char c; /* characters read in by file */
    i = row = 0;

    while (row < SIZE) {
        j = col = 0;
        /* add characters to grid while not EOF.
        Go to second row of array when col is 3 */
        while (((c = getc(fp)) != EOF) && (col < SIZE)) {
            /* if newline or carraige return character, 
            skip and do not add to 2D array tile */
            if (c == '\r' || c == '\n') {
                j++; /* SKIP */
            }
            else {
                first_board[row][col] = c;
                j++;
                col++;
            }
            /* exit if invalid characters found */
            if(check_char_valid(c) == INVALID) {
                fprintf(stderr, "Error: Invalid character in file\n");
                exit(EXIT_FAILURE);
            }
        }
        i++;
        row++;
    }

    check_spaces(first_board); /* check number of spaces */
    check_if_number_already_present(first_board); /* check for duplicate numbers */

} /* end read_file */

/* checks to see if any character bar ' ', '\n', '\r',
EOF, or '1' to '8' are present in file */
int check_char_valid(char c)
{

    if (VALID_CHARS) {
        return VALID;
    }
    else {
        return INVALID;
    }

} /* end check_char_valid */

/* This function checks to see how many spaces there 
are in the file. If more than 1 space, file is invalid */
void check_spaces(char first_board[][SIZE])
{

    int i, j;
    int num_of_spaces = 0;

    /* count number of spaces */
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            if(SPACE_CHARACTERS) {
                num_of_spaces++;
            }
        }
    }

    /* file invalid if more than one space */
    if (num_of_spaces > MAX_NUM_OF_SPACES) {
        fprintf(stderr, "Error: Invalid file. More than one space\n");
        exit(EXIT_FAILURE);
    }

} /* end check_spaces */

/* This function checks to see whether there are duplicate 
numbers in the file. If same number appears, file is invalid */
void check_if_number_already_present(char first_board[][SIZE])
{

    /* compare first_board[i][j] against first_board[x][y] to determine
    if there are any duplicates */
    int i, j, x, y;

    for (i = 0; i < SIZE ; i++) {
        for (j = 0; j < SIZE; j++) {
            /* use x and y to compare elements in the same array */
            for (x = 0; x < SIZE ; x++) {
                for (y = 0; y < SIZE; y++) {
                    if(i == x && j == y) {
                        /* skip - otherwise will find duplicates when 
                        comparing two elements in the same position, 
                        e.g. first_board[0][1] against first_board[0][1] */
                        SKIP_COMPARISON;
                    }
                    /* exit program if duplicate numbers in file */
                    if(first_board[i][j] == first_board[x][y]) {
                        fprintf(stderr, "Error: Duplicate numbers in file\n");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
    }

} /* end check_if_number_already_present */

/* This function allocates space for a struct tileboard 
(aka Board) */
Board * allocate_space(tile currrent_board[][SIZE], int *parent)
{

    Board *new_board;
 
    /* allocates memory of size Board */
    new_board = (Board *)malloc(sizeof(Board));

    /* if not enough memory, exit program */
    if (new_board == NULL) {
        fprintf(stderr, "Cannot allocate space. Not enough memory\n");
        exit(EXIT_FAILURE);
    }

    /* copy the tile being created into grid inside Board */
    memcpy(new_board->grid, currrent_board, SIZE*SIZE);
    /* copy the parent into parent_cell inside Board */
    memcpy(&(new_board)->parent_cell, parent, sizeof(int));

    /* make next pointer point to NULL */
    new_board->next = NULL;

    return new_board;

} /* end allocate_space */

void set_attributes(NCURS_Simplewin *sw)
{

    /* set the numbers in the board to have white background and red letters. 
    Space character will be a blue square and other characters in file will 
    be black. A border will appear outside the board in red */
    Neill_NCURS_CharStyle(sw, "12345678", COLOR_RED, COLOR_WHITE, A_BOLD);
    Neill_NCURS_CharStyle(sw, " ", COLOR_BLUE, COLOR_BLUE, A_NORMAL);
    /* make newline, carriage return and '-' characters black */
    Neill_NCURS_CharStyle(sw, "\n\r-", COLOR_BLACK, COLOR_BLACK, A_NORMAL);
    Neill_NCURS_CharStyle(sw, "_", COLOR_RED, COLOR_RED, A_NORMAL);

} /* end set_attributes */

/* This function finds the position of the ' ' character and moves left, 
right, down, and up until the solution board is found */
int find_solution(Board **front, Board **back, Board **temp, Board **current, int *cnt, int *parent)
{

    int i, j;
    Status goal_status; /* can either be SOLVED or UNSOLVED */
    goal_status = UNSOLVED;
    (*cnt)++; /* Add 1 to count (because first grid was already made)*/

    fprintf(stdout, "Solving puzzle...\n");

    /* keep iterating until goal grid is found */
    while(goal_status != SOLVED) {
        /* find the position of the space character - exit if not found */
        if(find_position(&(*temp), &i, &j) == POSITION_NOT_FOUND) {
            fprintf(stderr, "Error: Position not found. Exiting program.\n");
            exit(EXIT_FAILURE); 
        }
        /* move left, right, up and down and end if final board found */
        if (move_directions(&(*front), &(*back), &(*temp), &(*current), &(*cnt), i, j, &(*parent)) == PROBLEM_SOLVED) {
            goal_status = SOLVED; 
            return PROBLEM_SOLVED;
        }
        (*parent)++; /* move to next level in tree */
        *temp = (*temp)->next; /* point temp (which is current parent grid) to the next grid */

        /* if number of boards exceeds 100,000, there is no solution */
        if (*cnt > MAX_NUM_OF_POSSIBLE_BOARDS) {
            return PROBLEM_NOT_SOLVED;
        }
    }
    return SOLUTION_NOT_YET_FOUND; 

} /* end find_solution */

/* This function finds the position of each parent grid. 
It searches through the 2D grid to find a space - which 
indicates where to start */
int find_position(Board **temp, int *i, int *j)
{

    /* temp holds the board of the parent grid, so this
    needs to be checked  */
    for (*i = 0; *i < SIZE; (*i)++) {
        for (*j = 0; *j < SIZE; (*j)++) {
            if ((**temp).grid[*i][*j] == SPACE) {
                return POSITION_FOUND;
            }
        }
    }
    return POSITION_NOT_FOUND;

} /* end find_position */

/* This function moves left, right, up and down until the goal
board is found */
int move_directions(Board **front, Board **back, Board **temp, Board **current, int *cnt, int i, int j, int *parent)
{

    if(move_left(&(*front), &(*back), &(*temp), &(*current), &(*cnt), i, j, &(*parent)) == GOAL_BOARD) {
        return PROBLEM_SOLVED;
    }
    if(move_right(&(*front), &(*back), &(*temp), &(*current), &(*cnt), i, j, &(*parent)) == GOAL_BOARD) {
        return PROBLEM_SOLVED;
    }
    if(move_down(&(*front), &(*back), &(*temp), &(*current), &(*cnt), i, j, &(*parent)) == GOAL_BOARD) {
        return PROBLEM_SOLVED;
    }
    if(move_up(&(*front), &(*back), &(*temp), &(*current), &(*cnt), i, j, &(*parent)) == GOAL_BOARD){
        return PROBLEM_SOLVED;
    }
    return PROBLEM_NOT_SOLVED;

} /* end move_directions  */

int move_left(Board **front, Board **back, Board **temp, Board **current, int *cnt, int i, int j, int *parent)
{

    /* copies temp into current->next and the back of the queue */
    (*current)->next = (*back) = allocate_space((char(*)[SIZE])*temp, &(*parent));

    /* bounds check - makes sure that j - 1 does not go out of bounds. 
    If [i][j - 1] is not a space character, then we can move the space here */
    if(((GOING_LEFT) >= LOWER_BOUND) && (*current)->next->grid[i][j - 1] != SPACE) {

        /* swap characters - move space left */
        swap(&(*current)->next->grid[i][j], &(*current)->next->grid[i][j - 1]);
        
        /* check to see if current->next already exists. If not, make it the current 
        board and check to see if it is the goal grid */
        if(change_board(&*front, &*back, &(*current), cnt) == PROBLEM_SOLVED) {
            return PROBLEM_SOLVED;
        }

    }
    return SOLUTION_NOT_YET_FOUND;

} /* end move_left */

int move_right(Board **front, Board **back, Board **temp, Board **current, int *cnt, int i, int j, int *parent)
{

    /* copies temp into current->next and the back of the queue  */
    (*current)->next = (*back) = allocate_space((char(*)[SIZE])*temp, &(*parent));

    /* bounds check - make sure that j + 1 does not go out of bounds. 
    If [i][j + 1] is a space, we can move here */
    if(((GOING_RIGHT) <= UPPER_BOUND) && (*current)->next->grid[i][j + 1] != SPACE) {

        /* swap characters - move space right */
        swap(&(*current)->next->grid[i][j], &(*current)->next->grid[i][j + 1]);

        if(change_board(&*front, &*back, &(*current), cnt) == PROBLEM_SOLVED) {
            return PROBLEM_SOLVED;
        }

    }
    return SOLUTION_NOT_YET_FOUND;

} /* end move_right */

int move_down(Board **front, Board **back, Board **temp, Board **current, int *cnt, int i, int j, int *parent)
{

    /* copies temp into current->next and the back of the queue  */
    (*current)->next = (*back) = allocate_space((char(*)[SIZE])*temp, &(*parent));

    /* bounds check - make sure that i + 1 does not go out of bounds. 
    If [i + 1][j] is a space, we can move here */
    if(((GOING_DOWN) <= UPPER_BOUND) && (*current)->next->grid[i + 1][j] != SPACE) {
        /* swap characters - move space down */
        swap(&(*current)->next->grid[i][j], &(*current)->next->grid[i + 1][j]);

        if(change_board(&*front, &*back, &(*current), cnt) == PROBLEM_SOLVED) {
            return PROBLEM_SOLVED;
        }

    }
    return SOLUTION_NOT_YET_FOUND;

} /* end move_down */

int move_up(Board **front, Board **back, Board **temp, Board **current, int *cnt, int i, int j, int *parent)
{

    /* copies temp into current->next and the back of the queue */
    (*current)->next = (*back) = allocate_space((char(*)[SIZE])*temp, &(*parent));

    /* bounds check - make sure that i - 1 does not go out of bounds. 
    If [i - 1][j] is a space, we can move here */
    if(((GOING_UP) >= LOWER_BOUND) && (*current)->next->grid[i - 1][j] != SPACE) {

        /* swap characters - move space up */
        swap(&(*current)->next->grid[i][j], &(*current)->next->grid[i - 1][j]);

        if(change_board(&*front, &*back, &(*current), cnt) == PROBLEM_SOLVED) {
            return PROBLEM_SOLVED;
        }

    }
    return SOLUTION_NOT_YET_FOUND;

} /* end move_down */

void swap(char *element1, char *element2)
{

    char temp;

    temp = *element1;
    *element1 = *element2;
    *element2 = temp;

} /* end swap */

int change_board (Board **front, Board **back, Board **current, int *cnt)
{

    /* Check to see whether a board already exists in the list */
    if (already_exists(&*front, &*back, (*current)->next) == DOES_NOT_ALREADY_EXIST) {
        
        /* if board does not exist, make current->next the current board, hence
        adding it to the list */
        (*current) = (*current)->next; 
        
        /* check to see whether this board is the goal board */
        if(is_goal_reached((*current)->grid) == PROBLEM_SOLVED) {
            return PROBLEM_SOLVED;
        }

        (*cnt)++; /* add 1 to count as this is a possible board */
    }
    return SOLUTION_NOT_YET_FOUND;

} /* end change_board */

int already_exists(Board **front, Board **back, Board *tile)
{

    Board *current;

    current = *front;

    /* start from front of queue and see if tile already exists.
    If board is found then do not add it to list */
    while(current != *back) {
        if(memcmp(tile, current, SIZE*SIZE) == SAME_BOARDS) {
            return ALREADY_DOES_EXIST;
        }
        current = current->next; 
    }
    return DOES_NOT_ALREADY_EXIST;

} /* end already_exists */

/* This function will determine whether the current_board is 
equal to the goal_board */
int is_goal_reached(tile current_board[][SIZE])
{

    /* This is the board we are trying to reach */
    tile goal_board[SIZE][SIZE] = { {'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', ' '} };

    /* if comparison of both boards is 0, then they are equivalent */
    if(memcmp(goal_board, current_board, SIZE*SIZE) == SAME_BOARDS) {
        return PROBLEM_SOLVED;
    }
    return SOLUTION_NOT_YET_FOUND;

} /* end is_goal_reached */

void print_solution(Board **back, Board **front, int *cnt, NCURS_Simplewin *sw)
{

    int parent, i; 
    int cell = *cnt; /* make cell equal to the total number of possible boards */

    Board *temp;
    /* initialise all array elements to 0. This will set all elements 
    in array store that aren't used. This will be used to print the 
    first grid */
    int *store = {0};

    /* makes store the size of number of possible boards */
    store = malloc(sizeof(int)*(*cnt)); 

    temp = *back; /* make temp the final board (goal board) */
    parent = (*back)->parent_cell; /* find the parent cell of goal board */

    /* This prints the solution backwards - from end board to start */
    print_original_order(front, temp, &i, &parent, &cell, store);
    /* This function prints in the correct order - from start to end */
    print_correct_order(front, store, &i, sw);

} /* end print_solution */

void print_original_order(Board **front, Board *temp, int *i, int *parent, int *cell, int *store)
{

    Board *current; 
    int counter; 
    *i = 0; 

    /* This will populate the array store with the parent
    cell values. */
    while (*cell != FIRST_GRID_NUMBER) {
        store[*i] = *cell;
        (*i)++; 
        counter = 0; /* set counter to 0 each time we go through the loop */
        current = *front; /* set current to the front of queue */

        /* use counter to find the position of each grid. Start from front of 
        list and pointer chase until  counter == parent, and hence the grid is found */
        while (counter != *parent) {
            current = current->next; 
            counter++; 
        }
        /* make parent the current parent cell and make cell the temp parent cell. 
        Temp starts at the back of the queue and then becomes current (the parent grid) */
        *parent = current->parent_cell;
        *cell = temp->parent_cell; 
        temp = current; 
    }

} /* end print_original_order */

void print_correct_order(Board **front, int *store, int *i, NCURS_Simplewin *sw)
{

    int counter, moves, current_count, temp;
    Board *current; 
    char tempArray[PUZZLE_SIZE][PUZZLE_SIZE]; /* 21 by 21 array */
    moves = 0; /* number of moves to find solution to 0 */

    /* The array store contains all the parent cells. This loop will find the 
    parent grids of each grid and print these */
    for (counter = *i; counter >= 0; counter--) {
        current = *front; 
        temp = store[counter]; 

        /* temp is the parent cell number. While current_count is not equal to the 
        parent cell value, pointer chase along the list until the parent cell, 
        and hence parent grid, is found. */
        current_count = 0; 
        while (current_count != temp) {
            current = current->next; 
            current_count++; 
        }

        /* This creates a temporary array and puts current->grid
        into the middle of it */
        create_border_array(&current, tempArray);
        moves++;

        /* print array of size 21 by 21 and then delay 1 second  */
        Neill_NCURS_PrintArray(&tempArray[0][0], PUZZLE_SIZE, PUZZLE_SIZE, sw);
        Neill_NCURS_Delay(ONE_SECOND_DELAY); /* 1 second delay */
    }

    fprintf(stdout, "\n\nNumber of moves required: %d\n", MOVES_REQUIRED);
    Neill_NCURS_Delay(THREE_SECOND_DELAY); /* 3 second delay */

    free(store);

} /* end print_correct_order */

void create_border_array(Board **current, char tempArray[][PUZZLE_SIZE])
{

    int i, j;

    for (i = 0; i < PUZZLE_SIZE; i++) {
        for(j = 0; j < PUZZLE_SIZE; j++) {
            /* This is where the puzzle will be put into the array. 
            PAD_LOWER is 9 and PAD_UPPER is 11 */
            if((i >= PAD_LOWER && i <= PAD_UPPER) && (j >= PAD_LOWER && j <= PAD_UPPER)) {
                tempArray[i][j] = (*current)->grid[i - PAD_LOWER][j - PAD_LOWER];
            }
            /* This creates a border, which is 2 cells wide */
            else if (LEFT_BORDER || RIGHT_BORDER || TOP_BORDER || BOTTOM_BORDER){
                tempArray[i][j] = BORDER;
            }
            /* This makes all the remaining elements of the array padding, allowing
            the board to be displayed in the centre of the screen */
            else {
                tempArray[i][j] = PADDING;
            }
        }
    }

} /* end create_border_array */

/* These functions were not used in the final build - IGNORE */

void print_list(Board *head)
{

    int cnt = 0;

    /* exit program if list is empty */
    if(head == NULL) {
        fprintf(stderr, "List is empty\n");
        exit(EXIT_FAILURE);
    }

    puts("\nStart of list:\n");
    while (head != NULL) {

        printf("\nGrid %d", cnt);
        cnt++;
        print_grid(head->grid);
        printf("Parent: %d\n", head->parent_cell);
        head = head->next;
    }
    puts("\nEnd of list\n");

} /* end print_list */

void print_grid(tile grid[][SIZE])
{

    int i, j;

    /* This function prints a grid. The '-' characters
    are used to add a border */
    for (i = 0; i < SIZE; i++) {
        printf("\n---------------\n");
        for (j = 0; j < SIZE; j++) {
            printf("| %c |", grid[i][j]);
        }
    }
    printf("\n---------------\n");
    puts("");

} /* end print_grid */