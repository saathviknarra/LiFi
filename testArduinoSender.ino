int green = 12;
int red = 11;
#define DELAY 3

void setup() {
  // put your setup code here, to run once:
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
}

void loop() {
  while(1) {

    digitalWrite(red, HIGH);
    delay(DELAY);
    digitalWrite(red, LOW);
    delay(DELAY);
    //exit(0);
  }

}
