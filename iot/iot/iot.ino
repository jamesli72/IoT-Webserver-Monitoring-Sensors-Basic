#include "DHT.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "time.h"
#include <Firebase_ESP_Client.h>

#include <OneWire.h>
#include <DallasTemperature.h>

// Chân nối với Arduino
#define ONE_WIRE_BUS 5
//Thiết đặt thư viện onewire
OneWire oneWire(ONE_WIRE_BUS);
//Đọc giá trị của cảm biến ds18b20
DallasTemperature sensors(&oneWire);

#include "addons/RTDBHelper.h"
#include "addons/TokenHelper.h"

//Khai báo wifi, và server truy xuất thời gian truy cập
//const char* ssid = "Hờ Mờ Cờ";
//const char* password = "hmc01062002";
const char* ntpServer = "vn.pool.ntp.org";
const char* ssid = "TitanFall.com";
const char* password = "haogia123";
//Khai báo biến và các đường dẫn cơ sở dữ liệu của firebase
byte data1,data2;
byte lastData1, lastData2;
String uid;
String tempPath = "/innertemp";
String humPath = "/humid";
String gasPath = "/gas";
String temp2Path ="/outtertemp";
String timePath = "/timestamp";
String databasePath;
int timestamp;
FirebaseJson json;
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 60000;

//Định nghĩa chân kết nối DHT 11
#define DHTPIN 2
#define DHTTYPE DHT11

//Định nghĩa đường dẫn cơ sở dữ liệu và mã API truy cập
#define API_KEY "AIzaSyBO7GDJQMNRyJ9mZegNo2MsCBPjt1p9LQQ"
#define DATABASE_URL "https://iot-lab-eeb2b-default-rtdb.firebaseio.com/"
// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "ads123@gmail.com"
#define USER_PASSWORD "12332133"

DHT dht(DHTPIN, DHTTYPE); //Truyền tham số kết nối cho DHT11
//Định nghĩa các đối tượng của firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig configF;

//Hàm trả về giá trị thời gian dạng stamp (giây)
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}
//==================================================
//Hàm khởi tạo kết nối wifi
void initWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
}
//==================================================
//Hàm chớp tắt led
void blink(){
   digitalWrite(LED_BUILTIN,HIGH);
   delay(100);
   digitalWrite(LED_BUILTIN,LOW);
   delay(100);
}
//==================================================
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  //Mở giao tiếp với máy tính
  initWiFi(); //Khởi tạo wifi
  dht.begin();  //Khởi tạo DHT
  configTime(0, 0, ntpServer); //Khởi tạo thời gian
  configF.api_key = API_KEY;  //truyền tham số vào đối tượng config của firebase
  pinMode(LED_BUILTIN, OUTPUT); //cấu hình I/O, đây là một led on board esp8266
  pinMode(A0, INPUT); //cấu hình I/O
  // Assign the RTDB URL (required)
  configF.database_url = DATABASE_URL;    //truyền tham số vào đối tượng config
  //Assign the user sign in credentials
  auth.user.email = USER_EMAIL;       //truyền tham số vào đối tượng auth
  auth.user.password = USER_PASSWORD;

  // Assign the callback function for the long running token generation task */
  configF.token_status_callback = tokenStatusCallback;
  configF.max_token_generation_retry = 5;

  Firebase.begin(&configF, &auth);  //khởi tạo liên kết với firebase
  fbdo.setResponseSize(4096);     //giới hạn dung lượng kết nối trong đối tượng fbdo
  Firebase.reconnectWiFi(true);   //kết nối lại wifi trường hợp mất kết nối
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();  //lấy uid để gán vào đường dẫn chính
  Serial.print("User UID: ");
  Serial.println(uid);
  databasePath = "/UsersData/" + uid + "/readings";   //thiết lập đường dẫn chính của dữ liệu
}

void loop() {
  sensors.requestTemperatures();
  // put your main code here, to run repeatedly:
    if ( Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)  ) {
    sendDataPrevMillis = millis();

    //Get current timestamp
    timestamp = getTime();  // lấy giá trị thời gian
    Serial.print("time: ");
    Serial.println(timestamp);  // in ra monitor

    json.set(tempPath.c_str(), String(dht.readTemperature()));    // đặt các giá trị vào đường dẫn con với cấu trúc json
    json.set(humPath.c_str(), String(dht.readHumidity()));
    json.set(gasPath.c_str(), String(analogRead(A0)));
    json.set(temp2Path.c_str(), String(sensors.getTempCByIndex(0)));
    json.set(timePath.c_str(), String(timestamp));

    //Gán toàn bộ cấu trúc vào đường dẫn chính và đẩy lên firebase
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, databasePath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
    
  }
  
  if(  dht.readTemperature() < 30 && dht.readTemperature() > 40 || dht.readHumidity() > 70 ||  analogRead(A0) > 700 )  blink();   //cảnh báo ngưỡng nhiệt độ

  //Lấy giá trị từ firebase về dạng string
  if (Firebase.RTDB.getString(&fbdo, "/Web&App/Fan_status")){      
        if (fbdo.dataTypeEnum() == fb_esp_rtdb_data_type_string){
              data1 = fbdo.stringData().toInt();          //hàm lấy giá trị và đồng thời chuyển đổi về dạng Int   
              if(data1 != lastData1){
                    Serial.print("Data received 1: ");
                    Serial.println(data1); //print the data received from the Firebase database 
                    lastData1 = data1;
                }
              }
            }
        else {Serial.println(fbdo.errorReason());} //print he error (if any) 

  if (Firebase.RTDB.getString(&fbdo, "/Web&App/Speed")){
        if (fbdo.dataTypeEnum() == fb_esp_rtdb_data_type_string){
              data2 = fbdo.stringData().toInt();
              if(data2 != lastData2){
                    Serial.print("Data received 2: ");
                    Serial.println(data2); //print the data received from the Firebase database 
                    lastData2 = data2;
                }
              } 
            }
        else {Serial.println(fbdo.errorReason()); //print he error (if any)
              }



  }
