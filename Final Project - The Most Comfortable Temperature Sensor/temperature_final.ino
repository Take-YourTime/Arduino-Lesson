#include "DHT.h"
#include <math.h>
#include <Wire.h> // I2C程式庫
#include <WebServer.h>
#include <LiquidCrystal_I2C.h> // LCD_I2C模組程式庫
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <string>


WebServer webserver(80);  // Set up the server on port 80


const char* ssid = "小譽宸";          // network SSID
const char* password = "Samis666";    // network password
/*
const char* ssid = "我心繫著你卻新細明體";          // network SSID
const char* password = "B113040045";    // network password
*/
// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://api.thingspeak.com/update";

// write API Key provided by thingspeak
String apiKey = "LLOK9ELOM30RL1DU";    // your write API key


#define DHTPIN 14         //  degital pin connected to sensor
#define DHTTYPE DHT11     //  define DHT type
DHT dht(DHTPIN, DHTTYPE); //  initia;ize DHT sensor

#define REDPIN 25         //  LED pin
#define GREENPIN 26       //  LED pin
#define BLUEPIN 27        //  LED pin

LiquidCrystal_I2C lcd(0x27, 16, 2);     


//  global variables
double t = 0.0;           //  temperature
double h = 0.0;           //  humidity
double Td = 0.0;          //  Dew point 露點溫度
double THI = 0.0;         //  舒適度
double MAX_THI = 0.0;     //  最大舒適度
double MIN_THI = 10000.0; //  最小舒適度
int backgroundIndex = 0;
String currentStatus = "";
String maxStatus = "";
String minStatus = "";
String suggesstion = "";
String icon = "";


//  const value
const double t2P_a = 6.1129;
const double t2P_b = 17.62;
const double t2P_c = 243.12;


// 存放HTML檔案
const char* htmlContent = R"rawliteral(
<html lang="en"> 
  <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      
      <meta http-equiv="Cache-Control" content="no-cache, no-store, must-revalidate">
      <meta http-equiv="Pragma" content="no-cache">
      <meta http-equiv="Expires" content="0">
      <title>環境舒適度顯示</title>
      <style>
          /* 頁面設置 */
          body {
              margin: 0;
              font-family: Arial, sans-serif;
              background: linear-gradient(to bottom, #69b3ff, #ffffff);
              color: #333;
              text-align: center;
          }

          /* 標題樣式 */
          header {
              background: #007acc;
              color: white;
              padding: 15px 0;
              font-size: 1.5em;
          }

          /* 主要區域 */
          .main {
              margin: 20px auto;
          }

          .comfort-display {
              font-size: 5em; /* 舒適度數字放大 */
              color: #fff;
              text-shadow: 3px 3px 5px rgba(0, 0, 0, 0.3);
              margin-top: 10px;
          }

          .status {
              font-size: 2em;
              margin-top: 10px;
              color: #333;
          }

          .suggesstion {
              font-size: 2em;
              margin-top: 10px;
              color: #d54a00; 
          }

          .record {
              font-size: 1.2em;
              margin-top: 20px;
          }

          .icon {
              font-size: 6em;
              margin-top: 10px;
              color: #fff;
              text-shadow: 2px 2px 5px rgba(0, 0, 0, 0.3);
          }

          /* 預報區塊 */
          .forecast {
              display: flex;
              justify-content: center;
              gap: 20px;
              margin-top: 30px;
          }

          .forecast-box {
              background: #f2f2f2;
              border-radius: 10px;
              padding: 10px;
              width: 80px;
          }

          .forecast-box p {
              margin: 5px 0;
          }

          footer {
              margin-top: 30px;
              color: #666;
          }
      </style>
  </head>
  <body>
      <!-- 標題區域 -->
      <header>環境舒適度</header>

      <!-- 主區域顯示舒適度 -->
      <div class="main">
          <!-- 圖標顯示 -->
          <div class="icon" id = main-icon>%icon%</div>
          <div class="comfort-display" id="comfort-level">%THI%</div>
          <div class="status" id="comfort-status">%currentStatus%</div>
      </div>

      <!-- 最大最小記錄 -->
      <div class="record">
          <p>濕度：<span id="h">%humanity%</span></p>
          <p>溫度：<span id="t">%temperature%</span></p>
          <p>露點溫度：<span id="td">%DewPoint%</span></p>
          <p>最大 THI：<span id="max-thi">%max_THI%</span>，當時狀況：<span id="max-status">%maxStatus%</span></p>
          <p>最小 THI：<span id="min-thi">%min_THI%</span>，當時狀況：<span id="min-status">%minStatus%</span></p>
      </div>

      <!-- 新增建議文字 -->
      <div class="suggesstion" id="suggess">%suggesstion%</div>

      <!-- 底部 -->
      <footer>即時環境舒適度數據</footer>
      
      <script>
          setTimeout(function() {
              window.location.reload(true);
          }, 5000); // 每 5 秒強制刷新

          let value = 'PLACEHOLDER'; // 將 'PLACEHOLDER' 替換為 Arduino 的數據
          let colors = ['#0000FF', '#8F8FBD', '#00FFFF', '#00FF00', '#FFA500', '#FF0000'];
          document.body.style.backgroundColor = colors[value % colors.length];
      </script>
  </body>
</html>
)rawliteral";


