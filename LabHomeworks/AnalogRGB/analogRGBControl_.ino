int potPinR = A0;
int potPinG = A1;
int potPinB = A2;

int ledPinR = 9;
int ledPinG = 10;
int ledPinB = 11;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPinR, OUTPUT);
  pinMode(ledPinG, OUTPUT);
  pinMode(ledPinB, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  set_color(set_potVal(potPin0), set_potVal(potPin1), set_potVal(potPin2));
}

int set_potVal(int potPin){
    return map(analogRead(potPin), 0, 1023, 0, 255);
}

void set_color(int red, int green, int blue){
    analogWrite(ledPinR, red);
    analogWrite(ledPinG, green);
    analogWrite(ledPinB, blue);
}
