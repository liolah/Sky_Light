//---------------------------------------------------------This code was made by --------------------------------------------
//--------------------------------------------------------------Hesham Hany--------------------------------------------------
//-------------------------------------------------Special thanks to Nebuz, LEVW, and pierre---------------------------------

#include <BlynkSimpleEsp8266.h>

#define temperatureSensorPin A0   //temprature sensor pin
#define default_lighting_mode 1
#define fadeValue 10   // determines the fading animation smoothness along with the timer used with light()
#define breathing_fade 20

int pin[3] = {D5,D6,D7};
int RGBwaveCoordinates[6] = {0,-512,-1023,1,1,1};   // From [0] to [2] saves the previous value. From [3] to [5] saves the wave state (rising or falling)
int rgbSelection[3];
int Breathing[2] = {0,1};
float temp[2];
int lightingMode = default_lighting_mode;
bool on = true;

char ssid[] = "AHMED";                               //network SSID Token
char pass[] = "01235000";                           //network password Token
char auth[] = "qmsU01AN1dBjzbzC5oi8y0DrFNwNaQ7j";   //Auth Token 

SimpleTimer timer;

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


void wave(){    //RGB wave function
  for(int i=0;i<3;i++){
  if (RGBwaveCoordinates[3+i]){
   if (RGBwaveCoordinates[i] < 1023){
    analogWrite(pin[i], limit(RGBwaveCoordinates[i]));
    RGBwaveCoordinates[i] += fadeValue;
  } if(RGBwaveCoordinates[i] >= 1023) {
    analogWrite(pin[i], limit(RGBwaveCoordinates[i]));
    RGBwaveCoordinates[3+i] = 0;
  }} if(!RGBwaveCoordinates[3+i]) {
    if(RGBwaveCoordinates[i] > 0){
    analogWrite(pin[i], limit(RGBwaveCoordinates[i]));
    RGBwaveCoordinates[i] -= fadeValue;
  } else if(RGBwaveCoordinates[i] <= 0) {
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
   analogWrite(pin[0], limit(((temp[1]-15)/10)*1023));
   analogWrite(pin[1], min(limit(((temp[1]-5)/10)*1023), limit(((45-temp[1])/10)*1023)));
   analogWrite(pin[2], limit(((35-temp[1])/10)*1023)); 
  }

BLYNK_CONNECTED() {   //syncs the nodeMCU with the app on startup
    Blynk.syncAll();
}

BLYNK_WRITE(V3) {  //light modes list 
  lightingMode = param.asInt();
}
  
BLYNK_WRITE(V1){ // Zebra rgb 
    for(int i = 0;i<3;i++){
      rgbSelection[i] = param[i].asInt();
      }
}

BLYNK_WRITE(V2){   //On switch
  on = param.asInt();
}

void temprature(){
  temp[0] = analogRead(temperatureSensorPin);
  temp[1] = (temp[0]/1024.0)*330;
  Blynk.virtualWrite(V0, temp[1]); 
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
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(30, light);
  timer.setInterval(250, temprature);
}

void loop(){
  Blynk.run();
  timer.run();
}