//  get dew point
double DewPoint(void)
{
  // https://peakd.com/cn-stem/@julian2013/stem-05-jjt8e1ad
  double Es = t2P_a * exp(t2P_b * t / (t + t2P_c));
  double E = Es * (h / 100);
  return (t2P_c * log(E / t2P_a)) / (t2P_b - log(E / t2P_a));
}

void getTHI(double Td)
{
  // https://tpl.ncl.edu.tw/NclService/pdfdownload?filePath=lV8OirTfsslWcCxIpLbUfmhaNcSH2c_7SXqeTdgPrceTpxDBZU9sMtSxKxlXz1UH&imgType=Bn5sH4BGpJw=&key=3FDJvyqaGGgB-BMFionz8FJalz-CWXDWgvTgtlzVmHIeVVU9OyINO4qBZJhLTxWd&xmlId=0007650040
  THI = t - 0.55 * (1.0 - exp( (17.269 * Td) / (Td + 237.3) - (17.269 * t) / (t + 237.3) ) ) * (t - 14.0);
}



//  get status by THI value
String getStatus(double value)
{
  /*
    THI低於10，非常寒冷。
    11-15，寒冷。
    16-19，稍寒。
    20-26，舒適。
    27-30，悶熱。
    大於31，威脅生命。
  */
  if(value < 10.0){
    return "非常寒冷";
  }
  else if(value < 15.0){
    return "寒冷";
  }
  else if(value < 19.0){
    return "稍寒";
  }
  else if(value < 26.0){
    return "舒適";
  }
  else if(value < 30.0){
    return "悶熱";
  }
  else{
    return "威脅生命";
  }
}

//  set LED color R|G|B
//    since R and B of our is reverse, we reverse the paramaters
void setLED(int b, int g, int r)
{
  analogWrite(REDPIN, r);
  analogWrite(GREENPIN, g);
  analogWrite(BLUEPIN, b);
}

//  blink LED
void blinkLED() {
    static unsigned long previousMillis = 0;
    const long interval = 500;  // 500 ms 間隔
    static bool ledState = false;

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        ledState = !ledState;
        if (ledState) setLED(255, 0, 0);
        else setLED(0, 0, 0);
    }
}

//  ThingSpeak url
String url = "https://api.thingspeak.com/update?api_key=U2Z4JLT7CPZM1HOR";
void UploadData(){ 
  //建立一個網頁
  HTTPClient http;  

  //把土壤濕度數值寫入網址
  String url1= url+"&field1="+(int)t+"&field2="+(int)h+"&field3="+(int)Td+"&field4="+(int)THI;
  //提交網址
  http.begin(url1);

  //讀取網頁內容
  int httpCode = http.GET();
  if(httpCode == HTTP_CODE_OK){
    String payload = http.getString();
    Serial.print("網頁內容 = ");
    Serial.print(payload);
  } 
  else{
     Serial.println("網路傳送失敗");
  }

  //關閉網頁
  http.end(); 
}

