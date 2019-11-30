#include <EEPROM.h>
#include <LiquidCrystal.h>

const int RS = 12;
const int enable = 11;
const int D4 = 5;
const int D5 = 4;
const int D6 = 3;
const int D7 = 2;

LiquidCrystal lcd(RS, enable, D4, D5, D6, D7);

const int pinX = A0;
const int pinY = A1;
const int pinSW = 10;

//joystick variables
int xValue = 0;
int yValue = 0;
int swValue = 1;
byte xState = 0;
byte yState = 0;
unsigned int minTreshold = 150;
unsigned int maxTreshold = 950;

//button debounce variables
byte buttonState = 1;
byte lastButtonState = 1;
unsigned long lastDebounce = 0;
unsigned int debounceDelay = 80;

//menu variables
int mainMenuStates = 6;
byte menuState = 0;
byte mainMenuState = 0;

//start game variables
unsigned int startingLevelValue = 0;
unsigned int level = 0;
unsigned int lives = 3;
unsigned int score = 0;
unsigned long startGameTime = 0;
unsigned long startLevelTime = 0;
unsigned int levelInterval = 5000;
unsigned int gameInterval = 10000;
byte finishedGamePageIndex = 0;

//settings menu variables
int settingsMenuStates = 3;
byte settingMenuState = 0;

//name variables
const unsigned int numberOfLetters = 26;
const char letters[numberOfLetters] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
     'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'}; 
unsigned int letterIndex = 0;
const unsigned int numberName = 3;
unsigned int nameLetters[numberName] = {0, 0, 0};
String Name;
byte setNameState = 0;
byte xNameState = 0;

byte setStartLevel = 0;
int maxLevel = 10;


//for saving on EEPROM
struct player {
  unsigned int playerScore;
  String playerName;
};

player currentPlayer;
player highScorePalyer;

//setting on which page is the given menu on
byte setMenuState(byte menu, byte stateDown, byte stateUp, int xValue) {
     if (xValue < minTreshold && !xState) {
         menu = stateDown;
         xState = 1;
     }
     else  if (xValue > maxTreshold && !xState) {
         menu = stateUp;
         xState = 1;
     }
     else if (xValue < maxTreshold && xValue > minTreshold && xState){
         xState = 0;
     }
     return menu;
}

//set the name of the player
void setName(int xVal, int yVal) {
     if (yVal > maxTreshold && !yState) {
         if (letterIndex == numberName - 1) {
            letterIndex = 0;
         }
         else {
            letterIndex++;
         }
         yState = 1;
     }
     else  if (yVal < minTreshold && !yState) {
         if (letterIndex == 0) {
            letterIndex = numberName - 1;
         }
         else {
            letterIndex--;
         }
         yState = 1;
     }
     else if (yVal < maxTreshold && yVal > minTreshold && yState){
         yState = 0;
     } 
     
     if (xVal < minTreshold && !xState) {
         if (nameLetters[letterIndex] == numberOfLetters - 1) {
            nameLetters[letterIndex] = 0;
         }
         else {
            nameLetters[letterIndex]++;
         }
         xState = 1;
     }
     else  if (xVal > maxTreshold && !xState) {
         if (nameLetters[letterIndex] == 0) {
            nameLetters[letterIndex] = numberOfLetters - 1;
         }
         else {
            nameLetters[letterIndex]--;
         }
         xState = 1;
     }
     else if (xVal < maxTreshold && xVal > minTreshold && xState){
         xState  = 0;
     }
     for(int i = 0; i < numberName; ++i) {
        Name[i] = letters[nameLetters[i]];
     }
}


//set the starting level
void setStartingLevel(int xVal) {
     if (xVal < minTreshold && !xState) {
         if (startingLevelValue == 0) {
            startingLevelValue = maxLevel; 
         }
         else {
            startingLevelValue--;
         }
         xState = 1;
     }
     else  if (xVal > maxTreshold && !xState) {
         if (startingLevelValue == maxLevel) {
            startingLevelValue = 0; 
         }
         else {
            startingLevelValue++;
         }
         xState = 1;
     }
     else if (xVal < maxTreshold && xVal > minTreshold && xState){
         xState = 0;
     }
}


