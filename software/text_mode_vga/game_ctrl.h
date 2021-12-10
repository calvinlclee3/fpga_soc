#ifndef TEXT_MODE_VGA_COLOR_H_
#define TEXT_MODE_VGA_COLOR_H_

#define COLUMNS 8
#define ROWS 8

#define PAWN 0
#define BISHOP 1
#define KNIGHT 2
#define ROOK 3
#define QUEEN 4
#define KING 5
#define EMPTY_SQUARE 12

#define NO_PRESS 0
#define LEFT_CLICK 1
#define RIGHT_CLICK 2
#define SCROLL 4

#define BLACK 0
#define WHITE 1

#define NO_CASTLE 0
#define WHITE_CASTLE_LEFT 1
#define WHITE_CASTLE_RIGHT 2
#define BLACK_CASTLE_LEFT 3
#define BLACK_CASTLE_RIGHT 4

#include <system.h>
#include <alt_types.h>

struct BOARD {

	/* chess board */
	alt_u8 SQUARES [ROWS*COLUMNS];
	
	/* USB mouse data */
	alt_u8 x_offset;
	alt_u8 y_offset;
	alt_u8 button;
	alt_u8 reserved;

	/* absolute cursor coordinates */
	alt_u16 x_pos;
	alt_u16 y_pos;

	/* 
	 * flag to determine if end game screen should be displayed 
	 * 0 if game has not ended
	 * 1 if black won
	 * 2 if white won
	 */
	alt_u8 end_game;
};

static volatile struct BOARD* vga_ctrl = VGA_TEXT_MODE_CONTROLLER_0_BASE;
static alt_u16 in_check;


/* function prototypes */
void initGame();
alt_u8 makeMove(alt_u8 initRow, alt_u8 initCol, alt_u8 finalRow, alt_u8 finalCol);
alt_u8 checkMove(alt_u8 initRow, alt_u8 initCol);
alt_u16 inCheck(alt_u8 initRow, alt_u8 initCol);
alt_u8 checkMate(alt_u16 in_check_hash);
void simulateGame();

void processMouseClick(alt_u8 button);

void clearHighlight();

/* Call this function with color MACRO defined above
   to display corresponding end game screen */
void endGame(alt_u8 color);
#endif 
