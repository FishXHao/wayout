#include <Keypad.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include <EEPROM.h>
#include <ArduinoJson.h>

static int ledStatus;
static unsigned long lastDebounceTime;

#define DEBOUNCE_DELAY 400
#define KEY_ROWS 4 // 按鍵模組的列數
#define KEY_COLS 4 // 按鍵模組的行數
#define doorlock 36
#define package1_status 44
#define package2_status 42
#define button_open 46
#define opendoor 26
#define closedoor 25
#define opendrawer 27
#define closedrawer 29
#define led 31
#define doorlocksignal 38

StaticJsonBuffer<200> jsonBuffer;
JsonArray& jsarray = jsonBuffer.createArray();

MPU6050 accelgyro;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int value;
int degree_eq=6;
                       
// 依照行、列排列的按鍵字元（二維陣列）
char keymap[KEY_ROWS][KEY_COLS] = {
  {'D', '#', '0', '*'},
  {'C', '9', '8', '7'},
  {'B', '6', '5', '4'},
  {'A', '3', '2', '1'}
};
int i=0,j=0,k=0;

byte colPins[KEY_COLS] = {9, 8, 7, 6};     // 按鍵模組，行1~4接腳。
byte rowPins[KEY_ROWS] = {13, 12, 11, 10}; // 按鍵模組，列1~4接腳。 
char code [6] = {'1','2','3','4','5','6'}; 
char command;
String apptype = "";
char appcod [6];
char charstr_chang [1];
int gout = 1; 

int openclosecount=0;

void resetcode(void);
void getvalue_fromapp(void);
void store(void);
void opendoor_keypad(void);
void opendoor_eq(void);
void closeall(void);
void acquire(void);
void debounce(void);
// 初始化Keypad物件
// 語法：Keypad(makeKeymap(按鍵字元的二維陣列), 模組列接腳, 模組行接腳, 模組列數, 模組行數)
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, KEY_ROWS, KEY_COLS);
 
void setup(){
  /*EEPROM.write(10,'1');
  EEPROM.write(11,'2');
  EEPROM.write(12,'3');
  EEPROM.write(13,'4');
  EEPROM.write(14,'5');
  EEPROM.write(15,'6');*/
  pinMode(opendoor,OUTPUT);
  pinMode(closedoor,OUTPUT);
  pinMode(opendrawer,OUTPUT);
  pinMode(closedrawer,OUTPUT);
  pinMode(doorlock,OUTPUT);
  pinMode(led,OUTPUT);
  pinMode(package1_status,INPUT);
  pinMode(package2_status,INPUT);
  pinMode(button_open,INPUT);
  pinMode(doorlocksignal,INPUT);
  Wire.begin();
  Serial2.begin(9600);
  accelgyro.initialize();
  digitalWrite(doorlock,1);
  digitalWrite(opendoor,0);
  digitalWrite(closedoor,1);
  digitalWrite(closedrawer,0);
  digitalWrite(opendrawer,1);
  digitalWrite(led,0);
  jsarray.add(1==0);
  jsarray.add(1==0);
  jsarray.add(1==0);
  jsarray.add(1==0);
}
  
