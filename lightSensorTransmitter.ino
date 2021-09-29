int green = 11;
int red = 12;
void setup() {
  // put your setup code here, to run once:
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(green, HIGH);
  delay(10);
  digitalWrite(green, LOW);
  delay(10);
  digitalWrite(red, HIGH);
  delay(10);
  digitalWrite(red, LOW);
  delay(10);
  //exit(0);
}
