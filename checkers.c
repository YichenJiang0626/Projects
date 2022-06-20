#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <math.h>

/* some #define's from my sample solution ------------------------------------*/
#define BOARD_SIZE          8       // board size
#define ROWS_WITH_PIECES    3       // number of initial rows with pieces
#define CELL_EMPTY          '.'     // empty cell character
#define CELL_BPIECE         'b'     // black piece character
#define CELL_WPIECE         'w'     // white piece character
#define CELL_BTOWER         'B'     // black tower character
#define CELL_WTOWER         'W'     // white tower character
#define COST_PIECE          1       // one piece cost
#define COST_TOWER          3       // one tower cost
#define TREE_DEPTH          5       // minimax tree depth
#define COMP_ACTIONS        100      // number of computed actions
#define INITIAL_SPACE		"  "    // board spacing at top left
#define FIRST_LETTER		'A'     // first column letter
#define FIRST_NUM		    1       // first row number
#define ODD_CHECK		    2       // used to check if a number is odd
#define HORIZONTAL_LINE     "+---+---+---+---+---+---+---+---+"
#define SEPARATION_LINE     "====================================="
#define VERTICAL_LINE		'|'     // vertical separation line
#define MAX_POSSIBLE		4		// maximum possible moves in a turn 
#define COORD				2	    // a coordinate takes 2 integers
#define CONVERT		 		32	    // difference in ASCII value between 											capital and lower case
#define FIRST				0		// first row
#define LAST				7		// last row
#define MAXPOSMOVE			4*12	// max possible number of moves by a 										   player

/* one type definition from my sample solution -------------------------------*/
typedef unsigned char board_t[BOARD_SIZE][BOARD_SIZE];  // board type
typedef struct {
	int x;
	int y;
} coordinate_t;

typedef struct {
	board_t board;
	int cost;    /* cost of board */
	char player; /* current player taking a move */
	int level;   /* level that this node is in */
	int numchild; /* number of children nodes this node has */
} information_t; /* this is the data stored in for eaach node */

typedef struct node node_t;

struct node {
	information_t info;
	node_t **children;
};

void print_board(board_t);
void fill_board(board_t);
void count_bw(board_t, int*, int*);
int calc_board_cost(board_t);
int check_move(board_t board, coordinate_t start, coordinate_t finish, char prev);
int all_possible(board_t board, coordinate_t point, int M[][COORD]);
int in_grid(coordinate_t point);
void moves(board_t board, coordinate_t point, int state, int M[][COORD]);
int same_piece(board_t board, coordinate_t point1, coordinate_t point2);
void branch(board_t board, coordinate_t point, coordinate_t point1, coordinate_t point2, int M[][COORD], int num);
void execute_move(board_t board, coordinate_t start, coordinate_t finish);
void board_copy(board_t board_orig, board_t board_copy);
node_t *make_root(board_t board, char curr_move);
node_t *generate_tree(node_t *node, char curr_move, int level);
node_t *create_node(board_t board, int lvl);
void fill_all_nodes_cost(node_t *node);
void fill_node_cost(node_t *node);
void find_start_finish(board_t initial, board_t final, int *start_x, char *start_y, int *finish_x, char *finish_y, char currmove);
void free_malloc(node_t *node);
int check_winner(board_t board, char curr_move);
void print_move(board_t board, int nummove, char y1, int x1, char y2, int x2, char* prev_move);

