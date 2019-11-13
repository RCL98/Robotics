const int knockPin = A0;
const int pushButton = 2;
const int buzzerPin = 10;

int knockValue = 0 ;
int buttonState = 0 ;
int buzzerVal = 0;
unsigned int previousTime = 0;
unsigned int currentTime = 0;
unsigned int state = 0;
const int interval = 5500;
const int threshold = 5;


#include "pitches.h"

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

int thisNote = 0;

void setup () {
  pinMode(pushButton, INPUT_PULLUP);
  pinMode(knockPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
}

void loop () {
    knockValue = analogRead(knockPin);
    buttonState = digitalRead(pushButton);
    //in state 0 we wait for a "knock" value that passes the treshold
    if (!state && knockValue > threshold) {
        state = 1;
        previousTime = millis();
        Serial.println("Knock!");
    }
    
    //in state 1 we wait for the required waiting time to pass
    if (state == 1) {
        currentTime = millis() - previousTime;
        if (currentTime > interval) {
            knockValue  = 0;
            state = 2;
            Serial.println("Play!");
        }
    }
    
    //in state 2 the music plays until the button is hit
    if ( state == 2) {
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(buzzerPin, melody[thisNote], noteDuration);
        
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        noTone(buzzerPin);
        
        thisNote = thisNote + 1;
        if (thisNote == 8) {
            thisNote = 0;
            delay(5);
        }
        
        if (buttonState == LOW) {
            noTone(buzzerPin);
            state = 0;
            thisNote = 0;
        }
    }
}
