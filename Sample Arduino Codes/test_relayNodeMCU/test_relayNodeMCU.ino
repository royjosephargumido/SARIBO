int r1 = 5; // the input to the relay pin
int r2 = 14;
int r3 = 12;
int r4 = 15;

void setup() {
  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  pinMode(r3, OUTPUT);
  pinMode(r4, OUTPUT);
}

void loop() {

  digitalWrite(r1, HIGH);
  digitalWrite(r2, LOW);
  digitalWrite(r3, LOW);
  digitalWrite(r4, LOW);
  delay(250);
  
  digitalWrite(r1, LOW);
  digitalWrite(r2, HIGH);
  digitalWrite(r3, LOW);
  digitalWrite(r4, LOW);
  delay(250);

  digitalWrite(r1, LOW);
  digitalWrite(r2, LOW);
  digitalWrite(r3, HIGH);
  digitalWrite(r4, LOW);
  delay(250);

  digitalWrite(r1, LOW);
  digitalWrite(r2, LOW);
  digitalWrite(r3, LOW);
  digitalWrite(r4, HIGH);
  delay(250);
}
