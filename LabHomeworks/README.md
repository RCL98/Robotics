
- analogRGBControl_ -> Controlling  an RGB led with 3 potentiometers.

- knockTonePush -> Use a passive buzzer to detect a knock. After detecting one, wait for some time, then play a melody with the active buzzer.
		 If the button is pushed the melody stops and the wainting for the knock starts again. 


- 7-4segWithJoystickControl -> controling a 4-7 segments diplay with a joystick. The button changes the state from idle, to choosing one of the 4 digits with the 
		       	     X-axis of the joystick(1 button press), choosing the number with the Y-axis(2 button presses), back to former state(another button press), 
			     and back to idle(keeping the button pressed for 2 seconds).
			     
- joystickLcdDisplay -> a game menu diplayed on an LCD screen and controlled by a joystick. It contains the main menu, with the following options: Start Game, High Score, Settings and Exit. The options can be cycled through with the joystick and selected by pressing the button. The Settings menu contains the options of choosing a name and a starting level. The HighScore displays the current highest score saved on the arduino board(more precisely in the EEPROM memeory), and Start Game option launches a new game session. This video is ment to show how it works: https://drive.google.com/open?id=1WLgg-U9rTVmnFm0leJb2YnHD9d7vR_9m			     
