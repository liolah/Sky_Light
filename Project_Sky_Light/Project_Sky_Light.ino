#include <BlynkSimpleEsp8266.h>

#define tpin A0   //temprature sensor pin
#define lpin D1   //light sensor pin
#define ppin D2   //pir sensor pin
#define default_lighting_mode 1
#define fadeValue 10   // determines the fading animation smoothness along with the timer used with light()
#define breathing_fade 30

int pin[3] = {D5,D6,D7};
int rgbWave[6] = {0,-512,-1023,1,1,1};   // From [0] to [2] saves the previous value. From [3] to [5] saves the wave state (rising or falling)
int rgbSelection[3];
int Breathing[2] = {0,1};
float temp[2];
//int rgb[3];
int lightingMode = default_lighting_mode;
bool ambientLight = true;
bool motion = false;
bool pmotion = false;
bool on = true;
bool pirArmed = true;

char ssid[] = "AHMED";
char pass[] = "01235000";
char auth[] = "qmsU01AN1dBjzbzC5oi8y0DrFNwNaQ7j";   //Auth Token : qmsU01AN1dBjzbzC5oi8y0DrFNwNaQ7j

SimpleTimer timer;

float naturalNum(float x){    //returns positive numbers only, if the number is negative returns 0
  if(x >= 0)
    return x;
    return 0;  
  }

float limit(float x){    //returns 1023 if the number is greater than 1023
  if(x <= 1023)
    return x;
    return 1023;  
  }

void breathing(){   //breathing effect function
  if (Breathing[1]){
   if (Breathing[0] < 1023){
        for(int i = 0;i<3;i++){
         analogWrite(pin[i],(rgbSelection[i] * Breathing[0])/1023); 
         }
    Breathing[0] += breathing_fade;
  } if(Breathing[0] >= 1023) {
    for(int i = 0;i<3;i++){
         analogWrite(pin[i],(rgbSelection[i] * Breathing[0])/1023); 
         }
    Breathing[1] = 0;
  }} if(!Breathing[1]) {
    if(Breathing[0] > 0){
    for(int i = 0;i<3;i++){
         analogWrite(pin[i],(rgbSelection[i] * Breathing[0])/1023); 
         }
    Breathing[0] -= breathing_fade;
  } else if(Breathing[0] <= 0) {
    for(int i = 0;i<3;i++){
         analogWrite(pin[i],(rgbSelection[i] * Breathing[0])/1023); 
         }
    Breathing[1] = 1;
  }}}


void wave(){    //RGB wave function
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

void motionNotification(){    //sends a notification to the app if motion detected
 if(pirArmed){
  if(motion){
  Blynk.notify("Motion detected in the vicinity!"); }
  }}

void staticLight(){   //static light mode function
   analogWrite(pin[0],rgbSelection[0]);
   analogWrite(pin[1],rgbSelection[1]);
   analogWrite(pin[2],rgbSelection[2]); 
//    for(int i = 0;i<3;i++){
//      analogWrite(pin[i],rgbSelection[i]); 
//      }
  }

void tempLight(){             //temprature dependent light mode function
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

BLYNK_CONNECTED() {   //syncs the nodeMCU with the app on startup
    Blynk.syncAll();
}

BLYNK_WRITE(V4) {  //light modes list 
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
}

BLYNK_WRITE(V1){ // Zebra rgb 
    rgbSelection[0] = param[0].asInt();
    rgbSelection[1] = param[1].asInt();
    rgbSelection[2] = param[2].asInt();
    Breathing[4] = 0;

//    for(int i = 0;i<3;i++){
//      rgbSelection[i] = param[i].asInt();
//      }
//      Breathing[4] = 0;
}

BLYNK_WRITE(V2){   //On switch
  on = param.asInt();
}

BLYNK_WRITE(V3){   //Armed switch
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
  if(digitalRead(ppin)){
     motion = 1; }
  else {
     pmotion = 0;
    }
    }

void motionStill(){
  motion = digitalRead(ppin); 
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
      } else {
        for(int i = 0;i<3;i++){
    digitalWrite(pin[i],LOW);
    }      
    }}}
  else if(!on){
    for(int i = 0;i<3;i++){
    digitalWrite(pin[i],LOW);
    }
  }    
}

void motionChange(){
  if(pmotion == 0 && motion == 1){
    motionNotification(); 
    pmotion = 1;
  }
  }
  
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
  timer.setInterval(500, motionCheck);
  timer.setInterval(60000, motionStill);
  timer.setInterval(500, lightsOut);   
  timer.setInterval(30000, motionChange);
}

void loop(){
  Blynk.run();
  timer.run();
}
