const int SWITCH_PIN = 13;
const int SHAKE_PIN = 6;
const int BUTTON_PIN = 2;

void shine(void)
{
  delay(200);
  digitalWrite(SWITCH_PIN, HIGH);
  delay(200);
  digitalWrite(SWITCH_PIN, LOW);

  delay(200);
  digitalWrite(SWITCH_PIN, HIGH);
  delay(200);
  digitalWrite(SWITCH_PIN, LOW);
  
  delay(200);
  digitalWrite(SWITCH_PIN, HIGH);
  delay(200);
  digitalWrite(SWITCH_PIN, LOW);
  delay(200);
}

void setup() {
  Serial.begin(9600);
  pinMode(SWITCH_PIN, OUTPUT);

  Serial.println("Arduino begin!");
}

void loop() {
  if(digitalRead(SHAKE_PIN) == HIGH)  //  震動開關
  {
    Serial.println("Shake!");
    shine();
  }
  else if(digitalRead(BUTTON_PIN) == LOW) //  按下按鈕(不知道為甚麼LOW才是按下按鈕，跟PPT上的不一樣)
  {
    Serial.println("Press Button!");
    shine();
  }
 
  delay(300);
}
