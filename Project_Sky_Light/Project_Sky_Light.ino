#include <BlynkSimpleEsp8266.h>

#define tpin A0
#define lpin D1
#define ppin D2
#define default_lighting_mode 1
#define fadeValue 30   // determines the fading animation smoothness along with the timer used with light()

int pin[3] = {D5,D6,D7};
int rgbWave[6] = {0,-512,-1023,1,1,1};   // From [0] to [2] saves the previous value. From [3] to [5] saves the wave state (rising or falling)
int rgbSelection[3];
int Breathing[5] = {0,0,0,1,0};
float temp[2];
//int rgb[3];
int lightingMode = default_lighting_mode;
bool ambientLight = true;
bool motion = false;
bool on = true;
bool pirArmed = true;

char ssid[] = "AHMED";
char pass[] = "01235000";
char auth[] = "qmsU01AN1dBjzbzC5oi8y0DrFNwNaQ7j";   //Auth Token : qmsU01AN1dBjzbzC5oi8y0DrFNwNaQ7j

SimpleTimer timer;

float naturalNum(float x){
  if(x >= 0)
    return x;
    return 0;  
  }

float limit(float x){ 
  if(x <= 1023)
    return x;
    return 0;  
  }
  
void light(){
  if(on){
    if(ambientLight){
      switch(lightingMode){
         case 1:
         staticLight();
          break;
         case 2:
         wave();
          break;
         case 3:
         tempLight();
           break;
         case 4:
         breathing();
            break;
           }}
    else if(!ambientLight){
      if(motion){
      switch(lightingMode){
         case 1:
         staticLight();
          break;
         case 2:
         wave();
          break;
         case 3:
         tempLight();
           break;
         case 4:
         breathing();
            break;
           }
      }
    }}
  else if(!on){
    for(int i = 0;i<3;i++){
    digitalWrite(pin[i],LOW);
    }
  }    
}

void breathing(){
  if(Breathing[4] == 0){
    for(int i= 0;i<3;i++){
      Breathing[i] = rgbSelection[i];
      Breathing[4] = 1;
      }}
  for(int i=0;i<3;i++){
  if (Breathing[3]){
   if (Breathing[i] < 1023){
    analogWrite(pin[i], limit(naturalNum(Breathing[i])));
    Breathing[i] += fadeValue;
  } if(Breathing[i] >= 1023) {
    analogWrite(pin[i], limit(naturalNum(Breathing[i])));
    Breathing[3] = 0;
  }} if(!Breathing[3]) {
    if(Breathing[i] > 0){
    analogWrite(pin[i], limit(naturalNum(Breathing[i])));
    Breathing[i] -= fadeValue;
  } else if(Breathing[i] <= 0) {
    analogWrite(pin[i], limit(naturalNum(Breathing[i])));
    rgbWave[3] = 1;
  }}} 
  }

void wave(){
  for(int i=0;i<3;i++){
  if (rgbWave[3+i]){
   if (rgbWave[i] < 1023){
    analogWrite(pin[i], limit(naturalNum(rgbWave[i])));
    rgbWave[i] += fadeValue;
  } if(rgbWave[i] >= 1023) {
    analogWrite(pin[i], limit(naturalNum(rgbWave[i])));
    rgbWave[3+i] = 0;
  }} if(!rgbWave[3+i]) {
    if(rgbWave[i] > 0){
    analogWrite(pin[i], limit(naturalNum(rgbWave[i])));
    rgbWave[i] -= fadeValue;
  } else if(rgbWave[i] <= 0) {
    analogWrite(pin[i], limit(naturalNum(rgbWave[i])));
    rgbWave[3+i] = 1;
  }}}
  }

void motionNotification(){
  if(motion){
  Blynk.notify("Motion detected in the vicinity!"); }
  }

void staticLight(){
   analogWrite(pin[0],rgbSelection[0]);
   analogWrite(pin[1],rgbSelection[1]);
   analogWrite(pin[2],rgbSelection[2]); 
//    for(int i = 0;i<3;i++){
//      analogWrite(pin[i],rgbSelection[i]); 
//      }
  }

void tempLight(){             
  int r,g,b;
  r = limit(((naturalNum(temp[1]-15))/10)*1023);
  g = min(limit(((naturalNum(temp[1]-5))/10)*1023), limit(((naturalNum(45-temp[1]))/10)*1023));
  b = limit(((naturalNum(35-temp[1]))/10)*1023);
   analogWrite(pin[0],r);
   analogWrite(pin[1],g);
   analogWrite(pin[2],b); 
  }

//void tempLight(){             
//  rgb[0] = limit(((naturalNum(temp[1]-15))/10)*1023);
//  rgb[1] = min(limit(((naturalNum(temp[1]-5))/10)*1023), limit(((naturalNum(45-temp[1]))/10)*1023));
//  rgb[2] = limit(((naturalNum(35-temp[1]))/10)*1023);
//  for(int i = 0;i<3;i++){
//    analogWrite(pin[0],rgb[i]);
//  }}

BLYNK_CONNECTED() {
    Blynk.syncAll();
}

BLYNK_WRITE(V4) {
  switch (param.asInt())
  {
    case 1: // Static light
      lightingMode = 1;
      Serial.println("Static light selected");
      break;
    case 2: // RGB wave
      lightingMode = 2;
      Serial.println("RGB wave selected");
      break;
    case 3: // Temprature dependent lighting mode
      lightingMode = 3;
      Serial.println("Temprature dependent lighting mode selected");
      break;
    case 4: // Breathing
      lightingMode = 4;
      Serial.println("Breathing selected");
      break;
    default:
      Serial.println("Unknown mode selected");
  }
    Breathing[4] = 0;
}

BLYNK_WRITE(V1){
    rgbSelection[0] = param[0].asInt();
    rgbSelection[1] = param[1].asInt();
    rgbSelection[2] = param[2].asInt();

//    for(int i = 0;i<3;i++){
//      rgbSelection[i] = param[i].asInt();
//      }
}

BLYNK_WRITE(V2){   
  on = param.asInt();
}

BLYNK_WRITE(V3){   
  pirArmed = param.asInt();
}

void temprature(){
  temp[0] = analogRead(tpin);
  temp[1] = (temp[0]/1024.0)*330;
  Blynk.virtualWrite(V0, temp[1]);
  Serial.println(temp[1]);
  }

void lightsOut(){
   ambientLight = digitalRead(lpin);
  }

void motionCheck(){
  if(pirArmed){
  motion = digitalRead(lpin); 
  } else {
    motion = false;
    }}
    
void setup(){
  Serial.begin(9600);
  pinMode(pin[0],OUTPUT);
  pinMode(pin[1],OUTPUT);
  pinMode(pin[2],OUTPUT);
  pinMode(tpin,INPUT);
  pinMode(lpin,INPUT);
  pinMode(ppin,INPUT);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(30, light);
  timer.setInterval(250, temprature);
  timer.setInterval(1000, motionCheck);
  timer.setInterval(1000, lightsOut);   
  timer.setInterval(10000, motionNotification);
}

void loop(){
  Blynk.run();
  timer.run();
}
