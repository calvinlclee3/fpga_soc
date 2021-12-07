#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alt_types.h>
#include "game_ctrl.h"

#define NOT_SELECTED -1
#define CHECK_SQUARE_EMPTY(r,c) ((vga_ctrl->SQUARES[(r) * 8 + (c)] & 0x0F) == EMPTY_SQUARE)
#define CHECK_SQUARE_ENEMY(r,c,p) (((vga_ctrl->SQUARES[(r) * 8 + (c)] & 0x0F) != EMPTY_SQUARE) && ((vga_ctrl->SQUARES[(r) * 8 + (c)] & 0x01) != p))
#define CHECK_SQUARE_EMPTY_OR_ENEMY(r,c,p) (CHECK_SQUARE_EMPTY(r,c) || CHECK_SQUARE_ENEMY(r,c,p))
#define HILITE(r,c) vga_ctrl->SQUARES[(r) * 8 + (c)] |= 0x10; nPlaces++;

static alt_u8 selectedX = NOT_SELECTED;
static alt_u8 selectedY = NOT_SELECTED;

static alt_u8 player_turn;

typedef enum {

	DESELECT,
    START,
	INIT_POS_DOWN,
	INIT_POS_UP,
	FINAL_POS_DOWN,
	FINAL_POS_UP

} state;

static state st;

void initGame()
{
	/* Initialize pawn row and empty rows */
	for(int i = 0; i < 8; i++)
	{
		vga_ctrl->SQUARES[1 * 8 + i] = 0x0;
		vga_ctrl->SQUARES[2 * 8 + i] = 0xc;
		vga_ctrl->SQUARES[3 * 8 + i] = 0xc;
		vga_ctrl->SQUARES[4 * 8 + i] = 0xc;
		vga_ctrl->SQUARES[5 * 8 + i] = 0xc;
		vga_ctrl->SQUARES[6 * 8 + i] = 0x1;
	}

	/* Initialize row 0 */
    vga_ctrl->SQUARES[0 * 8 + 0] = 0x6;
	vga_ctrl->SQUARES[0 * 8 + 1] = 0x4;
	vga_ctrl->SQUARES[0 * 8 + 2] = 0x2;
	vga_ctrl->SQUARES[0 * 8 + 3] = 0x8;
	vga_ctrl->SQUARES[0 * 8 + 4] = 0xa;
	vga_ctrl->SQUARES[0 * 8 + 5] = 0x2;
	vga_ctrl->SQUARES[0 * 8 + 6] = 0x4;
	vga_ctrl->SQUARES[0 * 8 + 7] = 0x6;

	/* Initialize row 7 */
    vga_ctrl->SQUARES[7 * 8 + 0] = 0x7;
	vga_ctrl->SQUARES[7 * 8 + 1] = 0x5;
	vga_ctrl->SQUARES[7 * 8 + 2] = 0x3;
	vga_ctrl->SQUARES[7 * 8 + 3] = 0x9;
	vga_ctrl->SQUARES[7 * 8 + 4] = 0xb;
	vga_ctrl->SQUARES[7 * 8 + 5] = 0x3;
	vga_ctrl->SQUARES[7 * 8 + 6] = 0x5;
	vga_ctrl->SQUARES[7 * 8 + 7] = 0x7;

	/* Intialize the FSM used for mouse click processing */
	st = START;
	player_turn = WHITE;

}

void processMouseClick(alt_u8 button)
{
	// printf("x_pos: %d, ", vga_ctrl->x_pos);
	// printf("y_pos: %d\n", vga_ctrl->y_pos);
	// printf("selectedX: %d, ", selectedX);
	// printf("selectedY: %d\n", selectedY);
	// printf("state: %d\n", st);

	// Reset Game
	if(button == 4)
	{
		initGame();
	}
	switch(st)
	{
		case DESELECT:
			if(button == NO_PRESS)
			{
				st = START;
				selectedX = NOT_SELECTED;
				selectedY = NOT_SELECTED;
				clearHighlight();
			}
			break;
		case START:
			if(button == LEFT_CLICK)
			{
				if ((vga_ctrl->x_pos >= 0 && vga_ctrl->x_pos < 480) && 
					(vga_ctrl->y_pos >= 0 && vga_ctrl->y_pos < 480)) 
				{
					selectedX = vga_ctrl->x_pos / 60;
					selectedY = vga_ctrl->y_pos / 60;
					if(checkMove(selectedY, selectedX) != 0)
					{
						st = INIT_POS_DOWN;
					}	
				}
			}
			break;
		case INIT_POS_DOWN:
			if(button == NO_PRESS)
			{
				st = INIT_POS_UP;
			}
			break;
		case INIT_POS_UP:
			if(button == LEFT_CLICK)
			{
				// check if the player intends to deselect the selected chess piece by clicking on it
				if((selectedX == vga_ctrl->x_pos / 60) && (selectedY == vga_ctrl->y_pos / 60))
				{
					st = DESELECT;
				}
				else if ((vga_ctrl->x_pos >= 0 && vga_ctrl->x_pos < 480) && 
					    (vga_ctrl->y_pos >= 0 && vga_ctrl->y_pos < 480)) 
				{	
					if(makeMove(selectedY, selectedX, vga_ctrl->y_pos / 60, vga_ctrl->x_pos / 60) != 0)
					{
						// Only update player turn if legal move is made.
						player_turn ^= 1;
					}
					st = FINAL_POS_DOWN;
						
				}
			}
			break;
		case FINAL_POS_DOWN:
			if(button == NO_PRESS)
			{
				st = FINAL_POS_UP;
			}
			break;
		case FINAL_POS_UP:
			st = START;
			selectedX = NOT_SELECTED;
			selectedY = NOT_SELECTED;
			clearHighlight();
			break;
		default:
			break;
	}
}

