int buzzerPin = 7;
int currentColor = 0;
int ledPin = 13;

#include <Servo.h>
#include "pitches.h"

int servoPinDown = 11;
int servoPinUp = 6;
Servo servoDown, servoUp;
int penUpAngle = 5;
int penDownAngle = 180;
int angle = 0;

#include <Stepper.h>

#define LINE_BUFFER_LENGTH 512

// Should be right for DVD steppers, but is not too important here
const int stepsPerRevolution = 20;

// Initialize steppers for X- and Y-axis using this Arduino pins for the L293D H-bridge
Stepper myStepperY(stepsPerRevolution, 2, 3, 4, 5);
Stepper myStepperX(stepsPerRevolution, 8, 9, 10, 12);

/* Structures, global variables    */
struct point {
  float x;
  float y;
};

// Current position of plothead
struct point actuatorPos;

//  Drawing settings, should be OK
const float stepInc = 1;
const int stepDelay = 0;
const int lineDelay = 50;
const int penDelay = 100;
const int changeColorTimer = 10000;

// Motor steps to go 1 millimeter.
// Use test sketch to go 100 steps. Measure the length of line. 
// Calculate steps per mm. Enter here.
float stepsPerMillimeterX = 6.0;
float stepsPerMillimeterY = 6.0;

// Drawing robot limits, in mm
// OK to start with. Could go up to 50 mm if calibrated well. 
float xMin = 0;
float xMax = 40;
float yMin = 0;
float yMax = 40;

float xPos = xMin;
float yPos = yMin;

// Set to true to get debug output.
boolean verbose = false;

void setup() {
  //  Setup
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);

  servoDown.attach(servoPinDown);
  servoUp.attach(servoPinUp);
  servoDown.write(penUpAngle);
  servoUp.write(penUpAngle);

  delay(200);

  myStepperX.setSpeed(150);
  myStepperY.setSpeed(150);
}

void loop() {
  delay(200);
  char line[LINE_BUFFER_LENGTH];
  char c;
  int lineIndex;
  bool lineIsComment, lineSemiColon;

  lineIndex = 0;
  lineSemiColon = false;
  lineIsComment = false;

  while (1) {

    // Serial reception - Mostly from Grbl, added semicolon support
    while (Serial.available() > 0) {
      c = Serial.read();
      if ((c == '\n') || (c == '\r')) { // End of line reached
        if (lineIndex > 0) { // Line is complete. Then execute!
          line[lineIndex] = '\0'; // Terminate string
          if (verbose) {
            Serial.print("Received : ");
            Serial.println(line);
          }
          processIncomingLine(line, lineIndex);
          lineIndex = 0;
        } else {
          // Empty or comment line. Skip block.
        }
        lineIsComment = false;
        lineSemiColon = false;
        Serial.println("ok");
      } else {
        if ((lineIsComment) || (lineSemiColon)) { // Throw away all comment characters
          if (c == ')') lineIsComment = false; // End of comment. Resume line.
        } else {
          if (c <= ' ') { // Throw away whitepace and control characters
          } else if (c == '/') { // Block delete not supported. Ignore character.
          } else if (c == '(') { // Enable comments flag and ignore all characters until ')' or EOL.
            lineIsComment = true;
          } else if (c == ';') {
            lineSemiColon = true;
          } else if (lineIndex >= LINE_BUFFER_LENGTH - 1) {
            Serial.println("ERROR - lineBuffer overflow");
            lineIsComment = false;
            lineSemiColon = false;
          } else if (c >= 'a' && c <= 'z') { // Upcase lowercase
            line[lineIndex++] = c - 'a' + 'A';
          } else {
            line[lineIndex++] = c;
          }
        }
      }
    }
  }
}

void processIncomingLine(char * line, int charNB) {
  int currentIndex = 0;
  char buffer[64]; // Hope that 64 is enough for 1 parameter
  struct point newPos;

  newPos.x = 0.0;
  newPos.y = 0.0;

  //  Needs to interpret 
  //  G1 for moving
  //  G4 P300 (wait 150ms)
  //  G1 X60 Y30
  //  G1 X30 Y50
  //  M300 S30 (pen down)
  //  M300 S50 (pen up)
  //  C 30 (color) -- not a native gCode command
  //  Discard anything with a (
  //  Discard any other command!

  while (currentIndex < charNB) {
    switch (line[currentIndex++]) { // Select command, if any
    case 'U':
      penUp();
      break;
    case 'D':
      penDown();
      break;
    case 'G':
      buffer[0] = line[currentIndex++]; // /!\ Dirty - Only works with 2 digit commands
      buffer[1] = '\0';
      switch (atoi(buffer)) { // Select G command
      case 0: // G00 & G01 - Movement or fast movement. Same here
      case 1:
        // /!\ Dirty - Suppose that X is before Y
        char * indexX = strchr(line + currentIndex, 'X'); // Get X/Y position in the string (if any)
        char * indexY = strchr(line + currentIndex, 'Y');
        if (indexY <= 0) {
          newPos.x = atof(indexX + 1);
          newPos.y = actuatorPos.y;
        } else if (indexX <= 0) {
          newPos.y = atof(indexY + 1);
          newPos.x = actuatorPos.x;
        } else {
          newPos.y = atof(indexY + 1);
          indexY = '\0';
          newPos.x = atof(indexX + 1);
        }
        drawLine(newPos.x, newPos.y);
        actuatorPos.x = newPos.x;
        actuatorPos.y = newPos.y;
        break;
      }
      break;
    case 'M':
      buffer[0] = line[currentIndex++]; // /!\ Dirty - Only works with 3 digit commands
      buffer[1] = line[currentIndex++];
      buffer[2] = line[currentIndex++];
      buffer[3] = '\0';
      switch (atoi(buffer)) {
      case 300:
        {
          char * indexS = strchr(line + currentIndex, 'S');
          float Spos = atof(indexS + 1);
          if (Spos == 30) {
            penDown();
          }
          if (Spos == 50) {
            penUp();
          }
          break;
        }
      case 114: // M114 - Repport position
        Serial.print("Absolute position : X = ");
        Serial.print(actuatorPos.x);
        Serial.print("  -  Y = ");
        Serial.println(actuatorPos.y);
        break;
      default:
        Serial.print("Command not recognized : M");
        Serial.println(buffer);
      }
      break;
    case 'C': //only set for 4 colors: black, red, green, blue
      buffer[0] = line[currentIndex++];
      buffer[1] = line[currentIndex++];
      buffer[2] = '\0';
      int newColor = atoi(buffer);
      //Serial.print(buffer[0]);
      //Serial.print(buffer[1]);
      //Serial.println(buffer[2]);
      if (currentColor != newColor) {
        tone(buzzerPin, 1000, changeColorTimer);
        digitalWrite(ledPin, HIGH);
        Serial.print("Switch color to: ");
        switch (newColor) {
        case 10:
          Serial.println("black!");
          break;
        case 30:
          Serial.println("red!");
          break;
        case 40:
          Serial.println("green!");
          break;
        case 50:
          Serial.println("blue!");
          break;
        default:
          Serial.print("Command not recognized : C");
          Serial.println(buffer);
          break;
        }
        //Serial.println("To confirm you switched colors input: Go");
        unsigned long startClock = millis();
        Serial.println((millis() - startClock) < changeColorTimer);
        while ((millis() - startClock) < changeColorTimer) {
          Serial.println(millis() - startClock);
        }
        noTone(buzzerPin);
        digitalWrite(ledPin, LOW);
        currentColor = newColor;
      }
      break;
    }
  }

}

