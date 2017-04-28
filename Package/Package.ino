static int ledStatus;
static int voiceStatus;
static unsigned long lastDebounceTime;
#define DEBOUNCE_DELAY 800
#define ledswitch 12
#define voiceswitch 9
#define voice 2
#define led1 3
#define led2 4
#define switchled 10

void openled(void);
void openvoice(void);

void setup(){
  Serial.begin(9600);
  pinMode(ledswitch, INPUT);
  pinMode(voiceswitch, INPUT);
  pinMode(voice, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(switchled, OUTPUT);
  
  ledStatus = HIGH;
  voiceStatus = LOW;
  digitalWrite(led1, ledStatus);
  digitalWrite(led2, ledStatus);
  digitalWrite(voice, voiceStatus);
  digitalWrite(switchled, 1);
}



void loop(){
 int switchledStatus = 0;
 switchledStatus = digitalRead(ledswitch);
 int switchvoiceStatus = 0;
 switchvoiceStatus = digitalRead(voiceswitch);
      Serial.print(switchledStatus);
  if(switchledStatus == HIGH){
    openled();
  }
  else if(switchvoiceStatus == HIGH){
    openvoice();
  }
}

void openled(){
  unsigned long currentTime = millis();
  if((currentTime - lastDebounceTime) > DEBOUNCE_DELAY){
    lastDebounceTime = currentTime;
   
    ledStatus = ledStatus == HIGH ? LOW : HIGH;
    digitalWrite(led1, ledStatus);
    digitalWrite(led2, ledStatus);
  }
}

void openvoice(){
  unsigned long currentTime = millis();
  if((currentTime - lastDebounceTime) > DEBOUNCE_DELAY){
    lastDebounceTime = currentTime;
   
    voiceStatus = voiceStatus == HIGH ? LOW : HIGH;
    if(voiceStatus==1){
      tone(voice,20000,0);
      digitalWrite(switchled, 0);}
    else{
      noTone (voice);
      digitalWrite(switchled, 1);}
  }
}
