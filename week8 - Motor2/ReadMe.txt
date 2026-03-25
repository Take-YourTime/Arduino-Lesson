1. 個人化 wifi 連接以及pin腳

更改 line 10 的 ssid 為 wifi名稱

更改 line 11 的 password 為 wifi 密碼

預設pin腳：
	馬達方向： 16
	速度： 4


2. 執行

執行 Arduino 程式碼

使用連接相同網路的手機或電腦打開瀏覽器，輸入serial monitor顯示的address ID

以html網頁操控馬達


3. 設計邏輯

[ integer ]

speed 變數控制馬達轉速，255靜止 ~ 0最大速度

vector 控制馬達轉向，HIGH: 正向 | LOW: 逆向


[ boolean ]

isOn 紀錄當前馬達狀態，True: 運轉中 | False: 待機中




3. 預期結果

點擊on，如果馬達為靜止狀態，設置speed為155

點擊off，isOn = false

點擊turnAround，reverse vector

點擊speedUp，加速轉速，根據當前轉向來增加或減少speed的值

點擊slowDown，減少轉速，根據當前轉向來增加或減少speed的值，與speedUp相反