/*********************************
 * Draw a line from (x0;y0) to (x1;y1). 
 * Bresenham algo from https://www.marginallyclever.com/blog/2013/08/how-to-build-an-2-axis-arduino-cnc-gcode-interpreter/
 * int (x1;y1) : Starting coordinates
 * int (x2;y2) : Ending coordinates
 **********************************/
void drawLine(float x1, float y1) {

  if (verbose) {
    Serial.print("fx1, fy1: ");
    Serial.print(x1);
    Serial.print(",");
    Serial.print(y1);
    Serial.println("");
  }

  //  Bring instructions within limits
  if (x1 >= xMax) {
    x1 = xMax;
  }
  if (x1 <= xMin) {
    x1 = xMin;
  }
  if (y1 >= yMax) {
    y1 = yMax;
  }
  if (y1 <= yMin) {
    y1 = yMin;
  }

  if (verbose) {
    Serial.print("xPos, yPos: ");
    Serial.print(xPos);
    Serial.print(",");
    Serial.print(yPos);
    Serial.println("");
  }

  if (verbose) {
    Serial.print("x1, y1: ");
    Serial.print(x1);
    Serial.print(",");
    Serial.print(y1);
    Serial.println("");
  }

  //  Convert coordinates to steps
  x1 = (int)(x1 * stepsPerMillimeterX);
  y1 = (int)(y1 * stepsPerMillimeterY);
  float x0 = xPos;
  float y0 = yPos;

  //  Let's find out the change for the coordinates
  long dx = abs(x1 - x0);
  long dy = abs(y1 - y0);
  int sx = x0 < x1 ? stepInc : -stepInc;
  int sy = y0 < y1 ? stepInc : -stepInc;

  long i;
  long over = 0;

  if (dx > dy) {
    for (i = 0; i < dx; ++i) {
      myStepperX.step(sx);
      over += dy;
      if (over >= dx) {
        over -= dx;
        myStepperY.step(sy);
      }
      delay(stepDelay);
    }
  } else {
    for (i = 0; i < dy; ++i) {
      myStepperY.step(sy);
      over += dx;
      if (over >= dy) {
        over -= dy;
        myStepperX.step(sx);
      }
      delay(stepDelay);
    }
  }

  if (verbose) {
    Serial.print("dx, dy:");
    Serial.print(dx);
    Serial.print(",");
    Serial.print(dy);
    Serial.println("");
  }

  if (verbose) {
    Serial.print("Going to (");
    Serial.print(x0);
    Serial.print(",");
    Serial.print(y0);
    Serial.println(")");
  }

  //  Delay before any next lines are submitted
  delay(lineDelay);
  //  Update the positions
  xPos = x1;
  yPos = y1;
}

//  Raises pen
void penUp() {
  //In the up movement of the pen the servoDown moves to the left in order to release tension in it's string, allowing the
  //servoUp to lift the pen up by moving itself to the right and increasing the tension in it's string 
  for (angle = penDownAngle; angle >= penUpAngle; angle--) {
    servoDown.write(angle);
  }
  delay(penDelay);
  for (angle = penDownAngle; angle >= penUpAngle; angle--) {
    servoUp.write(angle);
  }
  delay(lineDelay);
  if (verbose) {
    Serial.println("Pen up!");
  }
}

//  Lowers pen
void penDown() {
  //In the down movement the oposite happens. 
  //The servoDown moves to the right in order to increase the tension in it's string, pulling the pen down 
  //while the servoUp moves to the left in order to realease the tension in it's string 
  for (angle = penUpAngle; angle <= penDownAngle; angle++) {
    servoUp.write(angle);
  }
  delay(penDelay);
  for (angle = penUpAngle; angle <= penDownAngle; angle++) {
    servoDown.write(angle);
  }
  delay(lineDelay);
  if (verbose) {
    Serial.println("Pen down.");
  }
}
