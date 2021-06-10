 //---------------------------------------------------------This code was made by --------------------------------------------
//--------------------------------------------------------------Hesham Hany--------------------------------------------------

#include <BlynkSimpleEsp8266.h>
#include <WiFiManager.h> 

#define temperatureSensorPin A0   //temprature sensor pin
#define default_lighting_mode 1
#define fadeValue 20   // determines the fading animation smoothness along with the timer used with light()
#define breathing_fade 20
#define waveMaxVal 2026
#define waveMinVal -1023

int pin[3] = {D5,D6,D7};
int RGBwaveCoordinates[6] = {2026,0,0,0,0,1};   // From [0] to [2] saves the previous value. From [3] to [5] saves the wave state (rising or falling)
int rgbSelection[3];
int randomRGB[3];
int Breathing[2] = {0,0};
float temp;
int lightingMode = default_lighting_mode;
bool on = true;
int colors[9][3] = {{1023,1023,1023} //  White
                  ,{1023,0,0} //  Red
                  ,{0,1023,0} //  Green
                  ,{0,0,1023} //  Blue
                  ,{1023,0,1023} //  Purple
                  ,{1023,1023,0} //  Yellow
                  ,{0,1023,1023} //  Cyan
                  ,{1023,40,0} //  Orange
                  ,{1023,0,50} };//  Pink 
                  
char auth[] = "KoVNbPQ5qrZS0uxW1sDrGFX2K6ssVlwI";   //Auth Token 

BlynkTimer timer;

float limit(float x){    //returns 1023 if the number is greater than 1023, returns 0 if the number is negative
  if(x <= 1023){
    if(x >= 0){
      return x;
      } else {
        return 0;
        }
    } else {
    return 1023;  
}
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

void randomBreathing(){   //breathing effect with random colors
  if (Breathing[1]){
   if (Breathing[0] < 1023){
        for(int i = 0;i<3;i++){
         analogWrite(pin[i],(randomRGB[i] * Breathing[0])/1023); 
         }
    Breathing[0] += breathing_fade;
  } if(Breathing[0] >= 1023) {
    for(int i = 0;i<3;i++){
         analogWrite(pin[i],(randomRGB[i] * Breathing[0])/1023); 
         }
    Breathing[1] = 0;
  }} if(!Breathing[1]) {
    if(Breathing[0] > 0){
    for(int i = 0;i<3;i++){
         analogWrite(pin[i],(randomRGB[i] * Breathing[0])/1023); 
         }
    Breathing[0] -= breathing_fade;
  } else if(Breathing[0] <= 0) {
    for(int i = 0;i<3;i++){
         analogWrite(pin[i],(randomRGB[i] * Breathing[0])/1023); 
         }
     for(int i = 0;i<3;i++){
     int j = random(0,8);
     randomRGB[i] = colors[j][i];
     }
    Breathing[1] = 1;
  }}}

void colorSwitch(){   //Switches colors randomly
  if (Breathing[1]){
   if (Breathing[0] < 1023){
    Breathing[0] += breathing_fade;
  } if(Breathing[0] >= 1023) {
    Breathing[1] = 0;
  }} if(!Breathing[1]) {
    if(Breathing[0] > 0){
    Breathing[0] -= breathing_fade;
  } else if(Breathing[0] <= 0) {
     for(int i = 0;i<3;i++){
     int j = random(0,8);
     randomRGB[i] = colors[j][i];
     }
     for(int i = 0;i<3;i++){
         digitalWrite(pin[i],randomRGB[i]); 
         }
    Breathing[1] = 1;
  }}}

void wave(){    //RGB wave function
  for(int i=0;i<3;i++){
  if (RGBwaveCoordinates[3+i]){
   if (RGBwaveCoordinates[i] < waveMaxVal){
    analogWrite(pin[i], limit(RGBwaveCoordinates[i]));
    RGBwaveCoordinates[i] += fadeValue;
  } if(RGBwaveCoordinates[i] >= waveMaxVal) {
    analogWrite(pin[i], limit(RGBwaveCoordinates[i]));
    RGBwaveCoordinates[3+i] = 0;
  }} if(!RGBwaveCoordinates[3+i]) {
    if(RGBwaveCoordinates[i] > waveMinVal){
    analogWrite(pin[i], limit(RGBwaveCoordinates[i]));
    RGBwaveCoordinates[i] -= fadeValue;
  } else if(RGBwaveCoordinates[i] <= waveMinVal) {
    analogWrite(pin[i], limit(RGBwaveCoordinates[i]));
    RGBwaveCoordinates[3+i] = 1;
  }}}
  }

void staticLight(){   //static light mode function
    for(int i = 0;i<3;i++){
      analogWrite(pin[i],rgbSelection[i]); 
      }
}

void tempLight(){             //temprature dependent light mode function
   analogWrite(pin[0], limit(((temp-20)/5)*1023));
   analogWrite(pin[1], min(limit(((temp-15)/5)*1023), limit(((35-temp)/5)*1023)));
   analogWrite(pin[2], limit(((30-temp)/5)*1023)); 
  }

BLYNK_CONNECTED() {   //syncs the nodeMCU with the app on startup
    Blynk.syncAll();
}
  
BLYNK_WRITE(V1){ // Zebra rgb 
    for(int i = 0;i<3;i++){
      rgbSelection[i] = param[i].asInt();
      }
}

BLYNK_WRITE(V2){   //On switch
  on = param.asInt();
}

BLYNK_WRITE(V3) {  //light modes list 
  lightingMode = param.asInt();
}

BLYNK_WRITE(V4) {  //reset wifi credentials 
  int reset = param.asInt();
  if(reset == 7){
    WiFiManager reset;
    reset.resetSettings();
    ESP.restart(); 
    }
}

void temprature(){
  temp = (analogRead(temperatureSensorPin)/1024.0)*330;
  Blynk.virtualWrite(V0, temp); 
  }

void light(){
  if(on){
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
         case 5:
         randomBreathing();
            break;
         case 6:
         colorSwitch();
            break;
           }} else if(!on){
    for(int i = 0;i<3;i++){
    digitalWrite(pin[i],LOW);
    }
  }    
}
  
void setup(){
  pinMode(pin[0],OUTPUT);
  pinMode(pin[1],OUTPUT);
  pinMode(pin[2],OUTPUT);
  pinMode(temperatureSensorPin,INPUT);
  WiFiManager wifiManager;
  wifiManager.autoConnect("Sky light");
  WiFi.mode(WIFI_STA);
  Blynk.config(auth);
  timer.setInterval(30, light);
  timer.setInterval(250, temprature);
}

void loop(){
  Blynk.run();
  timer.run();
}
