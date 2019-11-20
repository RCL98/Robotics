//segments 
const int pinA = 2;
const int pinB = 3;
const int pinC = 4;
const int pinD = 5;
const int pinE = 6;
const int pinF = 7;
const int pinG = 8;
const int pinDP = 9;

//digitSegments
const int pinD1 = 10;
const int pinD2 = 11;
const int pinD3 = 12;
const int pinD4 = 13;

//joystick
const int pinX = A0;
const int pinY = A1;
const int pinSW = A2;

const int segSize = 8;
const int digitSize = 4;
const int digitNum = 10;
int dpState = 0;
int currentDigit = 0;
int indexD = 0;

int number[digitSize] = {0, 0, 0, 0};
int segments[segSize] = {pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP};  
int digitSegments[digitSize] = {pinD1, pinD2, pinD3, pinD4}; 

//joystick values
int xValue = 0;
int yValue = 0;
int swValue = 0;

//joystick variables
int yState = 0;
int xState = 0;
int maxTresHold = 800;
int minTresHold = 300;

//button variables
int buttonState = HIGH;          
int lastButtonState = HIGH; 
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
unsigned long lastDebounceTimeExit = 0;
unsigned long debounceDelayExit = 2000;
unsigned int buttonCounter = 0;


//variables for blinking dp
unsigned long prevTime = 0;
unsigned long currentTime = 0;
int interval = 250;

byte digitMatrix[10][segSize - 1] = {
// A  B  C  D  E  F  G
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};

//set which of the 4 segmets will display 
void displayDigit(byte digit){
  for(int i = 0; i < digitSize; i++) { 
    digitalWrite(digitSegments[i], HIGH);
  }
  digitalWrite(digitSegments[digit], LOW);
}

//display the digit 
void displayNumber(byte number, byte dpState) {
  for(int j = 0; j < segSize - 1; j++){
     digitalWrite(segments[j], digitMatrix[number][j]);
  }
  digitalWrite(segments[segSize - 1], dpState); 
}


//display the full number on
void displayFullNumber(int dp) {
  indexD = 0;
  while (indexD < digitSize) {
    displayDigit(indexD);
    displayNumber(number[indexD], indexD == dp );
    delay(5);
    indexD++;
  }
}

//debouncing for the button
byte checkDebounce(int reading) {
   if ((millis() - lastDebounceTime) > debounceDelay) {
      if (swValue != buttonState) {
         buttonState = swValue;
    
        if (buttonState == LOW) {
           return 1;
        }
      }
   }
   return 0;
}

//how the X-axis controls the 4-7 display
void changesToX(int readX) {
   if (readX <= minTresHold &&  !xState){
      if (currentDigit == 0) {
        currentDigit= digitSize - 1;
      }
      else {
        currentDigit--;
      }
      xState = 1;
  }
  else if (readX >= maxTresHold &&  !xState){
      if (currentDigit == digitSize - 1) {
        currentDigit = 0;
      }
      else {
         currentDigit++;
      }
      xState = 1;
  }
  else if (readX > minTresHold && readX < maxTresHold && xState){
      xState = 0;
  }
}

//how the Y-axis controls the 4-7 display
void changesToY(int readY) {
  if (readY <= minTresHold &&  !yState){
     if (number[currentDigit] == 0) {
        number[currentDigit] = digitNum - 1;
     }
     else {
        number[currentDigit]--;
     }
     yState = 1;
  }
  else if (readY >= maxTresHold &&  !yState){
     if (number[currentDigit] == digitNum - 1) {
        number[currentDigit] = 0;
     }
     else {
        number[currentDigit]++;
     }
     yState = 1;
  }
  else if (readY > minTresHold && readY < maxTresHold && yState){
     yState = 0;
  }                  
}

void setup() {
  // put your setup code here, to run once:
  for(int index = 0; index < segSize; index++){
    pinMode(segments[index], OUTPUT);
  }
  for(int index = 0; index < digitSize; index++){
    pinMode(digitSegments[index], OUTPUT);
  }
  pinMode(pinSW, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
    // put your main code here, to run repeatedly:
    swValue = digitalRead(pinSW);
    displayFullNumber(-1);

    if (swValue  != lastButtonState) {
        lastDebounceTime = millis();
    }

    buttonCounter = buttonCounter + checkDebounce(swValue); 
     
    if (buttonCounter == 1) {
        prevTime = millis();
        dpState = currentDigit; 
         //loop for choosing the one of the 4 digit segments
        do {

            //changing the state of dp for the blinking
            if ( (millis() - prevTime) > interval) {
              if (dpState == -1) {
                dpState = currentDigit;
              }
              else {
                dpState = -1;
              }
              prevTime = millis(); 
            }
            displayFullNumber(dpState);
            
            
            xValue = analogRead(pinX);
            changesToX(xValue);
            
            if (buttonCounter >= 2) {
                //loop for setting the desired digit
                do {
                    displayFullNumber(currentDigit);
                    
                    yValue = analogRead(pinY);
                    changesToY(yValue);
                    
                    swValue = digitalRead(pinSW);
                    if (swValue  != lastButtonState) {
                          lastDebounceTime = millis();
                    }
                    buttonCounter = buttonCounter + checkDebounce(swValue); 
                    lastButtonState = swValue;
                    
                 }while(buttonCounter <= 2);
                 buttonCounter = 1;
            }
            
            swValue = digitalRead(pinSW);
            if (swValue  != lastButtonState) {
                          lastDebounceTime = millis();
            }

            //checking if the user wants to exit or wants to enter the next stage
            if (checkDebounce(swValue)) {
                lastDebounceTimeExit = millis();
                do {
                    swValue = digitalRead(pinSW);
                    displayFullNumber(currentDigit);
                }while(swValue == 0 && (millis() - lastDebounceTimeExit) < debounceDelayExit);
                
                if ((millis() - lastDebounceTimeExit) > debounceDelayExit) {
                      buttonCounter = 0;
                }
                else {
                   buttonCounter = 2; 
                }
            }
            lastButtonState = swValue;
            
        }while(buttonCounter >= 1);
    }
    currentDigit = 0;
    lastButtonState = swValue;
}