int main(int argc, char *argv[]) {
    
	int bcount = 0, wcount = 0, nummove = 0, i;
	char prev_move = CELL_WPIECE; /* game start with black */
	char curr_move = CELL_BPIECE, temp1y, temp2y, garbage; /* temporarily stores col num, then convert it to integer later */
	coordinate_t start, finish;
    board_t board; node_t *tree;
	/* fill initial board */
    fill_board(board);
	printf("BOARD SIZE: %dx%d\n", BOARD_SIZE, BOARD_SIZE);
	count_bw(board, &bcount, &wcount);
	printf("#BLACK PIECES: %d\n", bcount);
	printf("#WHITE PIECES: %d\n", wcount);
    print_board(board);
    
	while (scanf("%c%d-%c%d%c", &temp1y, &start.x, &temp2y, &finish.x, &garbage) == 5) {
		nummove++; start.x--; finish.x--;
		start.y = temp1y - FIRST_LETTER;
		finish.y = temp2y - FIRST_LETTER;
		if (check_move(board, start, finish, prev_move)) {
			/* this is a valid move */
			execute_move(board, start, finish);
		} else {
			continue;
		}
		print_move(board, nummove, temp1y, start.x+1, temp2y, finish.x+1, &prev_move);
		if (check_winner(board, curr_move) == 1) {
			return EXIT_SUCCESS;
		}

		nummove++;
		if (prev_move == CELL_BPIECE) {
			curr_move = CELL_WPIECE;
		} else if (prev_move == CELL_WPIECE) {
			curr_move = CELL_BPIECE;
		}
		if (check_winner(board, curr_move) == 1) {
			return EXIT_SUCCESS;
		}
		tree = make_root(board, curr_move);
		/* after creating root node, player would change again */
		tree = generate_tree(tree, curr_move, 0); 
			
		fill_all_nodes_cost(tree);
		fill_node_cost(tree);
			
		for (i = 0; i < tree->info.numchild; i++) {
			if (tree->children[i]->info.cost == tree->info.cost) {
				find_start_finish(board, tree->children[i]->info.board, &start.x, &temp1y, &finish.x, &temp2y, curr_move);
				board_copy(tree->children[i]->info.board, board);
				break;
			}
		}
			
		free_malloc(tree); free(tree); tree = NULL;
		/* finally free the handle */
		printf("%s\n", SEPARATION_LINE);
		if (prev_move == CELL_WPIECE) {
			printf("*** BLACK ACTION #%d: %c%d-%c%d\n", nummove, temp1y,start.x+1, temp2y, finish.x+1);
			prev_move = CELL_BPIECE;
		} else {
			printf("*** WHITE ACTION #%d: %c%d-%c%d\n", nummove, temp1y, start.x+1, temp2y, finish.x+1);
			prev_move = CELL_WPIECE;
		}
		printf("BOARD COST: %d\n", calc_board_cost(board));
		print_board(board);
	}
	
    return EXIT_SUCCESS;            // exit program with the success code
}

/*********************************/

/*********************************/
void print_board(board_t board) {
	/* prints out checker board in required format */
	int i, j;
	/* print first line*/
	printf("%s", INITIAL_SPACE);
	for (i = 0; i < BOARD_SIZE; i++) {
		printf("%4c", FIRST_LETTER + i); 
	}
	printf("\n");
	/* first line printing finished */
	
	/* print first horizontal separation line and the rest */
	printf("%s %s\n", INITIAL_SPACE, HORIZONTAL_LINE);
	for (i = 0; i < BOARD_SIZE; i++) {
		printf(" %d %c", i + 1, VERTICAL_LINE);
		for (j = 0; j < BOARD_SIZE; j++) {
			printf(" %c %c", board[i][j], VERTICAL_LINE);
		}
		printf("\n%s %s\n", INITIAL_SPACE, HORIZONTAL_LINE);
	}
} /* function finished */
/*********************************/

/*********************************/
void fill_board(board_t board) {
	/* initialise board array with either black, white, or empty pieces*/
	int i, j;	
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			if ((i + j) % ODD_CHECK && i < ROWS_WITH_PIECES) {
				/* fill white piece if the sum of board coordinates is odd,
				and cell is in the required rows */
				board[i][j] = CELL_WPIECE;
			} 
			else if ((i + j) % ODD_CHECK && 
				i >= BOARD_SIZE - ROWS_WITH_PIECES) {
				/* fill black piece if the sum of board coordinates is odd,
				and cell is in the required rows */
				board[i][j] = CELL_BPIECE;
			} 
			else {
				/* otherwise fill empty cell */
				board[i][j] = CELL_EMPTY;
			}
		}
	}			
} /* function finished */
/*********************************/

