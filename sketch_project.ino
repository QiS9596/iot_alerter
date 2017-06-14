#include <LTask.h>
#include <LGSM.h>
const int pinLed = 13;
char phone_number_list[5][20];
int top_phone = 0;
String str_buffer;
void sendMsg(char p_num[], String msg);
bool test = 1;
int time = 1000;
void handleMsg(char dtaget[], char p_num[]){

  char * piece;
  piece = strtok(dtaget,"\n");
  if(!strcmp(piece,"[setting]")){
    for(int i = 0; i < 3; i++){
      piece = strtok(NULL,"\n");
      if(i<=1)
        strcpy(phone_number_list[i],piece);
      else time = atoi(piece);
    }  
  }
}

void sendMsg(char p_num[], String msg){
  LSMS.beginSMS(p_num);
  LSMS.print(msg);
  if(LSMS.endSMS()){
    Serial.println("SMS sent");  
  }else{
    Serial.println("SMS is not sent");
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  /*
  while(!LSMS.ready()){
    delay(100);  
  }*/
  Serial.println("GSM OK!!");
  
}
void printSettingInfo(){
  Serial.println(phone_number_list[0]);
  Serial.println(phone_number_list[1]);
}
void loop() {
  // put your main code here, to run repeatedly:
  char p_num[20];
  int len = 0;
  char dtaget[500];
//  Serial.println(LSMS.available());
  strcpy(dtaget , "[setting]\n0981936325\n0811224156\n15");
  handleMsg(dtaget,"11111111");
  printSettingInfo();
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
    dtaget[len] = 0;
    Serial.println(dtaget); 
    LSMS.flush();
  }
}


