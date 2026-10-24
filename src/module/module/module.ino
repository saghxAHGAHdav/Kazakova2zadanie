#include <Servo.h>
#include <DHT11.h>           
#define pservo 9
#define pdht 2
#define trig 7
#define echo 6
#define plight A0
#define plamp 13
#define tempmax 30
#define lightnight 300
#define carcm 20
#define OPEN 90
#define CLOSED 0
Servo servo;
DHT11 dht11(pdht);
bool automode = false;
bool isopen = false;
float t,h;
int light;
long dist;
String status = "OK";
unsigned long lastread = 0;
void setup() {
  Serial.begin(9600);
  servo.attach(pservo);
  servo.write(CLOSED);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(plamp, OUTPUT);
  


}

void loop() {
  if (Serial.available()) command(Serial.readStringUntil('\n'));
  if (millis() - lastread >= 500) {
    lastread = millis();
    readsensors();
    if (automode && status == "OK") gate(dist < carcm);
  }

}
void gate(bool open) {
    isopen = open;
    servo.write(open ? OPEN : CLOSED);
}
void command(String cmd) {
    if (cmd == "M") automode = false;
    else if (cmd == "A") automode = true;
    else if (cmd == "O" && !automode) gate(true);
    else if (cmd == "C" && !automode) gate(false);
    else if (cmd == "T") selftest();
    else if (cmd == "R") {automode = false; gate(false);}
}
long readdist() {
    digitalWrite(trig, 1); delayMicroseconds(10); digitalWrite(trig, 0);
    long us = pulseIn(echo, 1, 30000);
    return us == 0 ? 999 : us / 58;
}
void readsensors() {
    t = dht11.readTemperature();
    h = dht11.readHumidity();
    light = analogRead(plight);
    dist = readdist();
    if (isnan(t) || isnan(h)) status = "NO_SIGNAL";
    else if (t > tempmax) status = "OVERHEAT";
    else status = "OK";
    digitalWrite(plamp, light < lightnight);
    Serial.print("DATA;");
    Serial.print(automode ? "AUTO;" : "MANUAL;");
    Serial.print(isopen ? "OPEN;" : "CLOSED;");
    Serial.print(dist); Serial.print(";");
    Serial.print(t, 1); Serial.print(";");
    Serial.print(h, 0); Serial.print(";");
    Serial.print(light); Serial.print(";");
    Serial.print(light < lightnight ? "DAY;" : "NIGHT;");
    Serial.println(status);
}
void selftest() {
    Serial.println(isnan(dht11.readTemperature()) ? "TEST;DHT11;FAIL нет сигнала" : "TEST;DHT11;OK");
    Serial.print("TEST;HC-SR04;"); Serial.println(readdist());
    Serial.print("TEST;LIGHT;"); Serial.println(analogRead(plight));
    servo.write(OPEN); delay(700); servo.write(CLOSED); delay(700);
    gate(isopen);
    Serial.println("TEST;SERVO;OK, проверить движение");
}
