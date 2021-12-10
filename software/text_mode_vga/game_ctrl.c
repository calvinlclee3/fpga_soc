#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alt_types.h>
#include "game_ctrl.h"

#define NOT_SELECTED -1
#define CHECK_SQUARE_EMPTY(r,c) ((vga_ctrl->SQUARES[(r) * 8 + (c)] & 0x0F) == EMPTY_SQUARE)
#define CHECK_SQUARE_ENEMY(r,c,p) (((vga_ctrl->SQUARES[(r) * 8 + (c)] & 0x0F) != EMPTY_SQUARE) && ((vga_ctrl->SQUARES[(r) * 8 + (c)] & 0x01) != p))
#define CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player,piece) ((vga_ctrl->SQUARES[(r) * 8 + (c)] & 0x0F) == ((player ^ 1) + (piece << 1)))
#define CHECK_SQUARE_FRIENDLY_IS_KING(r,c,player) ((vga_ctrl->SQUARES[(r) * 8 + (c)] & 0x0F) == (player + (KING << 1)))
#define CHECK_SQUARE_FRIENDLY_IS_PIECE(r,c,player,piece) ((vga_ctrl->SQUARES[(r) * 8 + (c)] & 0x0F) == (player + (piece << 1)))
#define CHECK_SQUARE_EMPTY_OR_ENEMY(r,c,p) (CHECK_SQUARE_EMPTY(r,c) || CHECK_SQUARE_ENEMY(r,c,p))
#define HILITE(r,c) vga_ctrl->SQUARES[(r) * 8 + (c)] |= 0x10; nPlaces++;
#define HASH(r,c,piece) (64 * piece + r * 8 + c + 1)

static alt_u8 selectedX = NOT_SELECTED;
static alt_u8 selectedY = NOT_SELECTED;

static alt_u8 W_kingX;
static alt_u8 W_kingY;

static alt_u8 B_kingX;
static alt_u8 B_kingY;

static alt_u8 player_turn;

static alt_u8 castling_status;

typedef enum {

	DESELECT,
    START,
	INIT_POS_DOWN,
	INIT_POS_UP,
	FINAL_POS_DOWN,
	FINAL_POS_UP,
	END_GAME
	// Maybe we can have END_GAME_WHITE and END_GAME_BLACK seperately

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
	/* Game has not ended */
	vga_ctrl->end_game = 0;
	castling_status = NO_CASTLE;
	clearHighlight();
	W_kingX = 4;
	W_kingY = 7;
	B_kingX = 4;
	B_kingY = 0;
	in_check = 0;

	// Clear LEDs
	clearLED(0);
	clearLED(1);
	clearLED(2);
	clearLED(3);

}

void processMouseClick(alt_u8 button)
{
	// printf("x_pos: %d, ", vga_ctrl->x_pos);
	// printf("y_pos: %d\n", vga_ctrl->y_pos);
	// printf("selectedX: %d, ", selectedX);
	// printf("selectedY: %d\n", selectedY);
	// printf("state: %d\n", st);

	// Reset Game
	if(button == SCROLL)
	{
		initGame();
	}
	else
	{
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

							if (player_turn == BLACK) {
								in_check = inCheck(B_kingY, B_kingX);
							} else {
								in_check = inCheck(W_kingY, W_kingX);
							}
							printf("inCheck: %d\n", in_check);
							if(in_check != 0)
							{
								setLED(0);
								setLED(1);
								setLED(2);
								setLED(3);

							}
							else
							{
								clearLED(0);
								clearLED(1);
								clearLED(2);
								clearLED(3);
							}

							if (checkMate(in_check) != 0) {
								endGame(player_turn ^ 1);
								st = END_GAME;
								break;
							}
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
			
			case END_GAME:
				if(button == LEFT_CLICK)
				{
					if((vga_ctrl->x_pos >= 180 && vga_ctrl->x_pos < 300) && 
					(vga_ctrl->y_pos >= 240 && vga_ctrl->y_pos < 270))
					{
						initGame();
					}
				}
				break;
			default:
				break;
		}
	}
	
}