/*********************************/
/* count the number of black and white pieces */
void count_bw(board_t board, int *bpiece, int *wpiece) {
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == CELL_WPIECE || board[i][j] == CELL_WTOWER) {
				*wpiece += 1;
			} else if (board[i][j] == CELL_BPIECE || board[i][j] == CELL_BTOWER) {
				*bpiece += 1;
			}
		}
	}
} /* function finished */
/*********************************/

/*********************************/
int calc_board_cost(board_t board) {
	int i, j, cost = 0;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == CELL_WPIECE) {
				/* white piece deteced */
				cost -= COST_PIECE;
			} else if (board[i][j] == CELL_WTOWER) {
				/* white tower detected */
				cost -= COST_TOWER;
			} else if (board[i][j] == CELL_BPIECE) {
				/* black piece detected */
				cost += COST_PIECE;
			} else if (board[i][j] == CELL_BTOWER){
				/* black tower detected */
				cost += COST_TOWER;
			}
		}
	}
	return cost; /* function finished */
}
/*********************************/

/*********************************/
/* check if a move is valid */
int check_move(board_t board, coordinate_t start, coordinate_t finish, char prev) {
	/* x represent row number, y represent column number, prev represent previous turn taken (either 'w' or 'b') */

	int i;
	if (!in_grid(start)) {
		printf("ERROR: Source cell is outside of the board.\n");
		return 0;
	} else if (!in_grid(finish)) {
		printf("ERROR: Target cell is outside of the board.\n");
		return 0;
	} else if (board[start.x][start.y] == CELL_EMPTY) {
		printf("ERROR: Source cell is empty.\n");
		return 0;
	} else if (board[finish.x][finish.y] != CELL_EMPTY) {
		printf("ERROR: Target cell is not empty.\n");
		return 0;
	}
	if (prev == board[start.x][start.y] || prev == board[start.x][start.y] + CONVERT) {
		/* if same player taking turn */
		printf("ERROR: Source cell holds opponent's piece/tower.\n");
		return 0;
	}
	int M[MAX_POSSIBLE][COORD];
	/* this array stores all possible moves of a piece or tower */
	all_possible(board, start, M);
	for (i = 0; i < MAX_POSSIBLE; i++) {
		if (finish.x == M[i][0] && finish.y == M[i][1]) {
			/* this move is part of valid moves */
			return 1;
		}
	}
	printf("ERROR: Illegal action.\n");
	return 0; /* function finished */
}
/*********************************/

/*********************************/
/* find all possible moves of a piece */
int all_possible(board_t board, coordinate_t point, int M[][COORD]) {
	/* M[][COORD] takes all possible moves */
	int i, count = 0;
	for (i = 0; i < MAX_POSSIBLE; i++) {
		M[i][0] = M[i][1] = -1;
	}

	if (board[point.x][point.y] == CELL_WPIECE || board[point.x][point.y] == CELL_WTOWER) {
		moves(board, point, 1, M);
	} else if(board[point.x][point.y] == CELL_BPIECE || board[point.x][point.y] == CELL_BTOWER) {
		moves(board, point, -1, M);
	}
	

	for (i = 0; i < MAX_POSSIBLE; i++) {
		if (M[i][0] != -1) {
			count++;
		}
	}
	return count;
} 
/*********************************/

/*********************************/
/* check if a cell is in the grid of board*/
int in_grid(coordinate_t point) {
	return (point.x < BOARD_SIZE && point.x >= 0 
			&& point.y < BOARD_SIZE && point.y >= 0);
} /* function finished */
/*********************************/

