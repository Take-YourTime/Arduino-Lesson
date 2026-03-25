const int BLUE_PIN = 11;
const int GREEN_PIN = 10;
const int RED_PIN = 9;

const int SENSOR_PIN = A0;
const int LIGHTSENSOR_PIN = A5;

int sensorValue = 0;  //  可變電阻值
int LEDlight = 0;
bool reverse = false;

int lightsensorValue = 0; //  光敏電阻值

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
  sensorValue = analogRead(SENSOR_PIN);
  sensorValue = map(sensorValue, 0, 1023, 0, 255);
  Serial.print("可變電阻值：");
  Serial.print(sensorValue);

  lightsensorValue = analogRead(LIGHTSENSOR_PIN);
  Serial.print("  光敏電阻值：");
  Serial.println(lightsensorValue);

  //  LED breathe
  if(reverse)
  {
    LEDlight += sensorValue / 5;
    if(LEDlight >= 255)
    {
      LEDlight = 255;
      reverse = false;
    }
  }
  else
  {
    LEDlight -= sensorValue / 10;
    if(LEDlight <= 0)
    {
      LEDlight = 0;
      reverse = true;
    }
  }

  //  LED color
  if(lightsensorValue <= 200)
    light(LEDlight, 0, 0);
  else if(lightsensorValue <= 550)
    light(0, LEDlight, 0);
  else
    light(0, 0, LEDlight);
  
  delay(250);
}
