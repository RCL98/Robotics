#include "LedControl.h" 
#include <EEPROM.h>
#include "EEPROMAnything.h"
#include <LiquidCrystal.h>
LedControl lc = LedControl(12, 11, 10, 2); //DIN, CLK, LOAD, No. DRIVER
#include <MemoryFree.h>
 
// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12

const int V0_PIN = 3;
const int RS = 2 ;
const int enable = 8;
const int d4 = 7 ;
const int d5 = 6 ;
const int d6 = 5 ;
const int d7 = 4;
LiquidCrystal lcd(RS , enable , d4 , d5 , d6 , d7);

const int pinX = A0;
const int pinY = A1;
const int pinSW = A2;
int xValue = 0;
int yValue = 0;
int swValue = 0;
byte xState = 0;
byte yState = 0;
const int minTreshold = 150;
const int maxTreshold = 950;

int currentRow = 0;
int currentCol = 0;
int lastRow = 0;
int lastCol = 0;

byte buttonState = 1;
byte lastButtonState = 1;
unsigned long lastDebounce = 0;
unsigned int debounceDelay = 80;
byte buttonStateGame = 1;
byte lastButtonStateGame = 1;
unsigned long lastDebounceGame = 0;
unsigned int debounceDelayGame = 30;

int tetrisBlockState = 0;
int newTetrisBlockState = 0;

unsigned int maxDropInterval = 3000;
unsigned int dropInterval = 3000;
unsigned long lastDrop = 0;

unsigned int goDownInterval = 50;
unsigned long lastGoDown = 0;

unsigned int changeSpeedInterval = 15000;
unsigned long lastSpeedChange = 0;

const int matrixSize = 8;
const int tetrisBlockSize = 4;
const int borderSize = 2;
byte displayMatrix[2 * matrixSize + 2 * borderSize][matrixSize + 2 * borderSize] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1}, 
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1}, 
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1}, 
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1}, 
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1}, 
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, 
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}   
};

const byte numOfRot[7] = {4, 2, 2, 4, 4, 1, 2};

const byte tetrisBlockT[4][tetrisBlockSize] PROGMEM  = {
  {
    0b01110000,
    0b00100000,
    0b00000000,
    0b00000000,
  },{
    0b01000000,
    0b01100000,
    0b01000000,
    0b00000000
  },{
    0b00000000,
    0b00100000,
    0b01110000,
    0b00000000
  },{
    0b00010000,
    0b00110000,
    0b00010000,
    0b00000000
  }
};

const byte tetrisBlockZ[2][tetrisBlockSize] PROGMEM  = {
  {
    0b00110000,
    0b01100000,
    0b00000000,
    0b00000000,
  },{
    0b00100000,
    0b00110000,
    0b00010000,
    0b00000000
  }
};

const byte tetrisBlockS[2][tetrisBlockSize] PROGMEM  = {
  {
    0b01100000,
    0b00110000,
    0b00000000,
    0b00000000
  },{
    0b00010000,
    0b00110000,
    0b00100000,
    0b00000000
  }
};

const byte tetrisBlockJ[4][tetrisBlockSize] PROGMEM  = {
  {
    0b01110000,
    0b01000000,
    0b00000000,
    0b00000000
  },{
    0b00100000,
    0b00100000,
    0b00110000,
    0b00000000
  },{
    0b00010000,
    0b01110000,
    0b00000000,
    0b00000000
  },{
    0b01100000,
    0b00100000,
    0b00100000,
    0b00000000
  }
};

const byte tetrisBlockL[4][tetrisBlockSize] PROGMEM  = {
  {
    0b01110000,
    0b00010000,
    0b00000000,
    0b00000000
  },{
    0b00110000,
    0b00100000,
    0b00100000,
    0b00000000
  },{
    0b01000000,
    0b01110000,
    0b00000000,
    0b00000000
  },{
    0b00100000,
    0b00100000,
    0b01100000,
    0b00000000,
  }
};

const byte tetrisBlockO[1][tetrisBlockSize] PROGMEM  = {
  {
    0b00110000,
    0b00110000,
    0b00000000,
    0b00000000
  }
};

