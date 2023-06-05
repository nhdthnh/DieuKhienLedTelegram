#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>
// Replace with your network credentials
const char* ssid = "Dinh Thanh";
const char* password = "thanhdat16";
#define BOTtoken "6205441124:AAEmPpe0Xmhl2z4w8Y8okpGbzXrlqUt_K1o"  
#define CHAT_ID "1104099590"
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
const int LEDPin = 2;
bool LEDState = LOW;
// Hàm xử lý nhận tin nhắn
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));
  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    // In ra tin nhắn
    String text = bot.messages[i].text;
    Serial.println(text);
    String from_name = bot.messages[i].from_name;
    if (text == "/start") {
      String welcome = "Xin chào, " + from_name + ".\n";
      welcome += "Hãy sử dụng các lệnh dưới đây để bật tắt LED.\n\n";
      welcome += "/on để bật LED \n";
      welcome += "/off để tắt LED \n";
      welcome += "/state để kiểm tra trạng thái LED \n";
      bot.sendMessage(chat_id, welcome, "");
    }
    if (text == "/on") {
      bot.sendMessage(chat_id, "LED đã được bật", "");
      LEDState = HIGH;
      digitalWrite(LEDPin, LEDState);
    }
    if (text == "/off") {
      bot.sendMessage(chat_id, "LED đã được tắt", "");
      LEDState = LOW;
      digitalWrite(LEDPin, LEDState);
    }
    if (text == "/state") {
      if (digitalRead(LEDPin)){
        bot.sendMessage(chat_id, "LED đang bật", "");
      }
      else{
        bot.sendMessage(chat_id, "LED đã tắt", "");
      }
    }
}
}
void setup() {
  Serial.begin(115200);
  #ifdef ESP8266
    client.setInsecure();
  #endif
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);  // Add root certificate for api.telegram.org
  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, LEDState);
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}
void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}