/*********************************/
/* find all the possible moves of a piece / tower*/
void moves(board_t board, coordinate_t point, int state, int M[][COORD]) {
	/* here white has state 1, black has state -1 */
	coordinate_t point_left, point_right, point_2left, point_2right;
	point_left.x = point_right.x = point.x + state;
	point_left.y = point.y - 1;
	point_right.y = point.y + 1;
	point_2left.x = point_2right.x = point.x + state*2;
	point_2left.y = point.y - 2;
	point_2right.y = point.y + 2;
	/* just initialised all the points that a piece can move */

	/* implementing first two moves for pieces / towers, and label them as either 0th, 1st, 2nd or 3rd moves */
	if (board[point.x][point.y] == CELL_WPIECE || 
		board[point.x][point.y] == CELL_WTOWER) {
		branch(board, point, point_left, point_2left, M, 2);
		branch(board, point, point_right, point_2right, M, 1);
	} else if (board[point.x][point.y] == CELL_BPIECE || 
		board[point.x][point.y] == CELL_BTOWER) {
		branch(board, point, point_left, point_2left, M, 3);
		branch(board, point, point_right, point_2right, M, 0);
	}

	/* implementing the last two moves for towers only, their row value should reverse the first two */
	point_left.x = point_right.x = point.x - state;
	point_2left.x = point_2right.x = point.x - state*2;
	if (board[point.x][point.y] == CELL_WTOWER) {
		branch(board, point, point_left, point_2left, M, 3);
		branch(board, point, point_right, point_2right, M, 0);
	} else if (board[point.x][point.y] == CELL_BTOWER) {
		branch(board, point, point_left, point_2left, M, 2);
		branch(board, point, point_right, point_2right, M, 1);
	}
} /* function finished */
/*********************************/

/*********************************/
/* checks if two pieces are the same (or of the same team) */
int same_piece(board_t board, coordinate_t point1, coordinate_t point2) {
	if (board[point1.x][point1.y] == board[point2.x][point2.y] || 
	abs(board[point1.x][point1.y] - board[point2.x][point2.y]) == CONVERT) {
		return 1;
	}
	return 0;
} /* function finished */
/*********************************/

/*********************************/
/* check for a branch, does a piece move or capture, i.e., moving 1 left up or 2 left ups (to capture) */
void branch(board_t board, coordinate_t point, coordinate_t point1, coordinate_t point2, int M[][COORD], int num) {
	coordinate_t POINT_EMPTY;
	POINT_EMPTY.x = 0;
	POINT_EMPTY.y = 0; /* always going to be an empty cell */
	/* num is the move number */
	if (in_grid(point)) {
		/* 2 scnearios, move forward or capture */
		if (board[point1.x][point1.y] == CELL_EMPTY && in_grid(point1)) {
			/* need to move */
			M[num][0] = point1.x;
			M[num][1] = point1.y;
		} else if (!same_piece(board, point, point1) &&
				same_piece(board, point2, POINT_EMPTY) && 
				in_grid(point2)) {
			/* non empty cell, within grid, can capture*/
			M[num][0] = point2.x;
			M[num][1] = point2.y;
		}
	}
} /* function finished */
/*********************************/

/*********************************/
/* execute move by start and finish coordinates */
void execute_move(board_t board, coordinate_t start, coordinate_t finish) {
	board[finish.x][finish.y] = board[start.x][start.y];
	board[start.x][start.y] = CELL_EMPTY;
	if (abs(finish.x - start.x) == 2) {
		/* capture move detected*/
		board[(start.x+finish.x)/2][(start.y+finish.y)/2] = CELL_EMPTY;
	}
	
	int i;
	/* converting any qualified pieces to towers */
	for (i = 0; i < BOARD_SIZE; i++) {
		if (board[FIRST][i] == CELL_BPIECE) {
			board[FIRST][i] = CELL_BTOWER;
		} else if (board[LAST][i] == CELL_WPIECE) {
			board[LAST][i] = CELL_WTOWER;
		}
	}
}
/*********************************/