const byte tetrisBlockI[2][tetrisBlockSize] PROGMEM   = {
  {
    0b00000000,
    0b00000000,
    0b11110000,
    0b00000000
  },{
    0b00100000,
    0b00100000,
    0b00100000,
    0b00100000,
  }
};

byte currentCeiling = 14;  

const byte numberOfBloks = 7;
struct pointerToBlock{
   const byte (*p)[4];
};
byte blockIndex = 0;
pointerToBlock blocks[numberOfBloks];

unsigned int screenTime = 3000;


struct blockPoz {
  int U;
  int D;
  int L;
  int R;
};

blockPoz currentPoz;

const int letterWidth = 7;
const int numberOfLetters = 8;
int letterIndex = 0;
byte gameOver = 0;

unsigned long gameOverTime = 0;
unsigned int gameOverScreenDelay = 300;
const byte gameOverMatrix[letterWidth * numberOfLetters + 1] PROGMEM = {
    0b00000000,
    0b00011000,
    0b00100000,
    0b00100000,
    0b00101100,
    0b00100100,
    0b00011000,
    0b00000000,
    0b00011000,
    0b00100100,
    0b00100100,
    0b00111100,
    0b00100100,
    0b00100100,
    0b00000000,
    0b01000010,
    0b01100110,
    0b01011010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b00000000,
    0b00111100,
    0b00100000,
    0b00111000,
    0b00100000,
    0b00100000,
    0b00111100,
    0b00000000,
    0b00011000,
    0b00100100,
    0b00100100,
    0b00100100,
    0b00100100,
    0b00011000,
    0b00000000,
    0b01000010,
    0b01000010,
    0b01000010,
    0b01000010,
    0b00100100,
    0b00011000,
    0b00000000,
    0b00111100,
    0b00100000,
    0b00111000,
    0b00100000,
    0b00100000,
    0b00111100,
    0b00000000,
    0b00111000,
    0b00100100,
    0b00100100,
    0b00111000,
    0b00100100,
    0b00100100,
    0b00000000,
};

unsigned int startMenuTime = 0;

//menu variables
int mainMenuStates = 8;
byte menuState = 0;
byte mainMenuState = 0;

//start game variables
unsigned int startingLevelValue = 0;
unsigned long startGameTime = 0;
unsigned long gameOverTimeRegister = 0;
unsigned int level = 0;
unsigned int lives = 3;
unsigned long score = 0;
byte finishedGamePageIndex = 0;

//settings menu variables
int settingsMenuStates = 3;
byte settingMenuState = 0;

//name variables
const unsigned int numberOfLettersName = 26;
const char letters[numberOfLettersName] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
     'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'}; 
unsigned int letterIndexName = 0;
const unsigned int numberName = 3;
unsigned int nameLetters[numberName] = {0, 0, 0};
char Name[numberName + 1];
byte setNameState = 0;
byte xNameState = 0;

struct player {
  unsigned long playerScore;
  char playerName[numberName + 1];
};

player currentPlayer;
player highScorePlayer1;
player highScorePlayer2;
player highScorePlayer3;
const byte numberOfHighScorePlayers = 3;
byte highScorePageIndex = 0;
int playerSize = sizeof(player);
byte highScoreLevel = 0;

byte setStartLevel = 0;
int maxLevel = 2;

unsigned int infoIndex = 0;

