# PvP Chess on NIOS/II SOC

Our project is PvP chess running on NIOS/II SOC using MAX10 FPGA with VGA output.

## How to run our project
* Download all SystemVerilog files, SDC file, and the software folder (text_mode_vga) from Compass submission.
* Make sure the sprite data are loaded from the correct file path when using $readmemh.
* Use the same pin assignments from the course website (can be found in Lab 6 page).
* Attach the USB shield to MAX10 FPGA board.
* Attach USB Mouse to the USB host port on the shield.
* Compile, program FPGA, and run configuration in Eclipse.
* Use the mouse to move chess pieces. The highlighted squares show the legal moves for the selected piece.
* If you want to restart the game at any point, click down on the scroll wheel.

## Primary Files
* In addition to the hardware Systemverilog modules, the software are contained in main.c, game_ctrl.c, and game_ctrl.h.

### If additional files are needed, we have a git repo for the project. Please feel free to email lclee3@illinois.edu for access.