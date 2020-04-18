int largest;
int smallest;

void setup() {
  Serial.begin(115200);
  smallest = analogRead(A0);
}

void loop() {
  const int val = analogRead(A0);
  
  if(val > largest)
    largest = val;
  else if(val < smallest)
    smallest = val;
  
  Serial.printf("\nValue:\t%d\n", val);
  Serial.printf("Smallest:\t%d\n", smallest);
  Serial.printf("Largest:\t%d\n", largest);
  delay(1000);
}