void setName(int &xVal, int &yVal) {
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
         if (nameLetters[letterIndex] == numberOfLettersName - 1) {
            nameLetters[letterIndex] = 0;
         }
         else {
            nameLetters[letterIndex]++;
         }
         xState = 1;
     }
     else  if (xVal > maxTreshold && !xState) {
         if (nameLetters[letterIndex] == 0) {
            nameLetters[letterIndex] = numberOfLettersName - 1;
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

byte setMenuState(byte menu, const byte &stateDown, byte stateUp, int xValue) {
     if (xValue > maxTreshold && !xState) {
         menu = stateDown;
         xState = 1;
     }
     else  if (xValue < minTreshold && !xState) {
         menu = stateUp;
         xState = 1;
     }
     else if (xValue < maxTreshold && xValue > minTreshold && xState){
         xState = 0;
     }
     return menu;
}

void setStartingLevel(const int &xVal) {
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


void clearDisplayMatrix() {
  for (int row = borderSize; row < 2 * matrixSize + borderSize; row++) {
      for (int col = borderSize; col < matrixSize + borderSize; col++) {
          displayMatrix[row][col] = 0;
      }
  }
}

void fillDisplayMatrix() {
  for(int row = borderSize + 2 * matrixSize; row >= borderSize; row--) {
      //Serial.println("DA");
      for(int col = borderSize; col < borderSize + matrixSize; col++) {
         displayMatrix[row][col] = 1;
         delay(35);
         displayTheMatrix(row);
      }
  }
}

void displayTheMatrix(const byte &startRow) {
  for (int row = startRow; row < 2 * matrixSize + borderSize; row++) {
      for (int col = borderSize; col < matrixSize + borderSize; col++) {
        if(row < matrixSize + borderSize && currentPoz.U <= matrixSize + borderSize + 1) {
            lc.setLed(0, row - borderSize, col - borderSize, displayMatrix[row][col]);
        } else if (currentPoz.D >= matrixSize + borderSize) {
            lc.setLed(1, row - (matrixSize + borderSize), col - borderSize, displayMatrix[row][col]);
        }
      }
  }
}

byte detectColisionsYaxis(const int &newPoz, const byte tetrisBlock[][tetrisBlockSize], const int &blockState, blockPoz &currentPoz) {
   int newL, newR;
   newL = currentPoz.L + newPoz;
   newR = currentPoz.R + newPoz;
   for(int i = currentPoz.U; i <= currentPoz.D; ++i) {
       byte theTetrisBlock = pgm_read_byte_near(tetrisBlock[blockState] + (i - currentPoz.U));
       for(int j = newL; j <= newR; ++j) {
          if(displayMatrix[i][j] >= 1 && bitRead(theTetrisBlock, 7 - (j - newL)) == 1) {
              return 1;
          }
       }
   }
   return 0;
}

byte detectColisionsXaxis(const int &newPoz, const byte tetrisBlock[][tetrisBlockSize], const int blockState, blockPoz &currentPoz) {
   int newU, newD;
   newU = currentPoz.U + newPoz;
   newD = currentPoz.D + newPoz;
   for(int i = newU; i <= newD; ++i) {
       byte theTetrisBlock = pgm_read_byte_near(tetrisBlock[blockState] + (i - newU));
       for(int j = currentPoz.L; j <= currentPoz.R; ++j) {
          if(displayMatrix[i][j] >= 1 && bitRead(theTetrisBlock , 7 - (j - currentPoz.L)) == 1) {
              return 1;
          }
       }
   }
   return 0;
}

byte detectColisionsButton(const byte tetrisBlock[][tetrisBlockSize], const int &blockState, blockPoz &currentPoz) {
   for(int i = currentPoz.U; i <= currentPoz.D; ++i) {
       byte theTetrisBlock = pgm_read_byte_near(tetrisBlock[blockState] + (i - currentPoz.U));
       for(int j = currentPoz.L; j <= currentPoz.R; ++j) {
          if(displayMatrix[i][j] >= 1 && bitRead(theTetrisBlock, 7 - (j - currentPoz.L)) == 1) {
              if (i <= 1) {
                 return 3;
              } else if (j >= 10 || j - currentPoz.L >= tetrisBlockSize/2) {
                 return 1;
              } else if (j <= 1 || j - currentPoz.L < tetrisBlockSize/2) {
                 return 2;
              } else {
                 return -1;
              }
            
          }
       }
   }
   return 0;
}

void setInitialPozition(byte blockInd) {
   if (blockInd < 6) {
      currentPoz.U = borderSize;
      currentPoz.D = borderSize + tetrisBlockSize - 1;
      currentPoz.L = borderSize + 1;
      currentPoz.R = borderSize + tetrisBlockSize;
   } else {
      currentPoz.U = 0;
      currentPoz.D = tetrisBlockSize - 1;
      currentPoz.L = borderSize + 2;
      currentPoz.R = borderSize + tetrisBlockSize + 1;
   }
}

void generateNewBlock() {
   blockIndex = random(7);
   tetrisBlockState = 0;
   setInitialPozition(blockIndex);
}

void clearCompleteRows(byte &currentCeiling) {
    byte complete = 0;
    byte numberOfIncompoleteRows = 0;
    byte incompleteRows[matrixSize];
    byte completeRowExists = 0;
    int i, j;
    for(i = borderSize; i < 2 * matrixSize + borderSize && !complete; i++) {
        complete = 0;
        for(j = borderSize; j < matrixSize + borderSize; j++) {
            if (displayMatrix[i][j] == 1) {
                currentCeiling = i;
                complete = 1;
                break; 
            }
        } 
    }
    for(i = currentCeiling; i < 2 * matrixSize + borderSize; i++) {
        complete = 1;
        for(j = borderSize; j < matrixSize + borderSize; j++) {
            if (displayMatrix[i][j] == 0) {
                complete = 0;
                break;
            }
        }
        if (!complete) {
            incompleteRows[numberOfIncompoleteRows] = i;
            numberOfIncompoleteRows++;
        } else {
            completeRowExists = 1;
        }
    }
    if (completeRowExists) {
        int p =  numberOfIncompoleteRows - 1;
        for(i = 2 * matrixSize + borderSize - 1; i >= currentCeiling; --i) {
            for(j = borderSize; j < matrixSize + borderSize; j++) {
                if (p >= 0) {
                    displayMatrix[i][j] = displayMatrix[incompleteRows[p]][j];
                } else {
                    displayMatrix[i][j] = 0;
                }
            }
            p--;
        }
        if (level == 0) {
            score = score + 5 * (2 * matrixSize + borderSize - currentCeiling - numberOfIncompoleteRows) + 3 * (millis() - startGameTime)/1000;
        } else if (level == 1) {
            score = score + 7 * (2 * matrixSize + borderSize - currentCeiling - numberOfIncompoleteRows) + 5 * (millis() - startGameTime)/1000;
        } else {
            score = score + 10 * (2 * matrixSize + borderSize - currentCeiling - numberOfIncompoleteRows) + 7 * (millis() - startGameTime)/1000;
        }
    }
    currentCeiling = 2 * matrixSize + borderSize - numberOfIncompoleteRows;
}

void clearLastPoz(const byte tetrisBlock[][4], const int &blockState, blockPoz &lastPoz) {
    for(int i = lastPoz.U; i <= lastPoz.D; i++) {
       byte theTetrisBlock = pgm_read_byte_near(tetrisBlock[blockState] + (i - currentPoz.U));
       for(int j = lastPoz.L; j <= lastPoz.R; j++) {
         if (bitRead(theTetrisBlock, 7 - (j - currentPoz.L)) == 1) {
             displayMatrix[i][j] = 0;
         }
    }
  }   
}

void writeTetrisBlock(const byte tetrisBlock[][4],const int &blockState, blockPoz &currentPoz) {
  for(int i = currentPoz.U; i <= currentPoz.D; ++i) {
    byte theTetrisBlock = pgm_read_byte_near(tetrisBlock[blockState] + (i - currentPoz.U));
    for(int j = currentPoz.L; j <= currentPoz.R; ++j) {
       if (displayMatrix[i][j] == 0) {
           displayMatrix[i][j] = bitRead(theTetrisBlock, 7 - (j - currentPoz.L));
       }
    }
  }
}

void changePozition(const byte tetrisBlock[][4], const int &blockState, const int &xVal, const int &yVal) {
  clearLastPoz(tetrisBlock, blockState, currentPoz);
  if (yVal <= minTreshold && !yState){
      if (currentPoz.L > 0) {
         if(!detectColisionsYaxis(-1, tetrisBlock, blockState, currentPoz)) { 
            currentPoz.L = currentPoz.L - 1;
            currentPoz.R  = currentPoz.R - 1;          
         }
      }
      yState = 1;
  } else if (yVal >= maxTreshold && !yState){
      if (currentPoz.R < matrixSize + 2 * borderSize) {
         if(!detectColisionsYaxis(1, tetrisBlock, blockState, currentPoz)) {
            currentPoz.L = currentPoz.L + 1;
            currentPoz.R  = currentPoz.R + 1;          
         }
      }
      yState = 1;
  } else if (yVal < maxTreshold && yVal > minTreshold && yState) {
      yState = 0;
  }
  if (xVal >= maxTreshold && ((millis() - lastGoDown) > goDownInterval)) {
      if (currentPoz.D < 2 * matrixSize + 2 * borderSize) {
         if(!detectColisionsXaxis(1, tetrisBlock, blockState, currentPoz)) {
            currentPoz.U = currentPoz.U + 1;
            currentPoz.D  = currentPoz.D + 1;          
         } else {
            writeTetrisBlock(blocks[blockIndex].p, tetrisBlockState, currentPoz);
            if (currentCeiling > currentPoz.U) {
               if (blockIndex == 6 && tetrisBlockState == 0) {
                  currentCeiling = currentPoz.U + 2;
               } else {
                  currentCeiling = currentPoz.U; 
               }
            }
            int lastCeiling = currentCeiling;
            clearCompleteRows(currentCeiling);
            displayTheMatrix(lastCeiling);
            generateNewBlock();
            tetrisBlockState = 0;
            writeTetrisBlock(blocks[blockIndex].p, tetrisBlockState, currentPoz);
            if (currentCeiling == borderSize) {
                currentPoz.U = borderSize + 1;
                currentPoz.D = borderSize + matrixSize + 1;
                displayTheMatrix(borderSize);
                fillDisplayMatrix();
                delay(500);
                clearDisplayMatrix();
                displayTheMatrix(borderSize);
                lives--;
                if (lives == 0) {
                    gameOverTime = millis();
                    finishedGamePageIndex = 0;
                    gameOverTimeRegister = gameOverTime;
                    gameOver = 1;
                    xState = 1;
                    player highScorePlayers[numberOfHighScorePlayers];
                    for(int i = 0; i < numberOfHighScorePlayers; i++) {
                        if (i == 0) {
                           EEPROM_readAnything(0, highScorePlayers[i]);
                        } else {
                           EEPROM_readAnything(i * playerSize + 1, highScorePlayers[i]);
                        }
                        if (highScorePlayers[i].playerScore < score) {
                           strcpy(currentPlayer.playerName, Name);
                           currentPlayer.playerScore = score;
                           if (i == 0) {
                                EEPROM_writeAnything(0, currentPlayer);
                           } else {
                                EEPROM_writeAnything(i * playerSize + 1, currentPlayer); 
                           }
                           highScoreLevel = i + 1;
                           break;
                        }
                     }
                     for(int i = highScoreLevel; i < numberOfHighScorePlayers; i++) {
                         EEPROM_readAnything(i * playerSize + 1, highScorePlayers[i]);
                         EEPROM_writeAnything(i * playerSize + 1, highScorePlayers[i - 1]);
                     }
                } else {
                    generateNewBlock();
                    writeTetrisBlock(blocks[blockIndex].p, tetrisBlockState, currentPoz);
                    tetrisBlockState = 0;
                    currentCeiling = 2 * matrixSize + borderSize - tetrisBlockSize + 1;
                }
                
            }
         }
      }
      lastGoDown = millis();
  }
}

void changeBlockState(const byte &numOfRotations) {
    clearLastPoz(blocks[blockIndex].p, tetrisBlockState, currentPoz);
    blockPoz lastPoz = currentPoz;
    newTetrisBlockState = tetrisBlockState;
    if (tetrisBlockState == numOfRotations - 1) {
        newTetrisBlockState = 0; 
    } else {
        newTetrisBlockState++;
    }
    switch(detectColisionsButton(blocks[blockIndex].p, newTetrisBlockState, currentPoz)) {
        case 0:
             tetrisBlockState = newTetrisBlockState;
        break;
        case 1: 
             currentPoz.L -= 1;
             currentPoz.R -= 1;
        break;
        case 2:
             if (blockIndex < 6) {
                 currentPoz.L++;
                 currentPoz.R++;   
             } else {
                 currentPoz.L += 2;
                 currentPoz.R += 2; 
             }
       break;
       case 3: 
           if (blockIndex < 6) {
               currentPoz.U++;
               currentPoz.D++;   
           } else {
               currentPoz.U += 2;
               currentPoz.D += 2; 
           } 
       break;
   }
   if (tetrisBlockState != newTetrisBlockState) {
      if (detectColisionsButton(blocks[blockIndex].p, newTetrisBlockState, currentPoz) == 0) {
          tetrisBlockState = newTetrisBlockState;
      } else {
          currentPoz = lastPoz;
      }
   }
}

void setup()
{
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, 2); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
  lc.shutdown(1, false); // turn off power saving, enables display
  lc.setIntensity(1, 2); // sets brightness (0~15 possible values)
  lc.clearDisplay(1);
  lcd.begin (16, 2);
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(V0_PIN, OUTPUT);
  analogWrite(V0_PIN, 17);
  Serial.begin(9600); 
  blocks[0].p = tetrisBlockT;
  blocks[1].p = tetrisBlockS;
  blocks[2].p = tetrisBlockZ;
  blocks[3].p = tetrisBlockL;
  blocks[4].p = tetrisBlockJ;
  blocks[5].p = tetrisBlockO;
  blocks[6].p = tetrisBlockI;
  randomSeed(analogRead(A5));
  generateNewBlock();
  for(int i = 0; i < numberName; ++i) {
     Name[i] = 'A';
  }
  strcpy(currentPlayer.playerName, Name);
  currentPlayer.playerScore = 3;
  lastDrop = millis();
  EEPROM_writeAnything(0, currentPlayer);
  currentPlayer.playerScore = 2;
  EEPROM_writeAnything(playerSize + 1, currentPlayer);
  currentPlayer.playerScore = 1;
  EEPROM_writeAnything(2 * playerSize+ 1, currentPlayer);
  startMenuTime = millis();
}

void loop()
{
  swValue = digitalRead(pinSW);
  xValue = analogRead(pinX);

  if (swValue != lastButtonState) {
     lastDebounce = millis();
  }

  if ((millis() - startMenuTime) < screenTime) {
      lcd.setCursor(2, 0);
      lcd.print("WELLCOME TO");
      lcd.setCursor(4,1);
      lcd.print("TETRIS");
  } else {
      if ((millis() - lastDebounce) > debounceDelay) {
          if (swValue != buttonState) {
             buttonState = swValue; 
          }
      }
      lastButtonState = swValue;
      if (menuState == 0) {
         lcd.clear();
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
                 level = startingLevelValue;
                 gameOver = 0;
                 letterIndex = 0;
                 clearDisplayMatrix();
                 generateNewBlock();
                 tetrisBlockState = 0;
                 writeTetrisBlock(blocks[blockIndex].p, tetrisBlockState, currentPoz);
                 lives = 3;
                 if (level == 0) {
                     changeSpeedInterval = 17500;
                 } else if (level == 1) { 
                     changeSpeedInterval = 15000;
                 } else {
                     changeSpeedInterval = 12500;
                 }
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
                 highScorePageIndex = 0;
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
                 highScorePageIndex = 0;
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
              lcd.print("Info");
              mainMenuState = setMenuState(mainMenuState, 5, 2, xValue);
              if (buttonState == 0) {
                 menuState = 4;
                 buttonState  = 1;
                 lastButtonState = 1;  
              }
            break; 
            case 5:
              lcd.clear();
              lcd.setCursor(4,0);
              lcd.print("Settings");
              lcd.setCursor(5,1);
              lcd.print(">Info");
              mainMenuState = setMenuState(mainMenuState, mainMenuStates - 1, 4, xValue);
              if (buttonState == 0) {
                 menuState = 5;
                 infoIndex = 0;
                 buttonState  = 1;
                 lastButtonState = 1;
              }
            break;
            case 6:
              lcd.clear();
              lcd.setCursor(4,0);
              lcd.print(">Info");
              lcd.setCursor(5,1);
              lcd.print("EXIT");
              mainMenuState = setMenuState(mainMenuState, 7, 4, xValue);
              if (buttonState == 0) {
                 menuState = 5;
                 infoIndex = 0;
                 buttonState  = 1;
                 lastButtonState = 1;
              }
            break; 
            case 7:
               lcd.clear();
              lcd.setCursor(6,0);
              lcd.print("Info");
              lcd.setCursor(5,1);
              lcd.print(">EXIT");
              mainMenuState = setMenuState(mainMenuState, 0, 6, xValue);
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
          if (gameOver == 0) {
                xValue = analogRead(pinX);
                yValue = analogRead(pinY);
                swValue = digitalRead(pinSW);
                if ((millis() - lastSpeedChange) > changeSpeedInterval) {
                    if (dropInterval <= 3000 && dropInterval >= 1000) {
                       if (level == 0) { 
                          dropInterval -= 90;
                       } else if (level = 1) {
                          dropInterval -= 120;
                       } else {
                          dropInterval -= 150;
                       }
                    } else if (dropInterval <= 1000 && dropInterval >= 500) {
                       if (level == 0) { 
                          dropInterval -= 40;
                       } else if (level = 1) {
                          dropInterval -= 70;
                       } else {
                          dropInterval -= 100;
                       }
                    } else if (dropInterval <= 500 && dropInterval >= 300) {
                       if (level == 0) { 
                          dropInterval -= 10;
                       } else if (level = 1) {
                          dropInterval -= 30;
                       } else {
                          dropInterval -= 50;
                       }
                    }
                    lastSpeedChange = millis(); 
                }
                if ((millis() - lastDrop) > dropInterval) {
                    changePozition(blocks[blockIndex].p, tetrisBlockState, maxTreshold, yValue);
                    lastDrop = millis();
                }
                changePozition(blocks[blockIndex].p, tetrisBlockState, xValue, yValue);
                
                if (swValue != lastButtonStateGame) {
                   lastDebounceGame = millis();
                }
                if ((millis() - lastDebounceGame) > debounceDelayGame) {
                    if (swValue != buttonStateGame) {
                       buttonStateGame = swValue;
                       if (buttonStateGame == LOW && numOfRot[blockIndex] > 1) {
                          changeBlockState(numOfRot[blockIndex]);
                       }
                    }
                }
                lastButtonStateGame = swValue;
                if (gameOver != 1) {
                    writeTetrisBlock(blocks[blockIndex].p, tetrisBlockState, currentPoz);
                }
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Lives:");
                lcd.setCursor(6,0);
                lcd.print(lives);
                lcd.setCursor(8,0);
                lcd.print("Level:");
                lcd.setCursor(14,0);
                lcd.print(startingLevelValue);
                lcd.setCursor(0,1);
                lcd.print("Score:");
                lcd.setCursor(6,1);
                lcd.print(score);
          }
          if (gameOver == 1) {
              if ((millis() - gameOverTime) > gameOverScreenDelay) {
                  for (int row = 2 * matrixSize + borderSize - 1; 2 * matrixSize + borderSize - 1 - row <= letterIndex && row >= borderSize; row--) {
                      for (int col = borderSize; col < borderSize + matrixSize; col++) {
                           byte gameOverMatrixrow = pgm_read_byte_near(gameOverMatrix + (letterIndex - (2 * matrixSize + borderSize - 1 - row)) % (letterWidth * numberOfLetters));
                           displayMatrix[row][col] = bitRead(gameOverMatrixrow, col - borderSize);
                      }
                  }
                  letterIndex++;
                  gameOverTime = millis();
              }
              if (xValue > maxTreshold && !xState) {
                 if (finishedGamePageIndex == 2) {
                   finishedGamePageIndex = 0;
                 } else {
                   finishedGamePageIndex++;
                 }
                 xState = 1;
             } else  if (xValue < minTreshold && !xState) {
                 if (finishedGamePageIndex == 0) {
                   finishedGamePageIndex = 2;
                 } else {
                   finishedGamePageIndex--;
                 }
                 xState = 1;
             } else if (xValue < maxTreshold && xValue > minTreshold && xState){
                 xState = 0;
             }
              if (finishedGamePageIndex == 0) {
                   lcd.clear();
                   lcd.setCursor(0,0);
                   lcd.print("TIME:");
                   lcd.setCursor(6,0);
                   lcd.print(gameOverTimeRegister/1000);
                   lcd.setCursor(0,1);
                   lcd.print("SCORE:");
                   lcd.setCursor(7,1);
                   lcd.print(score);
               } else if (finishedGamePageIndex == 1) {
                   lcd.clear();
                   lcd.setCursor(0,0);
                   if (highScoreLevel == 0) {
                      lcd.print("No high score");
                   } else {
                      lcd.print("High Score: ");
                      lcd.setCursor(11,0);
                      lcd.print(highScoreLevel);
                   }
                   lcd.setCursor(0,1);
                   lcd.print("Press the button");
               } else {
                   lcd.clear();
                   lcd.setCursor(5,0);
                   lcd.print("to exit.");
                   if (buttonState == 0) {
                     menuState = 1;
                     finishedGamePageIndex = 0;
                     score = 0;
                     buttonState  = 1;
                     lastButtonState = 1;
                     clearDisplayMatrix();              
                   } 
               }
         }
         displayTheMatrix(borderSize);
      }
      else if (menuState == 3) {
         delay(50);
         if (xValue > maxTreshold && !xState) {
                 if (highScorePageIndex == 1) {
                   highScorePageIndex = 0;
                 } else {
                   highScorePageIndex = 1;
                 }
                 xState = 1;
         } else  if (xValue < minTreshold && !xState) {
                 if (highScorePageIndex == 0) {
                   highScorePageIndex = 1;
                 } else {
                   highScorePageIndex = 0;
                 }
                 xState = 1;
         } else if (xValue < maxTreshold && xValue > minTreshold && xState){
                 xState = 0;
         }
         if (highScorePageIndex == 0) {
               EEPROM_readAnything(0, highScorePlayer1);
               EEPROM_readAnything(playerSize + 1, highScorePlayer2);
               lcd.clear();
               lcd.setCursor(2,0);
               lcd.print(highScorePlayer1.playerName);
               lcd.setCursor(6,0);
               lcd.print(highScorePlayer1.playerScore);
               lcd.setCursor(2,1);
               lcd.print(highScorePlayer2.playerName);
               lcd.setCursor(6,1);
               lcd.print(highScorePlayer2.playerScore);
         } else {
               EEPROM_readAnything(2 * playerSize + 1, highScorePlayer3);
               lcd.clear();
               lcd.setCursor(2,0);
               lcd.print(highScorePlayer3.playerName);
               lcd.setCursor(6,0);
               lcd.print(highScorePlayer3.playerScore);
               lcd.setCursor(1,1);
               lcd.print("Press to exit.");
               if (buttonState == 0) {
                  menuState = 1;
                  buttonState  = 1;
                  lastButtonState = 1;              
              }
         }
        
      }
      else if (menuState == 4) {
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
                  letterIndex = 0;
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
                  startingLevelValue = 0;
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
      } else if (menuState == 5) {
          lcd.clear();
          if (xValue > maxTreshold && !xState) {
                 if (infoIndex == 2) {
                   infoIndex = 0;
                 } else {
                   infoIndex++;
                 }
                 xState = 1;
         } else  if (xValue < minTreshold && !xState) {
                 if (infoIndex == 0) {
                   infoIndex = 2;
                 } else {
                   infoIndex--;
                 }
                 xState = 1;
         } else if (xValue < maxTreshold && xValue > minTreshold && xState){
                 xState = 0;
         }
          if (infoIndex == 0) {
              lcd.setCursor(0,0);
              lcd.print("Creator:");
              lcd.setCursor(0,1);
              lcd.print("Ranete Cristian");
          } else if (infoIndex == 1) {
              lcd.setCursor(0,0);
              lcd.print("github.com/RCL98/");
              lcd.setCursor(5,1);
              lcd.print("Robotics");
          } else {
              lcd.setCursor(5,0);
              lcd.print("TETRIS");
              lcd.setCursor(0,1);
              lcd.print("Press to exit.");
               if (buttonState == 0) {
                  menuState = 1;
                  buttonState  = 1;
                  lastButtonState = 1;              
              }
          }
          
      }
  }
}