alt_u8 checkMove(alt_u8 initRow, alt_u8 initCol) 
{
	alt_u8 piece = vga_ctrl->SQUARES[initRow * 8 + initCol];
	alt_u8 color = piece & 1;
	alt_u8 pieceType = (piece >> 1) & 7;

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
			while (r < ROWS) {
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
						CHECK_SQUARE_EMPTY_OR_ENEMY(r, c, color) &&
						(inCheck(r,c) == 0)) {
						
						HILITE(r,c);
					}
				}
			}

			/* SPECIAL CASE: CASTLING */
			if(color == WHITE && initRow == 7 && initCol == 4)
			{
				if(vga_ctrl->SQUARES[56] == ((ROOK << 1) + WHITE) && CHECK_SQUARE_EMPTY(7, 1) && CHECK_SQUARE_EMPTY(7, 2) && CHECK_SQUARE_EMPTY(7, 3))
				{
					HILITE(initRow, initCol - 2);
					castling_status = WHITE_CASTLE_LEFT;
				}
				else if(vga_ctrl->SQUARES[63] == ((ROOK << 1) + WHITE) && CHECK_SQUARE_EMPTY(7, 5) && CHECK_SQUARE_EMPTY(7, 6))
				{
					HILITE(initRow, initCol + 2);
					castling_status = WHITE_CASTLE_RIGHT;
				}
			}
			else if(color == BLACK && initRow == 0 && initCol == 4)
			{
				if(vga_ctrl->SQUARES[0] == ((ROOK << 1) + BLACK) && CHECK_SQUARE_EMPTY(0, 1) && CHECK_SQUARE_EMPTY(0, 2) && CHECK_SQUARE_EMPTY(0, 3))
				{
					HILITE(initRow, initCol - 2);
					castling_status = BLACK_CASTLE_LEFT;
				}
				else if(vga_ctrl->SQUARES[7] == ((ROOK << 1) + BLACK) && CHECK_SQUARE_EMPTY(0, 5) && CHECK_SQUARE_EMPTY(0, 6))
				{
					HILITE(initRow, initCol + 2);
					castling_status = BLACK_CASTLE_RIGHT;
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
	// printf("makeMove called with parameters: (%d, %d) -> (%d, %d)\n", initRow, initCol, finalRow, finalCol);
	
	alt_u8 piece = vga_ctrl->SQUARES[initRow * 8 + initCol];
	alt_u8 color = piece & 1;
	alt_u8 pieceType = (piece >> 1) & 7;	
	
	// printf("color: %d, pieceType: %d \n", color, pieceType);
	// printf("Castling Status: %d: \n", castling_status);
	
	/* Illegal Move */
	if((vga_ctrl->SQUARES[finalRow * 8 + finalCol] & 0x10) == 0)
	{
		return 0;
	}

	/* SPECIAL CASE: PAWN PROMOTION */
	if(color == WHITE && pieceType == PAWN && finalRow == 0)
	{
		vga_ctrl->SQUARES[initRow * 8 + initCol] = EMPTY_SQUARE;
		vga_ctrl->SQUARES[finalRow * 8 + finalCol] = (QUEEN << 1) + color;
	}
	else if(color == BLACK && pieceType == PAWN && finalRow == 7)
	{
		vga_ctrl->SQUARES[initRow * 8 + initCol] = EMPTY_SQUARE;
		vga_ctrl->SQUARES[finalRow * 8 + finalCol] = (QUEEN << 1) + color;
	}
	/* SPECIAL CASE: CASTLING */
	else if(castling_status != NO_CASTLE && abs(initCol - finalCol) == 2)
	{
		if(castling_status == WHITE_CASTLE_LEFT)
		{
			vga_ctrl->SQUARES[7 * 8 + 3] = (ROOK << 1) + WHITE;
			vga_ctrl->SQUARES[7 * 8 + 0] = EMPTY_SQUARE;
		}
		else if(castling_status == WHITE_CASTLE_RIGHT)
		{
			vga_ctrl->SQUARES[7 * 8 + 5] = (ROOK << 1) + WHITE;
			vga_ctrl->SQUARES[7 * 8 + 7] = EMPTY_SQUARE;
		}
		else if(castling_status == BLACK_CASTLE_LEFT)
		{
			vga_ctrl->SQUARES[0 * 8 + 3] = (ROOK << 1) + BLACK;
			vga_ctrl->SQUARES[0 * 8 + 0] = EMPTY_SQUARE;
		}
		else if(castling_status == BLACK_CASTLE_RIGHT)
		{
			vga_ctrl->SQUARES[0 * 8 + 5] = (ROOK << 1) + BLACK;
			vga_ctrl->SQUARES[0 * 8 + 7] = EMPTY_SQUARE;
		}
		castling_status = NO_CASTLE;
		vga_ctrl->SQUARES[initRow * 8 + initCol] = EMPTY_SQUARE;
		vga_ctrl->SQUARES[finalRow * 8 + finalCol] = piece;
		if(player_turn == BLACK && pieceType == KING)
		{
			B_kingX = finalCol;
			B_kingY = finalRow;
		}
		else if (player_turn == WHITE && pieceType == KING)
		{
			W_kingX = finalCol;
			W_kingY = finalRow;
		}
	}
	else
	{
		vga_ctrl->SQUARES[initRow * 8 + initCol] = EMPTY_SQUARE;
		vga_ctrl->SQUARES[finalRow * 8 + finalCol] = piece;
		if(player_turn == BLACK && pieceType == KING)
		{
			B_kingX = finalCol;
			B_kingY = finalRow;
		}
		else if (player_turn == WHITE && pieceType == KING)
		{
			W_kingX = finalCol;
			W_kingY = finalRow;
		}
	}
	
	usleep(1000); // adds delay to give time for the hardware to update graphics
	return 1;
}

// Return value is hashed for checkMate function in the following way: 64 * (piece value) + row * 8 + col + 1
// 0 is the return value for not in check
alt_u16 inCheck (alt_u8 initRow, alt_u8 initCol)
{
	int r, c;

	// Knight moves logic
	if (initRow >= 2) {
		if (initCol >= 1 && CHECK_SQUARE_ENEMY_IS_PIECE(initRow - 2, initCol - 1, player_turn, KNIGHT)) {
			return HASH(initRow - 2, initCol - 1, KNIGHT);
		}
		if (initCol <= 6 && CHECK_SQUARE_ENEMY_IS_PIECE(initRow - 2, initCol + 1, player_turn, KNIGHT)) {
			return HASH(initRow - 2, initCol + 1, KNIGHT);
		}
	}
	if (initRow >= 1) {
		if (initCol >= 2 && CHECK_SQUARE_ENEMY_IS_PIECE(initRow - 1, initCol - 2, player_turn, KNIGHT)) {
			return HASH(initRow - 1, initCol - 2, KNIGHT);
		}
		if (initCol <= 5 && CHECK_SQUARE_ENEMY_IS_PIECE(initRow - 1, initCol + 2, player_turn, KNIGHT)) {
			return HASH(initRow - 1, initCol + 2, KNIGHT);
		}
	}
	if (initRow <= 5) {
		if (initCol >= 1 && CHECK_SQUARE_ENEMY_IS_PIECE(initRow + 2, initCol - 1, player_turn, KNIGHT)) {
			return HASH(initRow + 2, initCol - 1, KNIGHT);
		}
		if (initCol <= 6 && CHECK_SQUARE_ENEMY_IS_PIECE(initRow + 2, initCol + 1, player_turn, KNIGHT)) {
			return HASH(initRow + 2, initCol + 1, KNIGHT);
		}
	}
	if (initRow <= 6) {
		if (initCol >= 2 && CHECK_SQUARE_ENEMY_IS_PIECE(initRow + 1, initCol - 2, player_turn, KNIGHT)) {
			return HASH(initRow + 1, initCol - 2, KNIGHT);
		}
		if (initCol <= 5 && CHECK_SQUARE_ENEMY_IS_PIECE(initRow + 1, initCol + 2, player_turn, KNIGHT)) {
			return HASH(initRow + 1, initCol + 2, KNIGHT);
		}
	}

	// Diagonal move logic
	r = initRow - 1;
	c = initCol - 1;
	if (r >= 0 && c >= 0) {
		if (CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, KING)){
			return HASH(r, c, KING);
		}
		else if ((player_turn == WHITE) && CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, PAWN)) {
			return HASH(r, c, PAWN);
		}
	}
	while (r >= 0 && c >= 0) {
		if (CHECK_SQUARE_EMPTY(r, c) || CHECK_SQUARE_FRIENDLY_IS_KING(r, c, player_turn)) {
			r--;
			c--;
		}
		else if (CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, BISHOP)) {
			return HASH(r, c, BISHOP);
		}
		else if (CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, QUEEN)) {
			return HASH(r, c, QUEEN);
		}
		else break;
	}

	r = initRow + 1;
	c = initCol - 1;
	if (r < ROWS && c >= 0) {
		if (CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, KING)){
			return HASH(r, c, KING);
		}
		else if ((player_turn == BLACK) && CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, PAWN)) {
			return HASH(r, c, PAWN);
		}
	}
	while (r < ROWS && c >= 0) {
		if (CHECK_SQUARE_EMPTY(r, c) || CHECK_SQUARE_FRIENDLY_IS_KING(r, c, player_turn)) {
			r++;
			c--;
		}
		else if (CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, BISHOP)) {
			return HASH(r, c, BISHOP);
		}
		else if (CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, QUEEN)) {
			return HASH(r, c, QUEEN);
		}
		else break;
	}

	r = initRow - 1;
	c = initCol + 1;
	if (r >= 0 && c < COLUMNS) {
		if (CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, KING)){
			return HASH(r, c, KING);
		}
		else if ((player_turn == WHITE) && CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, PAWN)) {
			return HASH(r, c, PAWN);
		}
	}
	while (r >= 0 && c < COLUMNS) {
		if (CHECK_SQUARE_EMPTY(r, c) || CHECK_SQUARE_FRIENDLY_IS_KING(r, c, player_turn)) {
			r--;
			c++;
		}
		else if (CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, BISHOP)) {
			return HASH(r, c, BISHOP);
		}
		else if (CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, QUEEN)) {
			return HASH(r, c, QUEEN);
		}
		else break;
	}

	r = initRow + 1;
	c = initCol + 1;
	if (r < ROWS && c < COLUMNS) {
		if (CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, KING)){
			return HASH(r, c, KING);
		}
		else if ((player_turn == BLACK) && CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, PAWN)) {
			return HASH(r, c, PAWN);
		}
	}
	while (r < ROWS && c < COLUMNS) {
		if (CHECK_SQUARE_EMPTY(r, c) || CHECK_SQUARE_FRIENDLY_IS_KING(r, c, player_turn)) {
			r++;
			c++;
		}
		else if (CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, BISHOP)) {
			return HASH(r, c, BISHOP);
		}
		else if (CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, QUEEN)) {
			return HASH(r, c, QUEEN);
		}
		else break;
	}

	// Rank move logic
	
	r = initRow - 1;
	c = initCol;
	if (r >= 0 && CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, KING)) {
		return HASH(r, c, KING);
	}
	while (r >= 0) {
		if (CHECK_SQUARE_EMPTY(r, c) || CHECK_SQUARE_FRIENDLY_IS_KING(r, c, player_turn)) {
			r--;
		}
		else if (CHECK_SQUARE_ENEMY_IS_PIECE(r ,c , player_turn, ROOK)) {
			return HASH(r, c, ROOK);
		}
		else if (CHECK_SQUARE_ENEMY_IS_PIECE(r ,c , player_turn, QUEEN)) {
			return HASH(r, c, QUEEN);
		}
		else break;
	}

	r = initRow + 1;
	c = initCol;
	if (r < ROWS && CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, KING)) {
		return HASH(r, c, KING);
	}
	while (r < ROWS) {
		if (CHECK_SQUARE_EMPTY(r, c) || CHECK_SQUARE_FRIENDLY_IS_KING(r, c, player_turn)) {
			r++;
		}
		else if (CHECK_SQUARE_ENEMY_IS_PIECE(r ,c , player_turn, ROOK)) {
			return HASH(r, c, ROOK);
		}
		else if (CHECK_SQUARE_ENEMY_IS_PIECE(r ,c , player_turn, QUEEN)) {
			return HASH(r, c, QUEEN);
		}
		else break;
	}

	r = initRow;
	c = initCol - 1;
	if (c >= 0 && CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, KING)) {
		return HASH(r, c, KING);
	}
	while (c >= 0) {
		if (CHECK_SQUARE_EMPTY(r, c) || CHECK_SQUARE_FRIENDLY_IS_KING(r, c, player_turn)) {
			c--;
		}
		else if (CHECK_SQUARE_ENEMY_IS_PIECE(r ,c , player_turn, ROOK)) {
			return HASH(r, c, ROOK);
		}
		else if (CHECK_SQUARE_ENEMY_IS_PIECE(r ,c , player_turn, QUEEN)) {
			return HASH(r, c, QUEEN);
		}
		else break;
	}

	r = initRow;
	c = initCol + 1;
	if (c < COLUMNS && CHECK_SQUARE_ENEMY_IS_PIECE(r,c,player_turn, KING)) {
		return HASH(r, c, KING);
	}
	while (c < COLUMNS) {
		if (CHECK_SQUARE_EMPTY(r, c) || CHECK_SQUARE_FRIENDLY_IS_KING(r, c, player_turn)) {
			c++;
		}
		else if (CHECK_SQUARE_ENEMY_IS_PIECE(r ,c , player_turn, ROOK)) {
			return HASH(r, c, ROOK);
		}
		else if (CHECK_SQUARE_ENEMY_IS_PIECE(r ,c , player_turn, QUEEN)) {
			return HASH(r, c, QUEEN);
		}
		else break;
	}

	// Not in Check
	return 0;
}

