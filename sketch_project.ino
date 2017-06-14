#include <LTask.h>
#include <LGSM.h>
const int pinLed = 13;

void handleMsg(String dtaget){
  if(dtaget == "set")
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!LSMS.ready()){
    delay(100);  
  }
  Serial.println("GSM OK!!");
}

void loop() {
  // put your main code here, to run repeatedly:
  String p_num;
  int len = 0;
  String dtaget;
//  Serial.println(LSMS.available());
  if(LSMS.available()){
    LSMS.remoteNumber(p_num,20);
    Serial.println("There is new message.");
    Serial.print("Number: ");
    Serial.println(p_num);
    Serial.print("Content: ");
    while(true){
      int v = LSMS.read();
      if(v < 0) break;

      dtaget[len++] = (char)v;


    } 
    handleMsg(staget, p_num)
    Serial.println(dtaget); 
    LSMS.flush();
  }
}


