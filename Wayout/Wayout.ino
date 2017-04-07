#include <Keypad.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include <EEPROM.h>
#include <ArduinoJson.h>

#define KEY_ROWS 4 // 按鍵模組的列數
#define KEY_COLS 4 // 按鍵模組的行數
#define led 5
#define package1_status 22
#define package2_status 23

StaticJsonBuffer<200> jsonBuffer;

MPU6050 accelgyro;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int value;
int degree_eq=0;

// 依照行、列排列的按鍵字元（二維陣列）
char keymap[KEY_ROWS][KEY_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
int i=0,j=0,k=0;

byte colPins[KEY_COLS] = {9, 8, 7, 6};     // 按鍵模組，行1~4接腳。
byte rowPins[KEY_ROWS] = {13, 12, 11, 10}; // 按鍵模組，列1~4接腳。 
char code [6]={'1','2','3','4','5','6'}; 
char command;
String apptype="";
char appcod [6];
char charstr_chang [1];

void resetcode(void);
void getvalue_fromapp(void);
void store(void);
void opendoor(void);
void acquire(void);
// 初始化Keypad物件
// 語法：Keypad(makeKeymap(按鍵字元的二維陣列), 模組列接腳, 模組行接腳, 模組列數, 模組行數)
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, KEY_ROWS, KEY_COLS);
 
void setup(){
  /*EEPROM.write(5,0);
  EEPROM.write(6,0);
  EEPROM.write(7,0);
  EEPROM.write(8,0);
  EEPROM.write(9,0);
  EEPROM.write(10,0);*/
  pinMode(led,OUTPUT);
  pinMode(package1_status,INPUT);
  pinMode(package2_status,INPUT);
  Wire.begin();
  Serial.begin(9600);
  accelgyro.initialize();
  digitalWrite(led,1);
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
    getvalue_fromapp();
    //Serial.print(code);
    if(apptype=="setpwd"){
      for(int s=0;s<6;s++){
        code[s]=appcod[s];    
        }
      store(); 
      Serial.write("finished");}
        
    if(apptype=="chkpwd"){
      int chkpwd=0;
      for(int s=0;s<6;s++){
        if(code[s]==appcod[s]){chkpwd++;}    
       }
      if(chkpwd==6){Serial.write("correct");}
      else{Serial.write("error");}}

    if(apptype=="update"){
      JsonArray& jsarray = jsonBuffer.createArray();
      jsarray.add(digitalRead(package1_status)==1);
      jsarray.add(digitalRead(package2_status)==1);
      jsarray.add(1==0);
      jsarray.add(1==0);
      jsarray.printTo(Serial);
      }
      
    char key1 = myKeypad.getKey();
 
    if(key1){
      if(key1==code[i]){i++;}
      else{i=0;}}

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
  opendoor();
}
  store();
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
  while(Serial.available() > 0)
     {  
      command = ((byte)Serial.read());
      //Serial.print(command);
      if(command == ':'){
        p++;}
      else{
        if(p==0){apptype += command;}
        if(p==1){appcod[m] = command;  m++;}}
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

void opendoor(void){
 if(i==6||value>=(10+18*degree_eq)){
  digitalWrite(led,0);
  delay(1500);
  i=0;
  value=0;
  digitalWrite(led,1);}
  return;}