/* return the following:
	0 for nothing
	1 for checkmate
	this function assumes that in_check is established to be true
*/
alt_u8 checkMate(alt_u16 in_check_hash)
{
	if (in_check == 0) {
		return 0;
	}

	alt_u16 adjusted_hash = in_check_hash - 1;
	// Enemy piece
	alt_u8 piece = (alt_u8) (adjusted_hash >> 6);
	printf("Piece value: %d\n", piece);
	// Enemy square
	alt_u8 row1 = (alt_u8) ((adjusted_hash & 0x28) >> 3);
	printf("Enemy at row: %d\n", row1);
	alt_u8 col1 = (alt_u8) (adjusted_hash & 0x07);
	printf("Enemy at col: %d\n", col1);
	// Friendly square
	alt_u8 row2;
	alt_u8 col2;

	if (player_turn == WHITE) {
		row2 = W_kingY;
		col2 = W_kingX;
	}
	else {
		row2 = B_kingY;
		col2 = B_kingX;
	}

	int dirRow, dirCol;

	if (row1 > row2) {
		dirRow = -1;
	} else if (row1 < row2) {
		dirRow = 1;
	} else {
		dirRow = 0;
	}

	if (col1 > col2) {
		dirCol = -1;
	} else if (col1 < col2) {
		dirCol = 1;
	} else {
		dirCol = 0;
	}

	int r = row1;
	int c = col1;
	int ri, ci;

	switch (piece) {
		case BISHOP:
		case ROOK:
		case QUEEN:
			while (r != row2 && c != col2) {
				printf("r: %d, c: %d, player_turn: %d\n", r, c, player_turn);

				// Possible pawn block?
				if (r == row1 && c == col1) {
					if (player_turn == WHITE) {
						if ((c > 0 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 1, c - 1, player_turn, PAWN)) || (c <= 6 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 1, c + 1, player_turn, PAWN))) {
							return 0;
						}
					} else {
						if ((c > 0 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 1, c - 1, player_turn, PAWN)) || (c <= 6 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 1, c + 1, player_turn, PAWN))) {
							return 0;
						}
					}
				}
				else {
					if (player_turn == WHITE) {
						if (r == 4) {
							if (CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 1, c, player_turn, PAWN) || CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 2, c, player_turn, PAWN)) {
								return 0;
							}
						} else {
							if (CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 1, c, player_turn, PAWN)) {
								return 0;
							}
						}
					} else {
						if (r == 5) {
							if (CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 1, c, player_turn, PAWN) || CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 2, c, player_turn, PAWN)) {
								return 0;
							}
						} else {
							if (CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 1, c, player_turn, PAWN)) {
								return 0;
							}
						}
					}
				}

				// Possible knight block?
				if (r >= 2) {
					if (c >= 1 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 2, c - 1, player_turn, KNIGHT)) {
						return 0;
					}
					if (c <= 6 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 2, c + 1, player_turn, KNIGHT)) {
						return 0;
					}
				}
				if (r >= 1) {
					if (c >= 2 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 1, c - 2, player_turn, KNIGHT)) {
						return 0;
					}
					if (c <= 5 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 1, c + 2, player_turn, KNIGHT)) {
						return 0;
					}
				}
				if (r <= 5) {
					if (c >= 1 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 2, c - 1, player_turn, KNIGHT)) {
						return 0;
					}
					if (c <= 6 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 2, c + 1, player_turn, KNIGHT)) {
						return 0;
					}
				}
				if (r <= 6) {
					if (c >= 2 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 1, c - 2, player_turn, KNIGHT)) {
						return 0;
					}
					if (c <= 5 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 1, c + 2, player_turn, KNIGHT)) {
						return 0;
					}
				}

				// Diagonal block?
				ri = r - 1;
				ci = c - 1;
				while (ri >= 0 && ci >= 0) {
					if (CHECK_SQUARE_EMPTY(ri, ci)) {
						ri--;
						ci--;
					}
					else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri,ci,player_turn, BISHOP)) {
						return 0;
					}
					else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri,ci,player_turn, QUEEN)) {
						return 0;
					}
					else break;
				}

				ri = r + 1;
				ci = c - 1;
				while (ri < ROWS && ci >= 0) {
					if (CHECK_SQUARE_EMPTY(ri, ci)) {
						ri++;
						ci--;
					}
					else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri,ci,player_turn, BISHOP)) {
						return 0;
					}
					else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri,ci,player_turn, QUEEN)) {
						return 0;
					}
					else break;
				}

				ri = r - 1;
				ci = c + 1;
				while (ri >= 0 && ci < COLUMNS) {
					if (CHECK_SQUARE_EMPTY(ri, ci)) {
						ri--;
						ci++;
					}
					else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri,ci,player_turn, BISHOP)) {
						return 0;
					}
					else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri,ci,player_turn, QUEEN)) {
						return 0;
					}
					else break;
				}

				ri = r + 1;
				ci = c + 1;
				while (ri < ROWS && ci < COLUMNS) {
					if (CHECK_SQUARE_EMPTY(ri, ci)) {
						ri++;
						ci++;
					}
					else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri,ci,player_turn, BISHOP)) {
						return 0;
					}
					else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri,ci,player_turn, QUEEN)) {
						return 0;
					}
					else break;
				}

				// Rank and file logic
				ri = r - 1;
				ci = c;
				while (ri >= 0) {
					if (CHECK_SQUARE_EMPTY(ri, ci)) {
						ri--;
					}
					else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri ,ci , player_turn, ROOK)) {
						return 0;
					}
					else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri ,ci , player_turn, QUEEN)) {
						return 0;
					}
					else break;
				}

				ri = r + 1;
				ci = c;
				while (ri < ROWS) {
					if (CHECK_SQUARE_EMPTY(ri, ci)) {
						ri++;
					}
					else if (CHECK_SQUARE_ENEMY_IS_PIECE(ri ,ci , player_turn, ROOK)) {
						return 0;
					}
					else if (CHECK_SQUARE_ENEMY_IS_PIECE(ri ,ci , player_turn, QUEEN)) {
						return 0;
					}
					else break;
				}

				ri = r;
				ci = c - 1;
				while (ci >= 0) {
					if (CHECK_SQUARE_EMPTY(ri, ci)) {
						ci--;
					}
					else if (CHECK_SQUARE_ENEMY_IS_PIECE(ri ,ci , player_turn, ROOK)) {
						return 0;
					}
					else if (CHECK_SQUARE_ENEMY_IS_PIECE(ri ,ci , player_turn, QUEEN)) {
						return 0;
					}
					else break;
				}

				ri = r;
				ci = c + 1;
				while (ci < COLUMNS) {
					if (CHECK_SQUARE_EMPTY(ri, ci)) {
						ci++;
					}
					else if (CHECK_SQUARE_ENEMY_IS_PIECE(ri ,ci , player_turn, ROOK)) {
						return 0;
					}
					else if (CHECK_SQUARE_ENEMY_IS_PIECE(ri ,ci , player_turn, QUEEN)) {
						return 0;
					}
					else break;
				}

				r += dirRow;
				c += dirCol;
			}
			break;
		case PAWN:
		case KNIGHT:
			if (r == row1 && c == col1) {
				if (player_turn == WHITE) {
					if ((c > 0 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 1, c - 1, player_turn, PAWN)) || (c <= 6 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 1, c + 1, player_turn, PAWN))) {
						return 0;
					}
				} else {
					if ((c > 0 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 1, c - 1, player_turn, PAWN)) || (c <= 6 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 1, c + 1, player_turn, PAWN))) {
						return 0;
					}
				}
			}
			else {
				if (player_turn == WHITE) {
					if (r == 4) {
						if (CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 1, c, player_turn, PAWN) || CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 2, c, player_turn, PAWN)) {
							return 0;
						}
					} else {
						if (CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 1, c, player_turn, PAWN)) {
							return 0;
						}
					}
				} else {
					if (r == 5) {
						if (CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 1, c, player_turn, PAWN) || CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 2, c, player_turn, PAWN)) {
							return 0;
						}
					} else {
						if (CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 1, c, player_turn, PAWN)) {
							return 0;
						}
					}
				}
			}

			// Possible knight block?
			if (r >= 2) {
				if (c >= 1 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 2, c - 1, player_turn, KNIGHT)) {
					return 0;
				}
				if (c <= 6 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 2, c + 1, player_turn, KNIGHT)) {
					return 0;
				}
			}
			if (r >= 1) {
				if (c >= 2 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 1, c - 2, player_turn, KNIGHT)) {
					return 0;
				}
				if (c <= 5 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r - 1, c + 2, player_turn, KNIGHT)) {
					return 0;
				}
			}
			if (r <= 5) {
				if (c >= 1 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 2, c - 1, player_turn, KNIGHT)) {
					return 0;
				}
				if (c <= 6 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 2, c + 1, player_turn, KNIGHT)) {
					return 0;
				}
			}
			if (r <= 6) {
				if (c >= 2 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 1, c - 2, player_turn, KNIGHT)) {
					return 0;
				}
				if (c <= 5 && CHECK_SQUARE_FRIENDLY_IS_PIECE(r + 1, c + 2, player_turn, KNIGHT)) {
					return 0;
				}
			}

			// Diagonal block?
			ri = r - 1;
			ci = c - 1;
			while (ri >= 0 && ci >= 0) {
				if (CHECK_SQUARE_EMPTY(ri, ci)) {
					ri--;
					ci--;
				}
				else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri,ci,player_turn, BISHOP)) {
					return 0;
				}
				else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri,ci,player_turn, QUEEN)) {
					return 0;
				}
				else break;
			}

			ri = r + 1;
			ci = c - 1;
			while (ri < ROWS && ci >= 0) {
				if (CHECK_SQUARE_EMPTY(ri, ci)) {
					ri++;
					ci--;
				}
				else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri,ci,player_turn, BISHOP)) {
					return 0;
				}
				else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri,ci,player_turn, QUEEN)) {
					return 0;
				}
				else break;
			}

			ri = r - 1;
			ci = c + 1;
			while (ri >= 0 && ci < COLUMNS) {
				if (CHECK_SQUARE_EMPTY(ri, ci)) {
					ri--;
					ci++;
				}
				else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri,ci,player_turn, BISHOP)) {
					return 0;
				}
				else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri,ci,player_turn, QUEEN)) {
					return 0;
				}
				else break;
			}

			ri = r + 1;
			ci = c + 1;
			while (ri < ROWS && ci < COLUMNS) {
				if (CHECK_SQUARE_EMPTY(ri, ci)) {
					ri++;
					ci++;
				}
				else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri,ci,player_turn, BISHOP)) {
					return 0;
				}
				else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri,ci,player_turn, QUEEN)) {
					return 0;
				}
				else break;
			}

			// Rank and file logic
			ri = r - 1;
			ci = c;
			while (ri >= 0) {
				if (CHECK_SQUARE_EMPTY(ri, ci)) {
					ri--;
				}
				else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri ,ci , player_turn, ROOK)) {
					return 0;
				}
				else if (CHECK_SQUARE_FRIENDLY_IS_PIECE(ri ,ci , player_turn, QUEEN)) {
					return 0;
				}
				else break;
			}

			ri = r + 1;
			ci = c;
			while (ri < ROWS) {
				if (CHECK_SQUARE_EMPTY(ri, ci)) {
					ri++;
				}
				else if (CHECK_SQUARE_ENEMY_IS_PIECE(ri ,ci , player_turn, ROOK)) {
					return 0;
				}
				else if (CHECK_SQUARE_ENEMY_IS_PIECE(ri ,ci , player_turn, QUEEN)) {
					return 0;
				}
				else break;
			}

			ri = r;
			ci = c - 1;
			while (ci >= 0) {
				if (CHECK_SQUARE_EMPTY(ri, ci)) {
					ci--;
				}
				else if (CHECK_SQUARE_ENEMY_IS_PIECE(ri ,ci , player_turn, ROOK)) {
					return 0;
				}
				else if (CHECK_SQUARE_ENEMY_IS_PIECE(ri ,ci , player_turn, QUEEN)) {
					return 0;
				}
				else break;
			}

			r += dirRow;
			c += dirCol;
			break;
		case KING:
		default:
			break;
	}

	for (ri = row2 - 1; ri <= row2 + 1; ri++) {
		for (ci = col2 - 1; ci <= col2 + 1; ci++) {
			if (ri >= 0 && ri < ROWS && ci >= 0 && ci < COLUMNS && 
				(ri != row2 || ci != col2) &&
				CHECK_SQUARE_EMPTY_OR_ENEMY(ri, ci, player_turn) &&
				(inCheck(ri,ci) == 0)) {
				
				return 0;
			}
		}
	}

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

void endGame(alt_u8 color)
{
	if(color == BLACK)
	{
		vga_ctrl->end_game = 1;
	}
	else if(color == WHITE)
	{
		vga_ctrl->end_game = 2;
	}
	else
	{
		printf("Error at endGame()!");
	}
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
