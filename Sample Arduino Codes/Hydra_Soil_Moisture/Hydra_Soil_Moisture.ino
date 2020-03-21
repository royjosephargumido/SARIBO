int moisture = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  moisture = analogRead(A0);
  Serial.print("Soil moisture: ");
  Serial.println(moisture);
  delay(500);
}
