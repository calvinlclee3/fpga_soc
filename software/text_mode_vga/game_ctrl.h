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

};

static volatile struct BOARD* vga_ctrl = VGA_TEXT_MODE_CONTROLLER_0_BASE;

/* function prototypes */
void initGame();
alt_u8 makeMove(alt_u8 initRow, alt_u8 initCol, alt_u8 finalRow, alt_u8 finalCol);
alt_u8 checkMove(alt_u8 initRow, alt_u8 initCol);
void simulateGame();

void processMouseClick(alt_u8 button);

void clearHighlight();
#endif 
