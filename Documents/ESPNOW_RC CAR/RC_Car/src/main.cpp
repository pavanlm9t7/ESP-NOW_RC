#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#define lf    25
#define lb    26
#define rb    32
#define rf    33



struct values
{
  int leftforward, leftbackward, rightforward, rightbackward;
};
struct values s;

void standby(){
  digitalWrite(rf, LOW);
  digitalWrite(rb, LOW);
  digitalWrite(lf, LOW);
  digitalWrite(lb, LOW);
}

void moveForward(){
  digitalWrite(lf, HIGH);
  digitalWrite(rf, HIGH);
  //standby();
}

void moveBackward(){
  digitalWrite(lb, HIGH);
  digitalWrite(rb, HIGH);
  //standby();
}

void turnLeft(){
  digitalWrite(lb, HIGH);
  digitalWrite(rf, HIGH);
  //standby();
}

void turnRight(){
  digitalWrite(lf, HIGH);
  digitalWrite(rb, HIGH);
  //standby();
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incommingData, int len){
  memcpy(&s, incommingData, sizeof(s));
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  pinMode(rf, OUTPUT);
  pinMode(lf, OUTPUT);
  pinMode(rb, OUTPUT);
  pinMode(lb, OUTPUT);
  if(esp_now_init()!=ESP_OK){
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  standby();
}

void loop() {
  
  if((s.leftforward == 1) && (s.rightforward == 1) && (s.leftbackward == 0) && (s.rightbackward == 0)){
    moveForward();
  }else if((s.leftforward == 0) && (s.rightforward == 0) && (s.leftbackward == 1) && (s.rightbackward == 1)){
    moveBackward();
  }else if((s.leftforward == 1) && (s.rightforward == 0) && (s.leftbackward == 0) && (s.rightbackward == 1)){
    turnRight();
  }else if((s.leftforward == 0) && (s.rightforward == 1) && (s.leftbackward == 1) && (s.rightbackward == 0)){
    turnLeft();
  }else{
    standby();
  }
  Serial.println("LF: "+String(s.leftforward)+" RF: "+String(s.rightforward)+" LB: "+String(s.leftbackward)+" RB: "+String(s.rightbackward));
  delay(100);
}