void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  pinMode(pinSW, INPUT_PULLUP);
  Serial.begin(9600);
  Name.reserve(numberName);
  for(int i = 0; i < numberName; ++i) {
        Name.concat('A');
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  swValue = digitalRead(pinSW);
  xValue = analogRead(pinX);

  if (swValue != lastButtonState) {
     lastDebounce = millis();
  }

  if ((millis() - lastDebounce) > debounceDelay) {
      if (swValue != buttonState) {
         buttonState = swValue; 
      }
  }
  lastButtonState = swValue;

  
  if (menuState == 0) {
     lcd.setCursor(3,0);
     lcd.print("Main Menu");
     lcd.setCursor(1,1);
     lcd.print("Press to Enter");
     if (buttonState == 0) {
        menuState++;
        mainMenuState = 0;
        buttonState  = 1;
        lastButtonState = 1;
     }
  }
  else if (menuState == 1) {
     delay(50);
     switch (mainMenuState) {
        case 0:
          lcd.clear();
          lcd.setCursor(3,0);
          lcd.print(">Start Game");
          lcd.setCursor(4,1);
          lcd.print("HighScore");
          mainMenuState = setMenuState(mainMenuState, 1, mainMenuStates - 1, xValue);
          if (buttonState == 0) {
             menuState = 2;
             score = 0;
             startGameTime = millis();
             startLevelTime = startGameTime;
             level = startingLevelValue; 
          }
        break;
        case 1:
          lcd.clear();
          lcd.setCursor(3,0);
          lcd.print("Start Game");
          lcd.setCursor(3,1);
          lcd.print(">HighScore");
          mainMenuState = setMenuState(mainMenuState, 3, 0, xValue);
          if (buttonState == 0) {
             menuState = 3;
             buttonState  = 1;
             lastButtonState = 1;
          }
        break;
        case 2:
          lcd.clear();
          lcd.setCursor(3,0);
          lcd.print(">HighScore");
          lcd.setCursor(3,1);
          lcd.print("Settings");
          mainMenuState = setMenuState(mainMenuState, 3, 0, xValue);
          if (buttonState == 0) {
             menuState = 3;
             buttonState  = 1;
             lastButtonState = 1;
          }
        break;          
        case 3:
          lcd.clear();
          lcd.setCursor(3,0);
          lcd.print("HighScore");
          lcd.setCursor(3,1);
          lcd.print(">Settings");
          mainMenuState = setMenuState(mainMenuState, 5, 2, xValue);
          if (buttonState == 0) {
             menuState = 4;
             buttonState  = 1;
             lastButtonState = 1;
          }
        break;
        case 4:
          lcd.clear();
          lcd.setCursor(3,0);
          lcd.print(">Settings");
          lcd.setCursor(5,1);
          lcd.print("Exit");
          mainMenuState = setMenuState(mainMenuState, 5, 2, xValue);
          if (buttonState == 0) {
             menuState = 4;
             buttonState  = 1;
             lastButtonState = 1;  
          }
        break; 
        case 5:
          lcd.clear();
          lcd.setCursor(3,0);
          lcd.print("Settings");
          lcd.setCursor(4,1);
          lcd.print(">Exit");
          mainMenuState = setMenuState(mainMenuState, 0, 4, xValue);
          if (buttonState == 0) {
             menuState = 0;
             buttonState  = 1;
             lastButtonState = 1;
          }
        break;                        
        default:
          lcd.clear();
          lcd.setCursor(3,0);
          lcd.print("ERROR");
        break;     
     } 
  }
  else if (menuState == 2) {
     if ((millis() - startLevelTime) > levelInterval) {
        level++;
        score = score + 3 * level; 
        startLevelTime = millis();
     }
     if ((millis() - startGameTime) > gameInterval) {
         if (xValue < minTreshold && !xState) {
             if (finishedGamePageIndex == 1) {
               finishedGamePageIndex = 0;
             }
             else {
               finishedGamePageIndex = 1;
             }
             xState = 1;
         }
         else  if (xValue > maxTreshold && !xState) {
             if (finishedGamePageIndex == 0) {
               finishedGamePageIndex = 1;
             }
             else {
               finishedGamePageIndex = 0;
             }
             xState = 1;
         }
         else if (xValue < maxTreshold && xValue > minTreshold && xState){
             xState = 0;
         }
         delay(50);
         if (finishedGamePageIndex == 0) {
             EEPROM.get(0, highScorePalyer);
             if (highScorePalyer.playerScore < score) {
                currentPlayer.playerName = Name;
                currentPlayer.playerScore = score;
                EEPROM.put(0, currentPlayer);   
             }
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print("Congratiulations");
             lcd.setCursor(0,1);
             lcd.print("you finished the");
         }
         else {
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print("game. Press the ");
             lcd.setCursor(0,1);
             lcd.print("button to exit.");
             if (buttonState == 0) {
               menuState = 1;
               finishedGamePageIndex = 0;
               score = 0;
               buttonState  = 1;
               lastButtonState = 1;              
             } 
         }
     }
     else {
        delay(50);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Lives:");
        lcd.setCursor(6,0);
        lcd.print(lives);
        lcd.setCursor(8,0);
        lcd.print("Level:");
        lcd.setCursor(14,0);
        lcd.print(startingLevelValue);
        lcd.setCursor(4,1);
        lcd.print("Score:");
        lcd.setCursor(10,1);
        lcd.print(score);
     }
  }
  else if (menuState == 3) {
     delay(50);
     EEPROM.get(0, highScorePalyer);
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("HighScore:");
     lcd.setCursor(10,0);
     lcd.print(highScorePalyer.playerName);
     lcd.setCursor(14,0);
     lcd.print(highScorePalyer.playerScore);
     lcd.setCursor(0,1);
     lcd.print("Press to exit");
     if (buttonState == 0) {
        menuState = 1;
        buttonState  = 1;
        lastButtonState = 1;              
    } 
  }
  else if (menuState = 4) {
     delay(50);
     switch (settingMenuState) {
        case 0:
           lcd.clear();
           lcd.setCursor(0,0);
           lcd.print(">Enter name:");
           lcd.setCursor(12,0);
           lcd.print(Name);
           lcd.setCursor(0,1);
           lcd.print("Starting lvl:");
           lcd.setCursor(13,1);
           lcd.print(startingLevelValue);
           if (!setNameState && !setStartLevel) {
              settingMenuState = setMenuState(settingMenuState, 1, settingsMenuStates - 1, xValue); 
           }
           if (buttonState == 0 && !setNameState) {
              setNameState = 1;
              buttonState  = 1;
              lastButtonState = 1;  
           }
        break;
        case 1:
           lcd.clear();
           lcd.setCursor(0,0);
           lcd.print("Enter name:");
           lcd.setCursor(11,0);
           lcd.print(Name);
           lcd.setCursor(0,1);
           lcd.print(">Starting lvl:");
           lcd.setCursor(14,1);
           lcd.print(startingLevelValue);
           if (!setNameState && !setStartLevel) {
              settingMenuState = setMenuState(settingMenuState, 2, 0, xValue);
           }
           if (buttonState == 0 && !setStartLevel) {
              setStartLevel = 1;
              buttonState  = 1;
              lastButtonState = 1; 
           }
        break;
        case 2:
           settingMenuState = setMenuState(settingMenuState, 0, 1, xValue);
           lcd.clear();
           lcd.setCursor(0,0);
           lcd.print("Starting lvl:");
           lcd.setCursor(13,0);
           lcd.print(startingLevelValue);
           lcd.setCursor(0,1);
           lcd.print(">Back");
           if (buttonState == 0) {
              menuState = 1;
              settingMenuState = 0;
              buttonState  = 1;
              lastButtonState = 1;  
           }
        break;
          
     }
     if (setNameState) {
        yValue = analogRead(pinY);
        setName(xValue, yValue); 
        if (buttonState == 0) {
              setNameState = 0;
              buttonState  = 1;
              lastButtonState = 1; 
       }       
     }
     if (setStartLevel) {
        setStartingLevel(xValue);
        if (buttonState == 0) {
              setStartLevel = 0;
              buttonState  = 1;
              lastButtonState = 1;
              settingMenuState = 1;   
       } 
     }
  }
  
}
