#include <LTask.h>
#include <LGSM.h>
#include <LGPS.h>//gps

gpsSentenceInfoStruct info;//gps info
char buff[256];//gps buffer
char message[1024];

const int pinLed = 13;
char phone_number_list[5][20];
int top_phone = 0;
String str_buffer;
void sendMsg(char p_num[], String msg);
bool test = 1;
int mytime = 1000;

static void sendMsgToAllContacts(String msg){
  for(int i = 0; i<2; i++){
    sendMsg(phone_number_list[i],msg);
  }  
}

//gps method
static unsigned char getComma(unsigned char num,const char *str)
{
  unsigned char i,j = 0;
  int len=strlen(str);
  for(i = 0;i < len;i ++)
  {
     if(str[i] == ',')
      j++;
     if(j == num)
      return i + 1; 
  }
  return 0; 
}
//gps method
static double getDoubleNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atof(buf);
  return rev; 
}

//gps method
static double getIntNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atoi(buf);
  return rev; 
}

//gps method
void parseGPGGA(const char* GPGGAstr)
{
  /* Refer to http://www.gpsinformation.org/dale/nmea.htm#GGA
   * Sample data: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
   * Where:
   *  GGA          Global Positioning System Fix Data
   *  123519       Fix taken at 12:35:19 UTC
   *  4807.038,N   Latitude 48 deg 07.038' N
   *  01131.000,E  Longitude 11 deg 31.000' E
   *  1            Fix quality: 0 = invalid
   *                            1 = GPS fix (SPS)
   *                            2 = DGPS fix
   *                            3 = PPS fix
   *                            4 = Real Time Kinematic
   *                            5 = Float RTK
   *                            6 = estimated (dead reckoning) (2.3 feature)
   *                            7 = Manual input mode
   *                            8 = Simulation mode
   *  08           Number of satellites being tracked
   *  0.9          Horizontal dilution of position
   *  545.4,M      Altitude, Meters, above mean sea level
   *  46.9,M       Height of geoid (mean sea level) above WGS84
   *                   ellipsoid
   *  (empty field) time in seconds since last DGPS update
   *  (empty field) DGPS station ID number
   *  *47          the checksum data, always begins with *
   */
  double latitude;
  double longitude;
  int tmp, hour, minute, second, num ;
  if(GPGGAstr[0] == '$')
  {
    tmp = getComma(1, GPGGAstr);
    hour     = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
    minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
    second    = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');
    
    sprintf(buff, "UTC timer %2d-%2d-%2d", hour, minute, second);
    Serial.println(buff);
    sprintf(message,buff);
    tmp = getComma(2, GPGGAstr);
    latitude = getDoubleNumber(&GPGGAstr[tmp]);
    tmp = getComma(4, GPGGAstr);
    longitude = getDoubleNumber(&GPGGAstr[tmp]);
    sprintf(buff, "latitude = %10.4f, longitude = %10.4f", latitude, longitude);
    Serial.println(buff);
    sprintf(message,buff); 
    
    tmp = getComma(7, GPGGAstr);
    num = getIntNumber(&GPGGAstr[tmp]);    
    sprintf(buff, "satellites number = %d", num);
    Serial.println(buff);
    sprintf(message,buff); 
    sendMsgToAllContacts(message);
  }
  else
  {
    Serial.println("Not get data"); 
  }
}


void handleMsg(char dtaget[], char p_num[]){

  char * piece;
  piece = strtok(dtaget,"\n");
  if(!strcmp(piece,"[setting]")){
    for(int i = 0; i < 3; i++){
      piece = strtok(NULL,"\n");
      if(i<=1)
        strcpy(phone_number_list[i],piece);
      else mytime = atoi(piece)*1000;
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
  LGPS.powerOn();//gps setting
  Serial.println("LGPS power on, and waiting");
  
  //while(!LSMS.ready()){
  //  delay(100);  
  //}
  Serial.println("GSM OK!!");
  pinMode(4,INPUT);
  pinMode(13,OUTPUT);
}

void sendgpsInfo(){
  LGPS.powerOn();
  LGPS.getData(&info);
  parseGPGGA((const char*)info.GPGGA);
}
void printSettingInfo(){
  Serial.println(phone_number_list[0]);
  Serial.println(phone_number_list[1]);
}
int timer = 0;
void loop() {
  timer ++;
  int switc = digitalRead(4);
  Serial.println(switc);
  if(switc){
    digitalWrite(13,LOW);

  }else{
    if(timer >= mytime){
      sendgpsInfo();
      timer = 0;
    }
    digitalWrite(13,HIGH);
  }
  // put your main code here, to run repeatedly:
  char p_num[20];
  int len = 0;
  char dtaget[500];
//  Serial.println(LSMS.available());
  
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
    handleMsg(dtaget,p_num);
    Serial.println(dtaget); 
    LSMS.flush();
  }
}


