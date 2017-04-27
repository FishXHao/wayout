static int ledStatus;
static int voiceStatus;
static unsigned long lastDebounceTime;
#define DEBOUNCE_DELAY 800
#define ledswitch 11
#define voiceswitch 9
#define voice 2
#define led 4
#define switchled 10

void openled(void);
void openvoice(void);

void setup(){
  Serial.begin(9600);
  pinMode(ledswitch, INPUT);
  pinMode(voiceswitch, INPUT);
  pinMode(voice, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(switchled, OUTPUT);
  
  ledStatus = HIGH;
  voiceStatus = LOW;
  digitalWrite(led, ledStatus);
  digitalWrite(voice, voiceStatus);
  digitalWrite(switchled, 1);
}



void loop(){
 int switchledStatus = 0;
 switchledStatus = digitalRead(ledswitch);
 int switchvoiceStatus = 0;
 switchvoiceStatus = digitalRead(voiceswitch);
      Serial.print(switchvoiceStatus);
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
    digitalWrite(led, ledStatus);
  }
}

void openvoice(){
  unsigned long currentTime = millis();
  if((currentTime - lastDebounceTime) > DEBOUNCE_DELAY){
    lastDebounceTime = currentTime;
   
    voiceStatus = voiceStatus == HIGH ? LOW : HIGH;
    if(voiceStatus==1){
      tone(voice,5000,0);
      digitalWrite(switchled, 0);}
    else{
      noTone (voice);
      digitalWrite(switchled, 1);}
  }
}