alt_u8 checkMove(alt_u8 initRow, alt_u8 initCol) 
{
	alt_u8 piece = vga_ctrl->SQUARES[initRow * 8 + initCol];
	alt_u8 color = piece & 1;
	alt_u8 pieceType = piece >> 1;
	

	/* Check empty square */
	if(piece == EMPTY_SQUARE)
	{
		return 0;
	}
	
	if (color != player_turn) {
		return 0;
	}

	// Variables used in switch
	int r, c;
	alt_u8 nPlaces = 0;
	switch (pieceType) {
		case PAWN:
			if (color == WHITE) {
				if (initRow != 0) {
					if (CHECK_SQUARE_EMPTY(initRow - 1, initCol)) {
						HILITE(initRow - 1, initCol);
						if (initRow == 6 && CHECK_SQUARE_EMPTY(initRow - 2, initCol))
						{
							HILITE(initRow - 2, initCol);
						}
					}

					if (initCol != 0 && CHECK_SQUARE_ENEMY(initRow - 1, initCol - 1, color)) {
						HILITE(initRow - 1, initCol - 1);
					}
					if (initCol != 7 && CHECK_SQUARE_ENEMY(initRow - 1, initCol + 1, color)) {
						HILITE(initRow - 1, initCol + 1);
					}
				}

			} else {
				if (initRow != 7) {
					if (CHECK_SQUARE_EMPTY(initRow + 1, initCol)) {
						HILITE(initRow + 1, initCol);
						if (initRow == 1 && CHECK_SQUARE_EMPTY(initRow + 2, initCol))
						{
							HILITE(initRow + 2, initCol);
						}
					}

					if (initCol != 0 && CHECK_SQUARE_ENEMY(initRow + 1, initCol - 1, color)) {
						HILITE(initRow + 1, initCol - 1);
					}
					if (initCol != 7 && CHECK_SQUARE_ENEMY(initRow + 1, initCol + 1, color)) {
						HILITE(initRow + 1, initCol + 1);
					}
				}
			}
			break;
		case KNIGHT:
			if (initRow >= 2) {
				if (initCol >= 1 && CHECK_SQUARE_EMPTY_OR_ENEMY(initRow - 2, initCol - 1, color)) {
					HILITE(initRow - 2, initCol - 1);
				}
				if (initCol <= 6 && CHECK_SQUARE_EMPTY_OR_ENEMY(initRow - 2, initCol + 1, color)) {
					HILITE(initRow - 2, initCol + 1);
				}
			}
			if (initRow >= 1) {
				if (initCol >= 2 && CHECK_SQUARE_EMPTY_OR_ENEMY(initRow - 1, initCol - 2, color)) {
					HILITE(initRow - 1, initCol - 2);
				}
				if (initCol <= 5 && CHECK_SQUARE_EMPTY_OR_ENEMY(initRow - 1, initCol + 2, color)) {
					HILITE(initRow - 1, initCol + 2);
				}
			}
			if (initRow <= 5) {
				if (initCol >= 1 && CHECK_SQUARE_EMPTY_OR_ENEMY(initRow + 2, initCol - 1, color)) {
					HILITE(initRow + 2, initCol - 1);
				}
				if (initCol <= 6 && CHECK_SQUARE_EMPTY_OR_ENEMY(initRow + 2, initCol + 1, color)) {
					HILITE(initRow + 2, initCol + 1);
				}
			}
			if (initRow <= 6) {
				if (initCol >= 2 && CHECK_SQUARE_EMPTY_OR_ENEMY(initRow + 1, initCol - 2, color)) {
					HILITE(initRow + 1, initCol - 2);
				}
				if (initCol <= 5 && CHECK_SQUARE_EMPTY_OR_ENEMY(initRow + 1, initCol + 2, color)) {
					HILITE(initRow + 1, initCol + 2);
				}
			}
			break;
		case BISHOP:
			r = initRow - 1;
			c = initCol - 1;
			while (r >= 0 && c >= 0) {
				if (CHECK_SQUARE_EMPTY(r, c)) {
					HILITE(r, c);
					r--;
					c--;
				}
				else if (CHECK_SQUARE_ENEMY(r ,c ,color)) {
					HILITE(r, c);
					break;
				}
				else break;
			}

			r = initRow + 1;
			c = initCol - 1;
			while (r < ROWS && c >= 0) {
				if (CHECK_SQUARE_EMPTY(r, c)) {
					HILITE(r, c);
					r++;
					c--;
				}
				else if (CHECK_SQUARE_ENEMY(r ,c ,color)) {
					HILITE(r, c);
					break;
				}
				else break;
			}

			r = initRow - 1;
			c = initCol + 1;
			while (r >= 0 && c < COLUMNS) {
				if (CHECK_SQUARE_EMPTY(r, c)) {
					HILITE(r, c);
					r--;
					c++;
				}
				else if (CHECK_SQUARE_ENEMY(r ,c ,color)) {
					HILITE(r, c);
					break;
				}
				else break;
			}

			r = initRow + 1;
			c = initCol + 1;
			while (r < ROWS && c < COLUMNS) {
				if (CHECK_SQUARE_EMPTY(r, c)) {
					HILITE(r, c);
					r++;
					c++;
				}
				else if (CHECK_SQUARE_ENEMY(r ,c ,color)) {
					HILITE(r, c);
					break;
				}
				else break;
			}
			break;
		case ROOK:
			r = initRow - 1;
			c = initCol;
			while (r >= 0) {
				if (CHECK_SQUARE_EMPTY(r, c)) {
					HILITE(r, c);
					r--;
				}
				else if (CHECK_SQUARE_ENEMY(r ,c ,color)) {
					HILITE(r, c);
					break;
				}
				else break;
			}

			r = initRow + 1;
			c = initCol;
			while (r < COLUMNS) {
				if (CHECK_SQUARE_EMPTY(r, c)) {
					HILITE(r, c);
					r++;
				}
				else if (CHECK_SQUARE_ENEMY(r ,c ,color)) {
					HILITE(r, c);
					break;
				}
				else break;
			}

			r = initRow;
			c = initCol - 1;
			while (c >= 0) {
				if (CHECK_SQUARE_EMPTY(r, c)) {
					HILITE(r,c);
					c--;
				}
				else if (CHECK_SQUARE_ENEMY(r ,c ,color)) {
					HILITE(r,c);
					break;
				}
				else break;
			}

			r = initRow;
			c = initCol + 1;
			while (c < COLUMNS) {
				if (CHECK_SQUARE_EMPTY(r, c)) {
					HILITE(r,c);
					c++;
				}
				else if (CHECK_SQUARE_ENEMY(r ,c ,color)) {
					HILITE(r,c);
					break;
				}
				else break;
			}
			break;
		case QUEEN:
			r = initRow - 1;
			c = initCol - 1;
			while (r >= 0 && c >= 0) {
				if (CHECK_SQUARE_EMPTY(r, c)) {
					HILITE(r,c);
					r--;
					c--;
				}
				else if (CHECK_SQUARE_ENEMY(r ,c ,color)) {
					HILITE(r,c);
					break;
				}
				else break;
			}

			r = initRow + 1;
			c = initCol - 1;
			while (r < ROWS && c >= 0) {
				if (CHECK_SQUARE_EMPTY(r, c)) {
					HILITE(r,c);
					r++;
					c--;
				}
				else if (CHECK_SQUARE_ENEMY(r ,c ,color)) {
					HILITE(r,c);
					break;
				}
				else break;
			}

			r = initRow - 1;
			c = initCol + 1;
			while (r >= 0 && c < COLUMNS) {
				if (CHECK_SQUARE_EMPTY(r, c)) {
					HILITE(r,c);
					r--;
					c++;
				}
				else if (CHECK_SQUARE_ENEMY(r ,c ,color)) {
					HILITE(r,c);
					break;
				}
				else break;
			}

			r = initRow + 1;
			c = initCol + 1;
			while (r < ROWS && c < COLUMNS) {
				if (CHECK_SQUARE_EMPTY(r, c)) {
					HILITE(r,c);
					r++;
					c++;
				}
				else if (CHECK_SQUARE_ENEMY(r ,c ,color)) {
					HILITE(r,c);
					break;
				}
				else break;
			}

			r = initRow - 1;
			c = initCol;
			while (r >= 0) {
				if (CHECK_SQUARE_EMPTY(r, c)) {
					HILITE(r,c);
					r--;
				}
				else if (CHECK_SQUARE_ENEMY(r ,c ,color)) {
					HILITE(r,c);
					break;
				}
				else break;
			}

			r = initRow + 1;
			c = initCol;
			while (r < COLUMNS) {
				if (CHECK_SQUARE_EMPTY(r, c)) {
					HILITE(r,c);
					r++;
				}
				else if (CHECK_SQUARE_ENEMY(r ,c ,color)) {
					HILITE(r,c);
					break;
				}
				else break;
			}

			r = initRow;
			c = initCol - 1;
			while (c >= 0) {
				if (CHECK_SQUARE_EMPTY(r, c)) {
					HILITE(r,c);
					c--;
				}
				else if (CHECK_SQUARE_ENEMY(r ,c ,color)) {
					HILITE(r,c);
					break;
				}
				else break;
			}

			r = initRow;
			c = initCol + 1;
			while (c < COLUMNS) {
				if (CHECK_SQUARE_EMPTY(r, c)) {
					HILITE(r,c);
					c++;
				}
				else if (CHECK_SQUARE_ENEMY(r ,c ,color)) {
					HILITE(r,c);
					break;
				}
				else break;
			}
			break;
		case KING:
			for (r = initRow - 1; r <= initRow + 1; r++) {
				for (c = initCol - 1; c <= initCol + 1; c++) {
					if (r >= 0 && r < ROWS && c >= 0 && c < COLUMNS && 
						(r != initRow || c != initCol) &&
						CHECK_SQUARE_EMPTY_OR_ENEMY(r, c, color)) {
						
						HILITE(r,c);
					}
				}
			}
			break;
		default:
			printf("Panic!\n");
			return 0;
	}

	if (nPlaces == 0)
		return 0;

	return 1;
}

