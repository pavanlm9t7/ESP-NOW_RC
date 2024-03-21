#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#define fwd    25
#define rev    26
#define left   32
#define right  33

struct values
{
  int leftforward, leftbackward, rightforward, rightbackward;
};
struct values s;

void forwardSet(){
  s.rightforward = 1;
  s.leftforward = 1;
}
void reverseSet(){
  s.rightbackward = 1;
  s.leftbackward = 1;
}
void leftTurn(){
  s.leftbackward = 1;
  s.rightforward = 1;
}
void rightTurn(){
  s.leftforward = 1;
  s.rightbackward = 1;
}
void standby(){
  s.leftbackward = 0;
  s.leftforward = 0;
  s.rightforward = 0;
  s.rightbackward = 0;
}

uint8_t broadcastAddress[] = {0x08,0xD1,0xF9,0xD1,0xDF,0xDC};

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

esp_now_peer_info_t peerInfo;


void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  pinMode(fwd, INPUT);
  pinMode(rev, INPUT);
  pinMode(left, INPUT);
  pinMode(right, INPUT);
  if(esp_now_init()!=ESP_OK){
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if(esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  standby();
}

void loop() {

  if((digitalRead(fwd) == HIGH) && (digitalRead(rev) == LOW) && (digitalRead(left) == LOW) && (digitalRead(right) == LOW)){
    forwardSet();
  }else if((digitalRead(fwd) == LOW) && (digitalRead(rev) == HIGH) && (digitalRead(left) == LOW) && (digitalRead(right) == LOW)){
    reverseSet();
  }else if((digitalRead(fwd) == LOW) && (digitalRead(rev) == LOW) && (digitalRead(left) == HIGH) && (digitalRead(right) == LOW)){
    leftTurn();
  }else if((digitalRead(fwd) == LOW) && (digitalRead(rev) == LOW) && (digitalRead(left) == LOW) && (digitalRead(right) == HIGH)){
    rightTurn();
  }else{
    standby();
  }

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t*)&s, sizeof(s));

  if(result == ESP_OK){
    Serial.println("Sending Confirmed");
  }
  else{
    Serial.println("Sending Error");
  }
  
  Serial.println("LF: "+String(s.leftforward)+" LB: "+String(s.leftbackward)+" RF: "+String(s.rightforward)+" RB: "+String(s.rightbackward));
  delay(50);
  standby();
} 


