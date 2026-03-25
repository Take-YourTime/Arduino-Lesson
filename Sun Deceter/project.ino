const int BLUE_PIN = 11;
const int GREEN_PIN = 10;
const int RED_PIN = 9;

const int LIGHTSENSOR_PIN1 = A0;
const int LIGHTSENSOR_PIN2 = A1;
const int LIGHTSENSOR_PIN3 = A2;
// 370 660 634

int lightsensorValue1 = 0; //  光敏電阻值
int lightsensorValue2 = 0;
int lightsensorValue3 = 0;

void light(int red, int green, int blue)
{
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}

void setup() {
  Serial.begin(9600);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  light(0, 0, 0);

  Serial.println("Arduino begin!");
}

void loop() {
  lightsensorValue1 = analogRead(LIGHTSENSOR_PIN1);
  lightsensorValue2 = analogRead(LIGHTSENSOR_PIN2);
  lightsensorValue3 = analogRead(LIGHTSENSOR_PIN3);

  Serial.print("光敏電阻值：");
  Serial.print(lightsensorValue1 - 550);
  Serial.print(" ");
  Serial.print(lightsensorValue2 - 660);
  Serial.print(" ");
  Serial.println(lightsensorValue3 - 640);

  delay(250);
}