alt_u8 makeMove(alt_u8 initRow, alt_u8 initCol, alt_u8 finalRow, alt_u8 finalCol)
{
	printf("makeMove called with parameters: (%d, %d) -> (%d, %d)\n", initRow, initCol, finalRow, finalCol);
	
	
	alt_u8 piece = vga_ctrl->SQUARES[initRow * 8 + initCol];
	/* Illegal Move */
	if((vga_ctrl->SQUARES[finalRow * 8 + finalCol] & 0x10) == 0)
	{
		return 0;
	}
	vga_ctrl->SQUARES[initRow * 8 + initCol] = EMPTY_SQUARE;
	vga_ctrl->SQUARES[finalRow * 8 + finalCol] = piece;
	usleep(1000); // adds delay to give time for the hardware to update graphics
	
	return 1;
}

/* Clear highlighting */
void clearHighlight()
{
	for(int i = 0; i < 64; i++)
	{
		vga_ctrl->SQUARES[i] &= 0xEF;
	}
	usleep(1000);
}

void simulateGame()
{

	while (1)
	{
		initGame();
		usleep (500000);
		// printf("Found piece at f2 with value: %d\n", vga_ctrl->SQUARES[6*8+5]);
		makeMove(6, 5, 5, 5);
		// printf("Moved piece to f3, piece had value: %d\n", vga_ctrl->SQUARES[5*8+5]);
		usleep (500000);
		// printf("Found piece at e7 with value: %d\n", vga_ctrl->SQUARES[1*8+4]);
		makeMove(1, 4, 3, 4);
		// printf("Moved piece to e5, piece had value: %d\n", vga_ctrl->SQUARES[3*8+4]);
		usleep (500000);
		// printf("Found piece at g2 with value: %d\n", vga_ctrl->SQUARES[6*8+6]);
		makeMove(6, 6, 4, 6);
		// printf("Moved piece to g4, piece had value: %d\n", vga_ctrl->SQUARES[4*8+6]);
		usleep (500000);
		// printf("Found piece at d8 with value: %d\n", vga_ctrl->SQUARES[0*8+3]);
		makeMove(0, 3, 4, 7);
		// printf("Moved piece to h4, piece had value: %d\n", vga_ctrl->SQUARES[4*8+7]);
		usleep (500000);
	}
}
