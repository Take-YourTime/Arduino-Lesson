const int RED_PIN = 8;
const int GREEN_PIN = 12;
const int BLUE_PIN = 13;

void light(int red, int green, int blue)
{
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}

void shine(void)
{
  delay(200);
  light(0, 0, 0);
  delay(200);
  light(0, 255, 0);

  delay(200);
  light(0, 0, 0);
  delay(200);
  light(0, 255, 0);
  
  delay(200);
  light(0, 0, 0);
  delay(200);
  light(0, 255, 0);

  delay(200);
}

void setup() {
  Serial.begin(9600);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  Serial.println("Arduino begin!");
}

void loop() {
  Serial.print("現在燈號 = 綠燈, 目前已經過"); Serial.print(millis()); Serial.println("ms");
  light(0, 255, 0);
  delay(1000);

  //  shine three times when green turn into yellow
  shine();

  Serial.print("現在燈號 = 黃燈, 目前已經過"); Serial.print(millis()); Serial.println("ms");
  light(150, 150, 0);
  delay(1500);

  Serial.print("現在燈號 = 紅燈, 目前已經過"); Serial.print(millis()); Serial.println("ms");
  light(255, 0, 0);
  delay(2000);
}
