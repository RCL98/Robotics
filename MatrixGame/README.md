- HARDWARE: 
     a. 1 - LCD: displayes the MENU
     b. 2 - 8 by 8 LED Matrix: where the actual game takes place
     d. 2 - MAX2791 driver: allows for an easier control of the matrices
     c. 1 - Joystick: allows the user to control the menu or play the game
     e. 1 - Arduino Uno: the "brain" that runs it all
     https://ibb.co/6n7z05f (an image with the hardware)
   
 - HOW TO PLAY:
     Since TETRIS is one of the most iconic games of all time, the user should have no problem playing the game. The controls are intuitive:
       - Use the Joystick to move the Tetromino block to the following directions: left, right(yAxis), and down(xAxis) when the player wants
     to set the block faster. Obviusly you are not allowed to move up.
       - Use the Joystick button to rotate the Tetromino block. This game uses the classical rotation schema of Tetris, but with a slight 
     twist, while in the classical Tetris you are not allowed to roate while stuck to the walls, here you can do this, the game will
     autoamtically move your piece so as to not colide with anything, but only if it's possible to do so. Also while in the classic game
     the S and Z blcoks have a tendecy to stick to the right of the wall, in this one, they stick to the left.
       - Just like classical TETRIS, after rows are cleared, blocks do not fall if there is nothing under them. They will remain in the same
     pozition, only going down the number of times as there are complete rows below them, that get cleared. This makes the game harder for
     the player, and frankly easier for the developer to program. It's a feature, not a bug.
  
  - THE MENU:
       The menu is also controlledwith the Joystcik. To enter an exit each of the options the user must press the button. The menu
    contains the folloing options:
       1. Start Game: starts a new game, on the LCD will be displayed relevant info: number of lives, score, and dificulty level
       2. HIGHSCORE: contains a list with the first 3 highscore players
       3. Settings: allows the user to set it's name, and the dificulty level(starting level): 0 - NOVICE, 1 - NORMAL, 2 - HARD
       4. Info: conatins information about the developer(name, github link, and the name of the game)
       5. Exist: exists to the main menu screen
