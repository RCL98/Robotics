int potPin0 = A0;
int potPin1 = A1;
int potPin2 = A2;
int potVal_0 = 0;
int potVal_1 = 0;
int potVal_2 = 0;

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
//  Serial.print("Var 0:");Serial.println(potVal_0);
//  Serial.print(" Var 1:");Serial.println(potVal_1);
//  Serial.print(" Var 2:");Serial.println(potVal_2);
//  Serial.print("-----------------------------------------------------\n");
//  delay(2000);
}

int set_potVal(int potPin){
    return map(analogRead(potPin), 0, 1023, 0, 255);
}

void set_color(int red, int green, int blue){
    analogWrite(ledPinR, red);
    analogWrite(ledPinG, green);
    analogWrite(ledPinB, blue);
}