void handleRoot(void) {
  // 網站
  String html = htmlContent;
  html.replace("%THI%", String(THI));
  html.replace("%max_THI%", String(MAX_THI));
  html.replace("%min_THI%", String(MIN_THI));
  html.replace("%humanity%", String(h));
  html.replace("%temperature%", String(t));
  html.replace("%DewPoint%", String(Td));
  html.replace("%currentStatus%", currentStatus);
  html.replace("%maxStatus%", maxStatus);
  html.replace("%minStatus%", minStatus);
  html.replace("%suggesstion%", suggesstion.c_str());
  html.replace("%icon%", icon.c_str());
  html.replace("PLACEHOLDER", String(backgroundIndex));
  webserver.send(200, "text/html", html);
}


void setup() {
  Serial.begin(115200);
  dht.begin();
  
  // LED initialization
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  setLED(0, 0, 0);

  //  LCD initialization
  lcd.init();
  lcd.setBacklight(255);
  lcd.clear();

  Serial.println("Arduino Initialization!");

  //  wi-fi connection
  Serial.println("Connecting to WiFi..");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
  // 啟動 WebServer
  webserver.on("/", handleRoot);
  webserver.begin();
  Serial.println("Web server started.");
  Serial.println("Server started.");
}



void loop() {
  
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    // Read temperature as Celsius (the default)
    t = dht.readTemperature();  //  temperature
    h = dht.readHumidity();     //  humidity 相對溼度

    //  Exception -> check if read failed
    if (isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor! t"));
      return;
    }
    if(isnan(h))
    {
      Serial.println(F("Failed to read from DHT sensor! h"));
      return;
    }

    
    //  計算露點溫度
    Td = DewPoint();

    //  計算舒適度
    getTHI(Td);

    //  更新最大最小舒適度
    if(MAX_THI < THI){  
      MAX_THI = THI;
      maxStatus = getStatus(MAX_THI);
    }
    if(MIN_THI > THI){
      MIN_THI = THI;
      minStatus = getStatus(MIN_THI);
    }

    //  output to terminal
    Serial.print(" 溫度:");
    Serial.print(t);
    Serial.print(" 相對濕度:");
    Serial.print(h);
    Serial.print(" 露點溫度:");
    Serial.print(Td);
    Serial.print(" 舒適度:");
    Serial.print(THI);

    //  output to LCD
    lcd.clear();
    lcd.setCursor(0, 0); // (colum, row)從第一排的第三個位置開始顯示
    lcd.print("THI:");
    lcd.print(THI);
  
    /*
      THI低於10，非常寒冷。
      11-15，寒冷。
      16-19，稍寒。
      20-26，舒適。
      27-30，悶熱。
      大於31，威脅生命。
    */

    lcd.setCursor(0, 1);
    if(THI < 10.0)
    {
      Serial.println(" 非常寒冷");
      lcd.print("Freezing");
      currentStatus = "非常寒冷";
      suggesstion = "提高溫度";
      backgroundIndex = 0;
      icon = "❄️";
      setLED(0, 0, 139);
    }
    else if(THI < 15.0)
    {
      Serial.println(" 寒冷");
      currentStatus = "寒冷";
      suggesstion = "提高溫度";
      backgroundIndex = 1;
      icon = "🥶";
      lcd.print("Cold");
      setLED(0, 255, 255);
    }
    else if(THI < 19.0)
    {
      Serial.println(" 稍寒");
      currentStatus = "稍寒";
      suggesstion = "提高溫度";
      backgroundIndex = 2;
      icon = "🌬️";
      lcd.print("Cool");
      setLED(0, 128, 128);
    }
    else if(THI < 26.0)
    {
      Serial.println(" 舒適");
      currentStatus = "舒適";
      suggesstion = "狀態良好";
      backgroundIndex = 3;
      icon = "😀";
      lcd.print("Great");
      setLED(0, 128, 0);
    }
    else if(THI < 30.0)
    {
      Serial.println(" 悶熱");
      currentStatus = "悶熱";
      suggesstion = "降低溫度";
      backgroundIndex = 4;
      icon = "🥵";
      lcd.print("Hot");
      setLED(255, 165, 0);
    }
    else
    {
      Serial.println(" 威脅生命");
      currentStatus = "威脅生命";
      suggesstion = "降低溫度";
      backgroundIndex = 5;
      icon = "💀";
      lcd.print("Threaten");
      blinkLED();
    }

    webserver.handleClient();

    //  update ThingSpeak
    UploadData();
    delay(1000);
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}