/*********************************/
node_t *generate_tree(node_t *node, char curr_move, int level)  {
	/* player curr_move is now making a move */
	node->info.player = curr_move;
	board_t curr_board, pos_board[MAXPOSMOVE], board_cpy;
	board_copy(node->info.board, curr_board);
	int pos_moves = 0, i, j, k, M[MAX_POSSIBLE][COORD], num_pos = 0; 
	/* pos_moves is number of possible moves */
	/* num_pos will be the number of moves implemented */
	coordinate_t coordinate, finish;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			if (curr_board[i][j] == curr_move || curr_board[i][j] + CONVERT 	== curr_move) {
				coordinate.x = i; coordinate.y = j;
				pos_moves += all_possible(curr_board, coordinate, M);	
				/* now for all these possible moves, implement them into pos_board array */
				for (k = 0; k < MAX_POSSIBLE; k++) {
					if (M[k][0] != -1) {
						finish.x = M[k][0]; finish.y = M[k][1];
						board_copy(curr_board, board_cpy);
						execute_move(board_cpy, coordinate, finish);
						board_copy(board_cpy, pos_board[num_pos]);
						num_pos++;
					}
				}
				assert(num_pos == pos_moves);
				/* finished implementing, checked, all good */
			}
		}
	}
	/* now finished counting the number of children nodes*/
	if (pos_moves) { 
		node->info.numchild = pos_moves;
		node->children = (node_t **)malloc(pos_moves * sizeof(node));
		if (curr_move == CELL_WPIECE) {
			curr_move = CELL_BPIECE;
		} else {
			curr_move = CELL_WPIECE;
		}
	
		for (i = 0; i < pos_moves; i++) {
			node->children[i] = create_node(pos_board[i], level + 1);
		}
	
		if (level < TREE_DEPTH - 1) {
			for (i = 0; i < pos_moves; i++) {
				generate_tree(node->children[i], curr_move, level+1);
			}
		} else if (level == TREE_DEPTH - 1) {
			for (i = 0; i < pos_moves; i++) {
				node->children[i]->info.cost = calc_board_cost(node->children[i]->info.board);
				node->children[i]->info.player = curr_move;
			}
		}
	} else {
		node->children = NULL;
		node->info.numchild = 0;
	}

	return node;
}
/*********************************/

/*********************************/
/* create one new node */
node_t *create_node(board_t board, int lvl) {
	node_t *new_node;
	new_node = (node_t *)malloc(sizeof(node_t));
	board_copy(board, new_node->info.board);
	new_node->children = NULL; new_node->info.level = lvl;
	return new_node;
}
/*********************************/

/*********************************/
/* make the root of the minimax tree */
node_t *make_root(board_t board, char curr_move) {
	node_t *rootnode;
	rootnode = (node_t *)malloc(sizeof(node_t));
	assert(rootnode!=NULL);
	rootnode->children = NULL; rootnode->info.level = 0;
	board_copy(board, rootnode->info.board);
	rootnode->info.player = curr_move;
	return rootnode;
}
/*********************************/

/*********************************/
/* fill the cost for the current nodes */
void fill_node_cost(node_t *node) {
	if (node->info.numchild == 0) {
		if (node->info.player == CELL_BPIECE) {
			node->info.cost = INT_MIN;
		} else {
			node->info.cost = INT_MAX;
		}
		/* no children nodes */
		return;
	}

	int optimal_cost = node->children[0]->info.cost, i;
	/* first initialize optimal cost to be the first children cost, if there are better costs, replace it */
	if (node->info.player == CELL_WPIECE && node->children != NULL) {
		/* want to minimize cost */
		for (i = 0; i < node->info.numchild; i++) {
			if (node->children[i]->info.cost < optimal_cost) {
				optimal_cost = node->children[i]->info.cost;
			}
		}
	} else if (node->info.player == CELL_BPIECE && node->children != NULL) {
		/* want to maximise cost */
		for (i = 0; i < node->info.numchild; i++) {
			if (node->children[i]->info.cost > optimal_cost) {
				optimal_cost = node->children[i]->info.cost;
			}
		}
	}
	node->info.cost = optimal_cost;
}
/*********************************/