void loop(){ 
 
  acquire();
  
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  gx=(gx+160)/100;
  gy=(gy-70)/100; 
  gz=(gz-80)/100;
  value= sqrt((gx*gx)+(gy*gy)+(gz*gz));
  
  for(int l = 0; l < LIST_MAX; l++) 
    {
    int switchStatus = 0;
    //Serial.print(switchStatus);
    switchStatus = digitalRead(button_open);
    if(switchStatus == 1){debounce();} 

    //Serial.print(code);
    
    getvalue_fromapp();    
    if(apptype=="setpwd"){
      for(int s=0;s<6;s++){
        code[s]=appcod[s];    
        }
      store(); 
      Serial2.write("finished");}
        
    if(apptype=="chkpwd"){
      int chkpwd=0;
      for(int s=0;s<6;s++){
        if(code[s]==appcod[s]){chkpwd++;}    
       }
      if(chkpwd==6){Serial2.write("correct");}
      else{Serial2.write("error");}}

    if(apptype=="update"){
      jsarray[2] = (digitalRead(package1_status)==1);
      jsarray[4] = (digitalRead(package2_status)==1);
      jsarray.printTo(Serial2);
      }
      
    char key1 = myKeypad.getKey();
    //Serial.print(key1);
    if(key1){
      if(key1==code[i]){i++;}
      else{i=0;}
      if(key1=='A'){
        if(gout==1){gout=0;}
        else{gout++;}
    }}
//Serial.print(gout);
    if(myKeypad.key[l].kchar=='*'){
      switch(myKeypad.key[l].kstate){
        case HOLD:
          resetcode(); 
          store();
          break;}}
    if(myKeypad.key[l].kchar=='#'){
      switch(myKeypad.key[l].kstate){
        case HOLD:  
          while(1){            
            char key3 = myKeypad.getKey();
            if(key3){
              charstr_chang[0] = key3;
              degree_eq=atoi(charstr_chang);
              if(degree_eq<=9&&degree_eq>=0){break;}}}
          break;}}  
      //Serial.print(degree_eq);   
    if(digitalRead(doorlocksignal) == 1){i==6;}
    opendoor_keypad();
    opendoor_eq();
}
}

void resetcode(void){
  do{
    char key2 = myKeypad.getKey();
    if(key2){
      code[k]=key2;
      k++;}
        }while(k<6);
    k=0;
  return;}

void getvalue_fromapp(void){
  int m=0,p=0;
  apptype="";
  while(Serial2.available() > 0)
     {  
      command = ((byte)Serial2.read());
      //Serial.print(command);
      if(command == ':'){
        p++;}
      else{
        if(p==0){apptype += command;}
        if(p==1){appcod[m] = command;  m++;}}
        //Serial2.print(command);
      delay(1);}
  return;}

void acquire(void){
  code[0]=EEPROM.read(10);
  code[1]=EEPROM.read(11);
  code[2]=EEPROM.read(12);
  code[3]=EEPROM.read(13);
  code[4]=EEPROM.read(14);
  code[5]=EEPROM.read(15);
  return;}

void store(void){
  EEPROM.write(10,code[0]);
  EEPROM.write(11,code[1]);
  EEPROM.write(12,code[2]);
  EEPROM.write(13,code[3]);
  EEPROM.write(14,code[4]);
  EEPROM.write(15,code[5]);
  return;}

void opendoor_keypad(void){
 if(i==6){
  digitalWrite(doorlock,0);
  delay(1000);
  digitalWrite(opendoor,1);
  digitalWrite(closedoor,0);
  i=0;
  value=0;
  openclosecount=1;
  delay(1500);
  digitalWrite(doorlock,1);}
  return;}

void opendoor_eq(void){
 if(((value*gout)>=(10+18*degree_eq))){
  digitalWrite(doorlock,0);
  digitalWrite(closedrawer,1);
  digitalWrite(opendrawer,0);
  delay(1000);
  digitalWrite(opendoor,1);
  digitalWrite(closedoor,0);
  digitalWrite(led,1);
  delay(1500);
  digitalWrite(doorlock,1);
  i=0;
  value=0;
  openclosecount=1;}
  return;}

void debounce(void){
  unsigned long currentTime = millis();
  if((currentTime - lastDebounceTime) > DEBOUNCE_DELAY){
    //Serial.print(true);
    lastDebounceTime = currentTime;
    if(openclosecount==0){
      i=6;
      opendoor_keypad();
      openclosecount=1;
      }
    else{
      closeall();
      openclosecount=0;}
  }
}

void closeall(void){
  digitalWrite(opendoor,0);
  digitalWrite(closedoor,1);
  digitalWrite(closedrawer,0);
  digitalWrite(opendrawer,1);
  digitalWrite(led,0);
  i=0;
}

