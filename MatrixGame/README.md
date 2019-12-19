- HARDWARE: 
     1. 1 - LCD: displays the MENU
     2. 2 - 8 by 8 LED Matrix: where the actual game takes place
     3. 2 - MAX2791 driver: allows for an easier control of the matrices
     4. 1 - Joystick: allows the user to control the menu or play the game
     5. 1 - Arduino Uno: the "brain" that runs it all
     
     https://ibb.co/6n7z05f (an image with the hardware)
   
 - HOW TO PLAY:
     Since TETRIS is one of the most iconic games of all time, the user should have no problem playing the game. The controls are intuitive:
       -> Use the Joystick to move the Tetromino block to the following directions: left, right(yAxis), and down(xAxis) when the player wants
     to set the block faster. Obviously you are not allowed to move up.
       -> Use the Joystick button to rotate the Tetromino block. This game uses the classical rotation schema of Tetris, but with a slight 
     twist, while in the classical Tetris you are not allowed to rotate while stuck to the walls, here you can do this, the game will
     automatically move your piece so as to not collide with anything, but only if it's possible to do so. Also while in the classic game
     the S and Z blocks have a tendency to stick to the right of the wall, in this one, they stick to the left.
       -> Just like classical TETRIS, after rows are cleared, blocks do not fall if there is nothing under them. They will remain in the        same position, only going down the number of times as there are complete rows below them, that get cleared. This makes the game 
     harder for the player, and frankly easier for the developer to program. It's a feature, not a bug.
  
  - THE MENU:
       The menu is also controlled with the Joystcik. To enter an exit each of the options the user must press the button. The menu
    contains the following options:
       1. Start Game: starts a new game, on the LCD will be displayed relevant info: number of lives, score, and difficulty level
       2. HIGHSCORE: contains a list with the first 3 highscore players
       3. Settings: allows the user to set it's name, and the difficulty level(starting level): 0 - NOVICE, 1 - NORMAL, 2 - HARD.
                     The difference between the difficulty is the following: each level starts with the same drop speed, that is
                     the speed at which blocks fall automatically one row, but in time this speed changes, and it changes according to 
                     the difficulty level: 17.5 seconds for level 0, 15 second for level 1, and 11 seconds for the hardest level. The                            amount with which the speed changes also varies with the level. 
       4. Info: contains information about the developer(name, GitHub link, and the name of the game)
       5. Exist: exists to the main menu screen