/*********************************/
/* fill the cost for all nodes */
void fill_all_nodes_cost(node_t *node) {
	int i;
	if (node->info.level < TREE_DEPTH - 1 && node->info.numchild != 0) {
		/* if not at one before 3rd level, keep recursion */
		for (i = 0; i < node->info.numchild; i++) {
			/* fill its children nodes' cost first */
			fill_all_nodes_cost(node->children[i]);
			fill_node_cost(node->children[i]);
		}
	} else if (!node->info.numchild || node->info.level == TREE_DEPTH - 1) {
		fill_node_cost(node);
	}
}
/*********************************/

/*********************************/
/* check the difference in two boards to find start / finish coordinates */
void find_start_finish(board_t initial, board_t final, int *start_x, char *start_y, int *finish_x, char *finish_y, char currmove) {
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			if (final[i][j] == CELL_EMPTY && (initial[i][j] == currmove || initial[i][j] == currmove - CONVERT)) {
				/* found starting coordinate */
				*start_x = i; *start_y = j + 65;
			} else if (final[i][j] != CELL_EMPTY && initial[i][j] == CELL_EMPTY) {
				/* found finish coordinate */
				*finish_x = i; *finish_y = j + 65;
			}
		}
	}
}
/*********************************/

/*********************************/
/* makes a copy of a board configuration */
void board_copy(board_t board_orig, board_t board_copy) {
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			board_copy[i][j] = board_orig[i][j];
		}
	}
}
/*********************************/

/*********************************/
/* freeing malloc'ed space into the heap */
void free_malloc(node_t *node) {
	int i;
	if (node->info.level < TREE_DEPTH - 1) {
		for (i = 0; i < node->info.numchild; i++) {
			free_malloc(node->children[i]);
			/* now this node should become a leaf node */
			free(node->children[i]);
			node->children[i] = NULL;
		}
	} else if (node->info.level == TREE_DEPTH - 1) {
		for (i = 0; i < node->info.numchild; i++) {
			/* freeing leaf nodes */
			free(node->children[i]);
			node->children[i] = NULL;
		}
	}
}
/*********************************/

/*********************************/
/* check if any player wins at this moment */
int check_winner(board_t board, char curr_move) {
	int i, j, black_count = 0, white_count = 0, moves[MAX_POSSIBLE][COORD];
	count_bw(board, &black_count, &white_count);
	coordinate_t coordinate;
	/* return 1 if have a winner */
	if (black_count == 0) {
		printf("WHITE WIN!\n");
		return 1;
	} else if (white_count == 0) {
		printf("BLACK WIN!\n");
		return 1;
	}
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			coordinate.x = i; coordinate.y = j;
			if (board[i][j] == curr_move || board[i][j] == curr_move - 			CONVERT) {
				if (all_possible(board, coordinate, moves) != 0) {
					/* there are still moves possible */
					return 0;
				}
			}
		}
	}
	if (curr_move == CELL_BPIECE) {
		printf("WHITE WIN!\n");
	} else {
		printf("BLACK WIN!\n");
	}
	return 1;
}
/*********************************/

/*********************************/
void print_move(board_t board, int nummove, char y1, int x1, char y2, int x2, char* prev_move) {
	printf("%s\n", SEPARATION_LINE);
	if (*prev_move == CELL_WPIECE) {
		printf("*** BLACK ACTION #%d: %c%d-%c%d\n", nummove, y1, x1, y2, x2);
		*prev_move = CELL_BPIECE;
	} else {
		printf("*** WHITE ACTION #%d: %c%d-%c%d\n", nummove, y1, x1, y2, x2);
		*prev_move = CELL_WPIECE;
	}
	printf("BOARD COST: %d\n", calc_board_cost(board));
	print_board(board);
}
/*********************************/
/* ALGORITHMS ARE FUN */
/* THE END -------------------------------------------------------------------*/



/*

discard_tree(root, 0);





void discard_tree(node_t* root, int depth) {
    int i;
    if (root == NULL) {
        return;
    }
    if (root->num_children != 0 || depth < 2) {
        discard_tree(, depth+1);
    }
    free(root);
    root = NULL;

}
*/


